// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "generator.h"
#include "renderer.h"

#include <qtsmithy/model.h>

#include <QDebug>
#include <QDirIterator>
#include <QJsonParseError>
#include <QRegularExpression>

#define QSL(str) QStringLiteral(str) // Shorten the QStringLiteral macro for readability.

class ScopedContext
{
public:
    ScopedContext(Renderer * renderer, const QVariantHash &hash) : renderer(renderer)
    {
        renderer->push(hash);
    }

    ScopedContext() {
        renderer->pop();
    }

private:
    Renderer * renderer;
};


const QRegularExpression Generator::servicePattern{
    QSL("(?<type>Service)(?<seperator>[^a-zA-Z0-9]*)(?<id>Title|Id|sdkId)"),
    QRegularExpression::CaseInsensitiveOption
};

const QRegularExpression Generator::operationPattern{
    QSL("(?<type>Operation)(?<seperator>[^a-zA-Z0-9]*)(?<id>Name)"),
    QRegularExpression::CaseInsensitiveOption
};

Generator::Generator(const smithy::Model * const model, Renderer * const renderer)
    : model(model), renderer(renderer)
{
    Q_ASSERT(model->isValid());
    Q_ASSERT(renderer);
}


int Generator::expectedFileCount() const
{
    const QHash<smithy::ShapeId, smithy::Shape> services = model->shapes(smithy::Shape::Type::Service);
    const int operations = std::accumulate(services.constBegin(), services.constEnd(), 0,
        [](const int a, const smithy::Shape &shape) { return a + shape.operations().size();
    });
    const QStringList templates = renderer->templatesNames();
    return std::accumulate(templates.constBegin(), templates.constEnd(), 0,
        [&](const int a, const QString &name) {
            return a + (servicePattern.match(name).hasMatch() ?
                (operationPattern.match(name).hasMatch() ? operations : services.size()) : 1);
        });
}

bool Generator::generate(const QString &outputDir, ClobberMode clobberMode)
{
    // Add initial context.
    QVariantMap servicesMap;
    const QHash<smithy::ShapeId, smithy::Shape> services = model->shapes(smithy::Shape::Type::Service);
    for (const smithy::Shape &service: services) {
        servicesMap.insert(service.id().toString(), toContext(service));
    }
    const ScopedContext context(renderer, { { QSL("services"), servicesMap } });

    // Build the list of template names.
    const QStringList templatesNames = renderer->templatesNames();
    QStringList serviceTemplateNames, operationTemplateNames, plainTemplateNames;
    for (const QString &name: templatesNames) {
        ((servicePattern.match(name).hasMatch()) ? (operationPattern.match(name).hasMatch() ?
            operationTemplateNames : serviceTemplateNames) : plainTemplateNames).append(name);
    }
    Q_ASSERT(serviceTemplateNames.size() + operationTemplateNames.size()
             + plainTemplateNames.size() == templatesNames.size());

    // Render all of the services.
    for (const smithy::Shape &service: services) {
        if (!renderService(service, serviceTemplateNames, operationTemplateNames, outputDir, clobberMode)) {
            return false;
        }
    }

    // Render all of the one-off (not per-service) files.
    qCDebug(lc).noquote() << tr("Rendering %n unary template/s", nullptr, plainTemplateNames.size());
    return std::all_of(plainTemplateNames.constBegin(), plainTemplateNames.constEnd(),
        [&](const QString &templateName){
            const QString outputPathName = outputDir + QLatin1Char('/') + templateName;
            return render(templateName, outputPathName, clobberMode);
    });
}

QStringList Generator::renderedFiles() const
{
    return renderedPathNames;
}

QStringList Generator::skippedFiles() const
{
    return skippedPathNames;
}

