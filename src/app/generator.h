// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QDir>
#include <QJsonObject>

#include <grantlee/engine.h>

#include <QCoreApplication>
#include <QLoggingCategory>

namespace smithy {
  class Model;
  class Shape;
}
class Renderer;

class Generator
{
Q_DECLARE_TR_FUNCTIONS(Generator);

public:
    enum class ClobberMode {
        Prompt,
        Overwrite,
        Skip,
    };

    Generator(const smithy::Model * const model, Renderer * const renderer);

    int expectedFileCount() const;

    bool generate(const QString &outputDir, ClobberMode clobberMode);

    QStringList renderedFiles() const;
    QStringList skippedFiles() const;

protected:
    enum class Capitalisation {
        NoChange  = 0,
        lowercase = 1,
        camelCase = 2,
        CamelCase = 3,
        UPPERCASE = 4,
    };

    bool renderService(const smithy::Shape &service, const QStringList &templateNames,
                       const QString &outputDir, ClobberMode &clobberMode);

    bool renderOperation(const smithy::Shape &service, const smithy::Shape &operation,
                         const QStringList &templateNames, const QString &outputDir,
                         ClobberMode &clobberMode);

    bool render(const QString &templateName, const QString &outputPathName,
                ClobberMode &clobberMode);

    static bool promptToOverwrite(const QString &pathName, ClobberMode &clobberMode);

    static Capitalisation getCase(const QString &first, const QString &second);
    static QString makeCase(const QString &string, const Capitalisation &capitalisation);
    static QString makeOutputPathName(const QString &templateName, const QRegularExpression &pattern,
                                      const QHash<QString,QString> &ids,
                                      const QString &outputDir = QString{});

private:
    const smithy::Model * const model;
    Renderer * const renderer;
    QStringList renderedPathNames;
    QStringList skippedPathNames;

    static const QRegularExpression servicePattern;
    static const QRegularExpression operationPattern;
    static Q_LOGGING_CATEGORY(lc, "smithy.Generator", QtInfoMsg); ///< Logging category for Generator.
};
