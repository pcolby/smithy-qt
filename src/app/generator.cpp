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
    QSL("(?<type>Service)(?<seperator>[-_.]*)(?<id>Name|sdkId)"),
    QRegularExpression::CaseInsensitiveOption
};

const QRegularExpression Generator::operationPattern{
    QSL("(?<type>Operation)(?<seperator>[-_.]*)(?<id>Name)"),
    QRegularExpression::CaseInsensitiveOption
};

Generator::Generator(const smithy::Model * const model, const Renderer * const renderer)
    : renderedFileCount(0), model(model), renderer(renderer)
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

int Generator::generate(const QString &outputDir, ClobberMode clobberMode)
{
    renderedFileCount = 0;

    /// \todo build a list of services to add to context.
    QVariantMap context;

    // Build the list of template names.
    const QStringList templatesNames = renderer->templatesNames();
    QStringList serviceTemplateNames, operationTemplateNames, plainTemplateNames;
    const QHash<smithy::ShapeId, smithy::Shape> services = model->shapes(smithy::Shape::Type::Service);
    for (const QString &name: templatesNames) {
        if (servicePattern.match(name).hasMatch()) {
            if (operationPattern.match(name).hasMatch()) {
                operationTemplateNames.append(name);
            } else {
                serviceTemplateNames.append(name);
            }
        } else {
            plainTemplateNames.append(name);
        }
    }
    Q_ASSERT(serviceTemplateNames.size() + operationTemplateNames.size()
             + plainTemplateNames.size() == templatesNames.size());

    // Render all of the services.
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
                return -1;
            }
            renderOperation(service, operation, operationTemplateNames, outputDir, context, clobberMode);
        }
    }

    // Render all of the one-off (not per-service) files.
    for (const QString &templateName: plainTemplateNames) {
        render(templateName, outputDir, context, clobberMode);
    }
    Q_UNUSED(outputDir);
    Q_UNUSED(clobberMode);
    Q_UNIMPLEMENTED();
    return renderedFileCount;
}

bool Generator::renderService(const smithy::Shape &service,  const QStringList &templateNames,
                              const QString &outputDir, const QVariantMap &context,
                              ClobberMode &clobberMode)
{
    qCDebug(lc).noquote() << tr("Rendering templates for service %1").arg(service.id().toString());

    /// \todo extend context for this operation.
    const QString sdkId = service.traits().value(QSL("aws.api#service")).toObject().value(QSL("sdkId")).toString();
    if (sdkId.isEmpty()) {
        qCCritical(lc).noquote() << tr("Service %1 has no SDK ID").arg(service.id().toString());
        return false;
    }
    qCDebug(lc).noquote() << tr("SDK ID: %1").arg(sdkId);


    /// \todo render each of serviceTemplateNames, with sanitised service id.
    for (const QString &templateName: templateNames) {
        /// \todo Build the output pathname.
        QString outputPathName = outputDir + QLatin1Char('/') + templateName;
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
    Q_UNUSED(service);

    for (const QString &templateName: templateNames) {
        /// \todo Build the output pathname.
        QString outputPathName = outputDir + QLatin1Char('/') + templateName;
        if (!render(templateName, outputPathName, context, clobberMode)) {
            return false;
        }
    }
    return true;
}

bool Generator::render(const QString &templateName, const QString &outputPathName,
                       const QVariantMap &context, ClobberMode &clobberMode)
{
    // Apply the clobber.
    Q_UNUSED(clobberMode);

    renderedFileCount++;

    // Render the content.
    return renderer->render(templateName, outputPathName, context);
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