bool Generator::renderService(const smithy::Shape &service,
                              const QStringList &serviceTemplateNames,
                              const QStringList &operationTemplateNames,
                              const QString &outputDir, ClobberMode &clobberMode)
{
    qCDebug(lc).noquote() << tr("Rendering templates for service %1").arg(service.id().toString());

    // Add renderer context for this service.
    const QVariantHash serviceContext = toContext(service);
    // cppcheck-suppress unreadVariable
    const ScopedContext context(renderer, { { QSL("service"), serviceContext } });

    // Render each service template.
    for (const QString &templateName: serviceTemplateNames) {
        const QString outputPathName = makeOutputPathName(templateName, servicePattern, {
            { QSL("name"), service.id().shapeName() },
            { QSL("id"), serviceContext.value(QSL("canonicalId")).toString() },
            { QSL("sdkid"), serviceContext.value(QSL("sdkId")).toString() },
        }, outputDir);
        if (!render(templateName, outputPathName, clobberMode)) {
            return false;
        }
    }

    // Render each of the service's operation.
    const smithy::Shape::ShapeReferences operations = service.operations();
    for (const smithy::Shape::ShapeReference &shapeRef: operations) {
        const smithy::Shape operation = model->shape(shapeRef.target);
        if (!operation.isValid()) {
            qCCritical(lc).noquote() << tr("Failed to find shape for %1 operation in %2 service")
                .arg(shapeRef.target.toString(), service.id().toString());
            return false;
        }
        if (!renderOperation(operation, serviceContext, operationTemplateNames, outputDir, clobberMode)) {
            return false;
        }
    }
    return true;
}

bool Generator::renderOperation(const smithy::Shape &operation, const QVariantHash serviceContext,
                                const QStringList &templateNames, const QString &outputDir,
                                ClobberMode &clobberMode)
{
    qCDebug(lc).noquote() << tr("Rendering templates for operation %1").arg(operation.id().toString());

    // Add renderer context for this operation.
    // cppcheck-suppress unreadVariable
    const ScopedContext context(renderer, { { QSL("operation"), toContext(operation) } });
    for (const QString &templateName: templateNames) {
        QString outputPathName = makeOutputPathName(templateName, servicePattern, {
            { QSL("name"), serviceContext.value(QSL("shapeName")).toString() },
            { QSL("id"), serviceContext.value(QSL("canonicalId")).toString() },
            { QSL("sdkid"), serviceContext.value(QSL("sdkId")).toString() },
        });
        outputPathName = makeOutputPathName(outputPathName, operationPattern, {
            { QSL("name"), operation.id().shapeName() },
        }, outputDir);
        if (!render(templateName, outputPathName, clobberMode)) {
            return false;
        }
    }
    return true;
}

bool Generator::render(const QString &templateName, const QString &outputPathName,
                       ClobberMode &clobberMode)
{
    if (QFile::exists(outputPathName)) {
        switch (clobberMode) {
        case ClobberMode::Overwrite:
            qCDebug(lc) << tr("Overwriting existing file: %1").arg(outputPathName);
            break;
        case ClobberMode::Prompt:
            if (promptToOverwrite(outputPathName, clobberMode)) {
                break; // User said yes, so jump out of case statement.
            }
            __attribute__((fallthrough));
        case ClobberMode::Skip:
            qCDebug(lc) << tr("Skipping existing output file: %1").arg(outputPathName);
            skippedPathNames.append(outputPathName);
            return true;
        }
    }
    if (!renderer->render(templateName, outputPathName)) {
        return false;
    }
    renderedPathNames.append(outputPathName);
    return true;
}

