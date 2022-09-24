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

const QRegularExpression Generator::servicePattern{
    QSL("(?<type>Service)(?<seperator>[^a-zA-Z0-9]*)(?<id>Title|sdkId)"),
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
    /// \todo build a list of services to add to context.
    QVariantMap context;

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
    const QHash<smithy::ShapeId, smithy::Shape> services = model->shapes(smithy::Shape::Type::Service);
    for (const smithy::Shape &service: services) {
        if (!renderService(service, serviceTemplateNames, outputDir, context, clobberMode)) {
            return false;
        }
        const smithy::Shape::ShapeReferences operations = service.operations();
        for (const smithy::Shape::ShapeReference &shapeRef: operations) {
            const smithy::Shape operation = model->shape(shapeRef.target);
            if (!operation.isValid()) {
                qCCritical(lc).noquote() << tr("Failed to find shape for %1 operation in %2 service")
                    .arg(shapeRef.target.toString(), service.id().toString());
                return false;
            }
            if (!renderOperation(service, operation, operationTemplateNames, outputDir, context,
                                 clobberMode)) {
                return false;
            }
        }
    }

    // Render all of the one-off (not per-service) files.
    qCDebug(lc).noquote() << tr("Rendering %n unary template/s", nullptr, plainTemplateNames.size());
    return std::all_of(plainTemplateNames.constBegin(), plainTemplateNames.constEnd(),
        [&](const QString &templateName){
            const QString outputPathName = outputDir + QLatin1Char('/') + templateName;
            return render(templateName, outputPathName, context, clobberMode);
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

bool Generator::renderService(const smithy::Shape &service,  const QStringList &templateNames,
                              const QString &outputDir, const QVariantMap &context,
                              ClobberMode &clobberMode)
{
    qCDebug(lc).noquote() << tr("Rendering templates for service %1").arg(service.id().toString());

    /// \todo extend context for this service.
    const QString apiTitle = service.traits().value(QSL("smithy.api#title")).toString();
    const QString sdkId = service.traits().value(QSL("aws.api#service")).toObject().value(QSL("sdkId")).toString();
    if (apiTitle.isEmpty()) {
        qCCritical(lc).noquote() << tr("Service %1 has no API title").arg(service.id().toString());
        return false;
    }
    if (sdkId.isEmpty()) {
        qCCritical(lc).noquote() << tr("Service %1 has no SDK ID").arg(service.id().toString());
        return false;
    }

    // Render each of service templates.
    for (const QString &templateName: templateNames) {
        const QString outputPathName = makeOutputPathName(templateName, servicePattern, {
            { QSL("name"), apiTitle },
            { QSL("sdkid"), sdkId },
        }, outputDir);
        if (!render(templateName, outputPathName, context, clobberMode)) {
            return false;
        }
    }
    return true;
}

bool Generator::renderOperation(const smithy::Shape &service, const smithy::Shape &operation,
                                const QStringList &templateNames, const QString &outputDir,
                                const QVariantMap &context, ClobberMode &clobberMode)
{
    qCDebug(lc).noquote() << tr("Rendering templates for operation %1").arg(operation.id().toString());

    /// \todo extend context for this operation.
    const QString apiTitle = service.traits().value(QSL("smithy.api#title")).toString();
    const QString sdkId = service.traits().value(QSL("aws.api#service")).toObject().value(QSL("sdkId")).toString();
    if (apiTitle.isEmpty()) {
        qCCritical(lc).noquote() << tr("Service %1 has no API title").arg(service.id().toString());
        return false;
    }
    if (sdkId.isEmpty()) {
        qCCritical(lc).noquote() << tr("Service %1 has no SDK ID").arg(service.id().toString());
        return false;
    }

    for (const QString &templateName: templateNames) {
        QString outputPathName = makeOutputPathName(templateName, servicePattern, {
            { QSL("name"), apiTitle },
            { QSL("sdkid"), sdkId },
        });
        outputPathName = makeOutputPathName(outputPathName, operationPattern, {
            { QSL("name"), operation.id().shapeName() },
        }, outputDir);
        if (!render(templateName, outputPathName, context, clobberMode)) {
            return false;
        }
    }
    return true;
}

bool Generator::render(const QString &templateName, const QString &outputPathName,
                       const QVariantMap &context, ClobberMode &clobberMode)
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
    if (!renderer->render(templateName, outputPathName, context)) {
        return false;
    }
    renderedPathNames.append(outputPathName);
    return true;
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

//QStringList Generator::formatHtmlDocumentation(const QString &html)
//{
//    QString content(html);

//    /// @todo There's much more we can do here, indeed the documentation
//    /// conversation still needs a lot of love. But its a start, and no point
//    /// prioritising it yet, since its more important we get the code structure
//    /// right first.

//    content.replace(QSL("<function>"), QSL("<code>"));
//    content.replace(QSL("</function>"), QSL("</code>"));

//    content.replace(QSL("<important>"), QSL("<b>"));
//    content.replace(QSL("</important>"), QSL("</b>"));

//    // Replace /* and */ with &ast; versions to avoid breaking comment blocks.
//    content.replace(QSL("/*"), QSL("/&ast;"));
//    content.replace(QSL("*/"), QSL("&ast;/"));

//    QStringList lines;
//    QString line;
//    #if (QT_VERSION > QT_VERSION_CHECK(5, 14, 0))
//        #define SKIP_EMPTY_PARTS Qt::SkipEmptyParts // Introduced in Qt 5.14.
//    #else
//        #define SKIP_EMPTY_PARTS QString::SkipEmptyParts // Deprecated in Qt 5.15.
//    #endif
//    foreach (QString word, content.split(QRegularExpression(QSL("\\s+")), SKIP_EMPTY_PARTS)) {
//        if (word.startsWith(QSL("<p>")) || word.endsWith(QSL("</p>"))) {
//            lines.append(line);
//            line.clear();
//            if (!lines.last().isEmpty()) {
//                lines.append(QSL("")); // A blank line.
//            }
//            if (word.startsWith(QSL("<p>"))) {
//                word.remove(0,3);
//            }
//            if (word.endsWith(QSL("</p>"))) {
//                word.remove(word.size()-5,4);
//            }
//        }

//        if (line.isEmpty()) {
//            line += word;
//        } else if (line.size() + word.size() < 120) {
//            line += QLatin1Char(' ') + word;
//        } else {
//            lines.append(line);
//            line = word;
//        }
//    }

//    // Remove leading and trailing blank lines.
//    while ((!lines.isEmpty()) && (lines.first().isEmpty())) {
//        lines.removeFirst();
//    }
//    while ((!lines.isEmpty()) && (lines.last().isEmpty())) {
//        lines.removeLast();
//    }
//    return lines;
//}
