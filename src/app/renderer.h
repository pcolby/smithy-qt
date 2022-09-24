// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later


#include <grantlee/engine.h>

#include <QCoreApplication>
#include <QLoggingCategory>

class Renderer
{
Q_DECLARE_TR_FUNCTIONS(Renderer);

public:
    Renderer();

    bool loadTemplates(const QString &dirPath);

    QStringList templatesNames() const;

    bool render(const QString &templateName, const QString &outputPathName,
                const QVariantMap &additionalContext = QVariantMap{});

protected:
    Grantlee::Context context;
    Grantlee::Engine engine;
    QStringList templates;

    static Q_LOGGING_CATEGORY(lc, "smithy.Renderer", QtInfoMsg); ///< Logging category for Renderer.
};
