// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QDir>
#include <QJsonObject>

#include <grantlee/engine.h>

#include <QCoreApplication>
#include <QLoggingCategory>

namespace smithy {
  class Model;
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

    Generator(const smithy::Model * const model, const Renderer * const renderer);

    int expectedFileCount() const;

    int generate(const QString &outputDir, ClobberMode clobberMode);

protected:
    bool render(const QString &templateName, const QString &outputDir,
                const QVariantMap &context, ClobberMode &clobberMode) const;

private:
    const smithy::Model * const model;
    const Renderer * const renderer;

    static const QRegularExpression servicePattern;
    static const QRegularExpression operationPattern;
    static Q_LOGGING_CATEGORY(lc, "smithy.Generator", QtInfoMsg); ///< Logging category for Generator.
};
