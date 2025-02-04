// SPDX-FileCopyrightText: 2013-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtGlobal> // \todo Remove if/when dropping support for Qt 6.0.x.
#if (QT_VERSION_CHECK(6, 0, 0) <= QT_VERSION) && (QT_VERSION < QT_VERSION_CHECK(6, 1, 0))
#include <limits> // https://bugreports.qt.io/browse/QTBUG-89977
#endif

#if defined USE_CUTELEE
#include <cutelee/engine.h>
#elif defined USE_GRANTLEE
#include <grantlee/engine.h>
#elif defined USE_KTEXTTEMPLATE
#include <KTextTemplate/Engine>
#endif

#include "textlee.h"

#include <QCoreApplication>
#include <QLoggingCategory>

class Renderer
{
Q_DECLARE_TR_FUNCTIONS(Renderer);

public:
    Renderer();

    bool loadTemplates(const QString &dirPath);

    QStringList templatesNames() const;

    void push(const QVariantHash &context);
    void pop();

    bool render(const QString &templateName, const QString &outputPathName,
                const QVariantHash &additionalContext = QVariantHash{});

    static QString sanitise(const QString &key);
    static QVariant sanitise(const QVariant &variant);
    static QVariantHash sanitise(const QVariantHash &hash);
    static QVariantMap sanitise(const QVariantMap &map);

protected:
    Textlee::Context context;
    Textlee::Engine engine;
    QStringList templates;

    static Q_LOGGING_CATEGORY(lc, "smithy.Renderer", QtInfoMsg); ///< Logging category for Renderer.
};
