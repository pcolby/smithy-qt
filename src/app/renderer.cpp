// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "renderer.h"

#include <QDebug>
#include <QDirIterator>
#include <QJsonParseError>
#include <QRegularExpression>

#include <grantlee/cachingloaderdecorator.h>
#include <grantlee/templateloader.h>

#define QSL(str) QStringLiteral(str) // Shorten the QStringLiteral macro for readability.

Renderer::Renderer()
{
    engine.setSmartTrimEnabled(true);
}

bool Renderer::loadTemplates(const QString &dir)
{
    qCInfo(lc).noquote() << tr("Loading Grantlee templates from %1").arg(dir);

    // Setup the template loader.
    auto loader = QSharedPointer<Grantlee::FileSystemTemplateLoader>::create();
    // Note, {% include "<filename>" %} will look for files relative to templateDirs.
    loader->setTemplateDirs(QStringList{dir});
    auto cachedLoader = QSharedPointer<Grantlee::CachingLoaderDecorator>::create(loader);
    engine.addTemplateLoader(cachedLoader);

    // Load the templates.
    QDirIterator iter{dir, QDir::Files, QDirIterator::Subdirectories};
    while (iter.hasNext()) {
        const QString name = iter.next().mid(iter.path().size()+1);
        qCDebug(lc).noquote() << tr("Loading template: %1").arg(name);
        const Grantlee::Template tmplate = engine.loadByName(name);
        if (tmplate->error()) {
            qCritical().noquote() << tr("Error loading template %1: %2")
                .arg(name, tmplate->errorString());
            return false;
        }
        templates.append(name);
    }
    qDebug(lc).noquote() << tr("Loaded %n template/s from %1", nullptr, templates.size()).arg(dir);
    return true;
}

QStringList Renderer::templatesNames() const
{
    return templates;
}


// Grantlee output stream that does *no* content escaping.
//class NoEscapeStream : public Grantlee::OutputStream {
//public:
//    explicit NoEscapeStream(QTextStream * stream) : Grantlee::OutputStream(stream) { }

//    virtual QString escape(const QString &input) const { return input; }

//    virtual QSharedPointer<OutputStream> clone( QTextStream *stream ) const {
//        return QSharedPointer<OutputStream>(new NoEscapeStream(stream));
//    }
//};

//bool Renderer::render(const QString &templateName, const QString &outputPathName,
//            const QVariantMap &additionalContext = QVariantMap{}) const
//{
//    if (!templates.contains(templateName)) {
//        qWarning() << "template does not exist" << templateName;
//        return false;
//    }

//    QFile file(outputFileName);
//    if (!file.open(QFile::WriteOnly)) {
//        qWarning() << "failed to open file for writing" << outputFileName;
//        return false;
//    }

//    QTextStream textStream(&file);
//    NoEscapeStream noEscapeStream(&textStream);
//    templates[templateName]->render(&noEscapeStream, &context);
//    if (templates[templateName]->error()) {
//        qInfo() << "failed to generate" << outputFileName << templates[templateName]->errorString();
//        return false;
//    }
//    return true;
//}