// Turn shape into a context hash. Note, only service and operation shapes supported for now.
QVariantHash Generator::toContext(const smithy::Shape &shape) const
{
    if (shape.type() == smithy::Shape::Type::Service) {
        QVariantHash hash = shape.rawAst().toVariantHash();
        hash.insert(QSL("shapeName"), shape.id().shapeName());
        hash.insert(QSL("canonicalId"), canonicalServiceId(shape.traits()
            .value(QSL("aws.api#service")).toObject().value(QSL("sdkId")).toString()));
        hash.insert(QSL("sdkId"), shape.traits().value(QSL("aws.api#service")).toObject()
            .value(QSL("sdkId")).toString());
        hash.insert(QSL("documentation"), formatHtmlDocumentation(
            shape.traits().value(QSL("smithy.api#documentation")).toString()));
        QVariantMap operations;
        const smithy::Shape::ShapeReferences operationRefs = shape.operations();
        for (const smithy::Shape::ShapeReference &operationRef: operationRefs) {
            operations.insert(operationRef.target.shapeName(),
                              toContext(model->shape(operationRef.target)));
        }
        Q_ASSERT(operationRefs.size() == operations.size());
        const smithy::Shape::ShapeReferences resourceRefs = shape.resources();
        for (const smithy::Shape::ShapeReference &resourceRef: resourceRefs) {
            const QVariantHash resourceContext = toContext(model->shape(resourceRef.target));
            // resourceContext will contain the raw AST, and probably other things too over time,
            // but for now (to replicate historical aws-sdk-qt codegen behaviour) we just want to
            // add the resource operations to the service operations list.
            const QVariantMap resourceOperations = resourceContext.value(QSL("operations")).toMap();
            for (auto iter = resourceOperations.constBegin(); iter != resourceOperations.constEnd(); ++iter) {
                operations.insert(iter.key(), iter.value());
            }
        }
        hash.insert(QSL("operations"), operations);
        return hash;
    }

    if (shape.type() == smithy::Shape::Type::Operation) {
        QVariantHash hash = shape.rawAst().toVariantHash();
        hash.insert(QSL("name"), shape.id().shapeName());
        hash.insert(QSL("documentation"), formatHtmlDocumentation(
            shape.traits().value(QSL("smithy.api#documentation")).toString()));
        return hash;
    }

    if (shape.type() == smithy::Shape::Type::Resource) {
        QVariantHash hash = shape.rawAst().toVariantHash();
        QVariantMap operations;
        #define QTSMITHY_IF_VALID_INSERT(action) { \
            const smithy::ShapeId action##TargetId = shape.action().target; \
            if (action##TargetId.isValid()) {                   \
                operations.insert(action##TargetId.shapeName(), \
                    toContext(model->shape(action##TargetId))); \
            } \
        }
        QTSMITHY_IF_VALID_INSERT(create)
        QTSMITHY_IF_VALID_INSERT(put)
        QTSMITHY_IF_VALID_INSERT(read)
        QTSMITHY_IF_VALID_INSERT(update)
        QTSMITHY_IF_VALID_INSERT(Delete)
        QTSMITHY_IF_VALID_INSERT(list)
        #undef QTSMITHY_IF_VALID_INSERT

        #define QTSMITHY_ADD_SHAPES(property) { \
            const smithy::Shape::ShapeReferences refs = shape.property(); \
            for (const smithy::Shape::ShapeReference &ref: refs) { \
                operations.insert(ref.target.shapeName(), toContext(model->shape(ref.target))); \
            } \
        }
        QTSMITHY_ADD_SHAPES(operations)
        QTSMITHY_ADD_SHAPES(collectionOperations)
        #undef QTSMITHY_ADD_SHAPES

        const smithy::Shape::ShapeReferences resourceRefs = shape.resources();
        for (const smithy::Shape::ShapeReference &resourceRef: resourceRefs) {
            const QVariantHash resourceContext = toContext(model->shape(resourceRef.target));
            // resourceContext will contain the raw AST, and probably other things too over time,
            // but for now (to replicate historical aws-sdk-qt codegen behaviour) we just want to
            // add the resource operations to the service operations list.
            const QVariantMap resourceOperations = resourceContext.value(QSL("operations")).toMap();
            for (auto iter = resourceOperations.constBegin(); iter != resourceOperations.constEnd(); ++iter) {
                operations.insert(iter.key(), iter.value());
            }
        }

        hash.insert(QSL("operations"), operations);
        return hash;
    }

    qCCritical(lc).noquote() << tr("Cannot generate context for shape type 0x%1")
        .arg((int)shape.type(), 0, 16);
    return QVariantHash{};
}


const QString Generator::canonicalServiceId(const QString &sdkId)
{
    // Handle some hard-coded special cases that are hard to automate without a dictionary.
    const QMap<QString,QString> specialCases{
        { QSL("identitystore"   ), QSL("IdentityStore")    },
        { QSL("ivschat"         ), QSL("IvsChat")          },
        { QSL("forecastquery"   ), QSL("ForecastQuery")    },
        { QSL("resiliencehub"   ), QSL("ResilienceHub")    },
        { QSL("savingsplans"    ), QSL("SavingsPlans")     },
        { QSL("codeartifact"    ), QSL("CodeArtifact")     },
        { QSL("imagebuilder"    ), QSL("ImageBuilder")     },
        { QSL("billingconductor"), QSL("BillingConductor") },
    };
    for (const auto iter = specialCases.constFind(sdkId); iter != specialCases.constEnd(); ) {
        return iter.value();
    }

    // Start with the sdkId.
    QString id = sdkId;

    // Convert runs of upercase letters with first-upper, and the rest lower.
    auto iter = QRegularExpression(QSL("[A-Z]{2,}([^a-zA-Z]|$)")).globalMatch(id);
    while (iter.hasNext()) {
        const auto match = iter.next();
        id.replace(match.capturedStart()+1, match.capturedLength()-1,
                   match.captured().mid(1).toLower());
    }

    // If the sdkId was all lowercase, then uppercase the first letter of each word.
    if (id.isLower()) {
        const QStringList words = id.split(QRegularExpression(QSL("[^a-zA-Z0-9]+")));
        id.clear();
        for (const QString &word: words) {
            id.append(word.at(0).toUpper() + word.mid(1));
        }
    }

    // Strip all non-alphanumeric characters, and any instances of "Amazon" and "AWS".
    id.replace(QRegularExpression(QSL("[^a-zA-Z0-9]|Amazon|AWS"),
               QRegularExpression::PatternOption::CaseInsensitiveOption),QString());

    // Remove any trailing instances of "API", "Client" and "Service".
    if (id == QSL("ConfigService"))    return id; // Skip dropping the Service for this one.
    if (id == QSL("DirectoryService")) return id; // Skip dropping the Service for this one.
    id.replace(QRegularExpression(QSL("(API|Client|Service)$"),
               QRegularExpression::PatternOption::CaseInsensitiveOption),QString());
    return id;
}

bool Generator::promptToOverwrite(const QString &pathName, ClobberMode &clobberMode)
{
    Q_ASSERT(clobberMode == ClobberMode::Prompt);
    while (true) {
        qCWarning(lc).noquote() << tr("Overwrite %1 [y,n,a,s,q,?]? ").arg(pathName);
        QTextStream stream(stdin);
        const QString response = stream.readLine().toLower();
        if (response == QSL("y")) {
            return true;
        } else if (response == QSL("n")) {
            return false;
        } else if (response == QSL("a")) {
            clobberMode = ClobberMode::Overwrite;
            return true;
        } else if (response == QSL("s")) {
            clobberMode = ClobberMode::Skip;
            return false;
        } else if (response == QSL("q")) {
            QCoreApplication::exit(EXIT_FAILURE);
        } else {
            qCInfo(lc).noquote() << tr("y - overwrite this file");
            qCInfo(lc).noquote() << tr("n - do not overwrite this file");
            qCInfo(lc).noquote() << tr("a - overwrite this, and all remaining files");
            qCInfo(lc).noquote() << tr("s - do not overwrite this, or any remaining files");
            qCInfo(lc).noquote() << tr("q - quit now, without writing any further files");
            qCInfo(lc).noquote() << tr("? - print help");
        }
    }
    Q_UNREACHABLE();
}

Generator::Capitalisation Generator::getCase(const QString &first, const QString &second)
{
    if (first.isLower() && second.isLower()) {
        return Capitalisation::lowercase;
    }
    if (first.isUpper() && second.isUpper()) {
        return Capitalisation::UPPERCASE;
    }
    if (first.front().isLower() && second.front().isUpper()) {
        return Capitalisation::camelCase;
    }
    if (first.front().isUpper() && second.front().isUpper()) {
        return Capitalisation::CamelCase;
    }
    return Capitalisation::NoChange;
}

QString Generator::makeCase(const QString &string, const Capitalisation &capitalisation)
{
    switch (capitalisation) {
    case Capitalisation::NoChange:
        return string;
    case Capitalisation::lowercase:
        return string.toLower();
    case Capitalisation::camelCase:
        Q_UNIMPLEMENTED();
        break;
    case Capitalisation::CamelCase:
        Q_UNIMPLEMENTED();
        break;
    case Capitalisation::UPPERCASE:
        return string.toUpper();
    }
    qCCritical(lc).noquote() << tr("Unknown capitalisation %1 for %2")
        .arg((int)capitalisation).arg(string);
    return QString{};
}

QString Generator::makeOutputPathName(const QString &templateName, const QRegularExpression &pattern,
                                      const QHash<QString,QString> &ids, const QString &outputDir)
{
    QString outputPathName = templateName;
    for (auto iter = pattern.globalMatch(templateName); iter.hasNext(); ) {
        const QRegularExpressionMatch match = iter.next();
        const QString type = match.captured(QSL("type"));
        const QString sep = match.captured(QSL("separator"));
        const QString id = match.captured(QSL("id"));
        const QString newId = ids.value(id.toLower());
        if (newId.isNull()) {
            qCCritical(lc).noquote() << tr("No pathname ID value for %1").arg(id);
            return QString{};
        }
        const QString label = makeCase(newId, getCase(type, id)).replace(QLatin1Char(' '), sep);
        outputPathName.replace(type+sep+id, label);
    }
    if (!outputDir.isEmpty()) {
        outputPathName.prepend(outputDir + QLatin1Char('/'));
    }
    return outputPathName;
}

/// \todo This is often dropping the last word of a sentence... won't fix yet so we can maintain
/// the old aws-sdk-qt codegen behaviour until the swithover to smithy-qt templating is complete
/// (ie to minimise the initial change-over diff).
QStringList Generator::formatHtmlDocumentation(const QString &html)
{
    QString content(html);

    /// @todo There's much more we can do here, indeed the documentation
    /// conversation still needs a lot of love. But its a start, and no point
    /// prioritising it yet, since its more important we get the code structure
    /// right first.

    content.replace(QSL("<function>"), QSL("<code>"));
    content.replace(QSL("</function>"), QSL("</code>"));

    content.replace(QSL("<important>"), QSL("<b>"));
    content.replace(QSL("</important>"), QSL("</b>"));

    // Replace /* and */ with &ast; versions to avoid breaking comment blocks.
    content.replace(QSL("/*"), QSL("/&ast;"));
    content.replace(QSL("*/"), QSL("&ast;/"));

    QStringList lines;
    QString line;
    #if (QT_VERSION > QT_VERSION_CHECK(5, 14, 0))
        #define SKIP_EMPTY_PARTS Qt::SkipEmptyParts // Introduced in Qt 5.14.
    #else
        #define SKIP_EMPTY_PARTS QString::SkipEmptyParts // Deprecated in Qt 5.15.
    #endif
    foreach (QString word, content.split(QRegularExpression(QSL("\\s+")), SKIP_EMPTY_PARTS)) {
        if (word.startsWith(QSL("<p>")) || word.endsWith(QSL("</p>"))) {
            lines.append(line);
            line.clear();
            if (!lines.last().isEmpty()) {
                lines.append(QString()); // A blank line.
            }
            if (word.startsWith(QSL("<p>"))) {
                word.remove(0,3);
            }
            if (word.endsWith(QSL("</p>"))) {
                word.remove(word.size()-5,4);
            }
        }

        if (line.isEmpty()) {
            line += word;
        } else if (line.size() + word.size() < 120) {
            line += QLatin1Char(' ') + word;
        } else {
            lines.append(line);
            line = word;
        }
    }

    // Remove leading and trailing blank lines.
    while ((!lines.isEmpty()) && (lines.first().isEmpty())) {
        lines.removeFirst();
    }
    while ((!lines.isEmpty()) && (lines.last().isEmpty())) {
        lines.removeLast();
    }
    return lines;
}
