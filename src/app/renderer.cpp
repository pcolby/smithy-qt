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
    QDirIterator iter{QDir::cleanPath(dir), QDir::Files, QDirIterator::Subdirectories};
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
class NoEscapeStream : public Grantlee::OutputStream {
public:
    explicit NoEscapeStream(QTextStream * stream) : Grantlee::OutputStream(stream) { }

    // cppcheck-suppress unusedFunction
    virtual QString escape(const QString &input) const { return input; }

    // cppcheck-suppress unusedFunction
    virtual QSharedPointer<OutputStream> clone( QTextStream *stream ) const {
        return QSharedPointer<OutputStream>(new NoEscapeStream(stream));
    }
};

void Renderer::push(const QVariantHash &context)
{
    this->context.push();
    for (auto iter = context.constBegin(); iter != context.constEnd(); ++iter) {
        this->context.insert(sanitise(iter.key()), sanitise(iter.value()));
    }
}

void Renderer::pop()
{
    context.pop();
}

bool Renderer::render(const QString &templateName, const QString &outputPathName,
            const QVariantHash &additionalContext)
{
    qCDebug(lc).noquote() << tr("Rendering %1 to %2").arg(templateName, outputPathName);
    if (!templates.contains(templateName)) {
        qCCritical(lc).noquote() << tr("Template %1 has not been loaded").arg(templateName);
        return false;
    }

    const QDir dir = QFileInfo(outputPathName).dir();
    if (!dir.mkpath(QSL("./"))) {
        qCCritical(lc).noquote() << tr("Failed to create directory path %1").arg(dir.path());
        return false;
    }

    QFile file(outputPathName);
    if (!file.open(QFile::WriteOnly)) {
        qCCritical(lc).noquote() << tr("Failed to open %1 for writing: %2")
            .arg(outputPathName, file.errorString());
        return false;
    }

    push(additionalContext);
    QTextStream textStream(&file);
    NoEscapeStream noEscapeStream(&textStream);
    Grantlee::Template tmplate = engine.loadByName(templateName);
    if (!tmplate) {
        qCCritical(lc).noquote() << tr("Failed to fetch template %1").arg(outputPathName);
        pop();
        return false;
    }
    tmplate->render(&noEscapeStream, &context);
    if (tmplate->error()) {
        qCCritical(lc).noquote() << tr("Failed to render %1: %2").arg(outputPathName, tmplate->errorString());
        pop();
        return false;
    }
    pop();
    return true;
}

QString Renderer::sanitise(const QString &key)
{
    QString newKey = key;
    int pos;
    while ((pos = newKey.indexOf(QLatin1Char('.'))) >= 0) {
        newKey = newKey.mid(0, pos) + newKey.mid(pos+1,1).toUpper() + newKey.mid(pos+2);
    }
    newKey.replace(QRegularExpression{QSL("[^a-zA-Z0-9_]")}, QSL("_"));
    newKey.replace(QRegularExpression{QSL("^[0-9_]")}, QLatin1String());
    return newKey;
}

QVariant Renderer::sanitise(const QVariant &variant)
{
    if (variant.type() == QVariant::Hash) {
        return sanitise(variant.toHash());
    } else if (variant.type() == QVariant::Map) {
        return sanitise(variant.toMap());
    }
    return variant;
}

QVariantHash Renderer::sanitise(const QVariantHash &hash)
{
    QVariantHash newHash;
    for (auto iter = hash.begin(); iter != hash.end(); ++iter) {
        const QString saneKey = sanitise(iter.key());
        QVariant saneValue = iter.value();
        if (iter->type() == QVariant::Hash) {
            saneValue = sanitise(iter->toHash());
        } else if (iter->type() == QVariant::Map) {
            saneValue = sanitise(iter->toMap());
        }
        newHash.insert(saneKey, saneValue);
    }
    Q_ASSERT(hash.size() == newHash.size());
    return newHash;
}

QVariantMap Renderer::sanitise(const QVariantMap &map)
{
    QVariantMap newMap;
    for (auto iter = map.begin(); iter != map.end(); ++iter) {
        const QString saneKey = sanitise(iter.key());
        QVariant saneValue = iter.value();
        if (iter->type() == QVariant::Hash) {
            saneValue = sanitise(iter->toHash());
        } else if (iter->type() == QVariant::Map) {
            saneValue = sanitise(iter->toMap());
        }
        newMap.insert(saneKey, saneValue);
    }
    Q_ASSERT(map.size() == newMap.size());
    return newMap;
}
