// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QDir>
#include <QJsonObject>

#include <grantlee/engine.h>

#include <QCoreApplication>
#include <QLoggingCategory>

class Generator
{
Q_DECLARE_TR_FUNCTIONS(Generator);

public:
    explicit Generator(const QDir &outputDir);

    int generate(const QFileInfoList &descriptions);
    int generate(const QString &serviceFileName, const QJsonObject &description);

protected:
    static QStringList formatHtmlDocumentation(const QString &html);

    bool generateModelClasses(Grantlee::Context &context, const QString &projectDir,
                              const QString &operationName, const QJsonObject &description);

    static QString getServiceName(const QJsonObject &metaData);

    bool render(const QString &templateName, Grantlee::Context &context,
                const QString &outputPathName) const;

    bool render(const QString &templateName, Grantlee::Context &context,
                const QString &outputDirName, const QString &outputFileName) const;

    void renderClassFiles(const QString &templateBaseName, Grantlee::Context &context,
                          const QString &outputPathName, const QString className);

private:
    QDir outputDir;
    Grantlee::Engine engine;
    QMap<QString, Grantlee::Template> templates;
    QStringList headers, modules, sources;

    static Q_LOGGING_CATEGORY(lc, "smithy.Generator", QtInfoMsg); ///< Logging category for Generator.
};
