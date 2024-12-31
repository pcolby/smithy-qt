// SPDX-FileCopyrightText: 2013-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "generator.h"
#include "renderer.h"

#include <qtsmithy/model.h>

#if defined USE_CUTELEE
#include <cutelee/cutelee_version.h>
#elif defined USE_GRANTLEE
#include <grantlee/grantlee_version.h>
#elif defined USE_KTEXTTEMPLATE
#include <ktexttemplate_version.h>
#endif

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QJsonDocument>
#include <QLoggingCategory>

#if defined(Q_OS_UNIX)
#include <unistd.h>
#elif defined(Q_OS_WIN)
#include <Windows.h>
#endif

Q_LOGGING_CATEGORY(lc, "smithy.app", QtInfoMsg); ///< Logging category for main application.

inline bool haveConsole()
{
    #if defined(Q_OS_UNIX)
    return isatty(STDERR_FILENO);
    #elif defined(Q_OS_WIN)
    return GetConsoleWindow();
    #else
    return false;
    #endif
}

void configureLogging(const QCommandLineParser &parser)
{
    // Start with the Qt default message pattern (see qtbase:::qlogging.cpp:defaultPattern)
    QString messagePattern = QStringLiteral("%{if-category}%{category}: %{endif}%{message}");

    if (parser.isSet(QStringLiteral("debug"))) {
        #ifdef QT_MESSAGELOGCONTEXT
        // %{file}, %{line} and %{function} are only available when QT_MESSAGELOGCONTEXT is set.
        messagePattern.prepend(QStringLiteral("%{function} "));
        #endif
        messagePattern.prepend(QStringLiteral("%{time process} %{type} "));
        QLoggingCategory::setFilterRules(QStringLiteral("smithy.*.debug=true"));
    }

    const QString color = parser.value(QStringLiteral("color"));
    if ((color == QStringLiteral("yes")) || (color == QStringLiteral("auto") && haveConsole())) {
        messagePattern.prepend(QStringLiteral(
        "%{if-debug}\x1b[37m%{endif}"      // White
        "%{if-info}\x1b[32m%{endif}"       // Green
        "%{if-warning}\x1b[35m%{endif}"    // Magenta
        "%{if-critical}\x1b[31m%{endif}"   // Red
        "%{if-fatal}\x1b[31;1m%{endif}")); // Red and bold
        messagePattern.append(QStringLiteral("\x1b[0m")); // Reset.
    }

    qSetMessagePattern(messagePattern);
}

void parseCommandLineOptions(QCoreApplication &app, QCommandLineParser &parser)
{
    parser.addOptions({
        {{QStringLiteral("m"), QStringLiteral("models")},
          QCoreApplication::translate("main", "Read Smithy models from dir"),
          QStringLiteral("dir")},
        {{QStringLiteral("t"), QStringLiteral("templates")},
          QCoreApplication::translate("main", "Read text templates from dir"),
          QStringLiteral("dir")},
        {{QStringLiteral("o"), QStringLiteral("output")},
          QCoreApplication::translate("main", "Write output files to dir"), QStringLiteral("dir")},
        {{QStringLiteral("f"), QStringLiteral("force")},
          QCoreApplication::translate("main", "Overwrite existing files")},
        { {QStringLiteral("c"), QStringLiteral("color")},
          QCoreApplication::translate("main", "Color the console output (default auto)"),
          QStringLiteral("yes|no|auto"), QStringLiteral("auto")},
        {{QStringLiteral("d"), QStringLiteral("debug")},
          QCoreApplication::translate("main", "Enable debug output")},
    });
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);
}

bool requireOptions(const QStringList &requiredOtions, const QCommandLineParser &parser)
{
    QStringList missingOptions;
    for (const QString &requiredOption: requiredOtions) {
        if (!parser.isSet(requiredOption)) {
            missingOptions.append(requiredOption);
        }
    }
    if (!missingOptions.empty()) {
        qCCritical(lc).noquote() << QCoreApplication::translate("requireOptions",
            "Missing required option(s): %1").arg(missingOptions.join(QLatin1Char(' ')));
        return false;
    }
    return true;
}

inline bool checkRequiredOptions(const QCommandLineParser &parser)
{
    const QStringList requiredOptions{
        QStringLiteral("models"),
        QStringLiteral("templates"),
        QStringLiteral("output"),
    };
    return requireOptions(requiredOptions, parser);
}

bool requireDirs(const QCommandLineParser &parser, const QString &option, const QDir::Filters &rw)
{
    bool success = true;
    const QStringList dirs = parser.values(option);
    for (const QString &dir: dirs) {
        const QFileInfo info(dir);
        const QString label = option.at(0).toUpper() + option.mid(1);
        if (!info.exists()) {
            qCCritical(lc).noquote() << QCoreApplication::translate("requireDirs",
                "%1 directory does not exist: %2").arg(label, dir);
            success = false;
        } else if (!info.isDir()) {
            qCCritical(lc).noquote() << QCoreApplication::translate("requireDirs",
                "%1 directory is not a directory: %2").arg(label, dir);
            success = false;
        } else {
            if ((rw.testFlag(QDir::Readable)) && (!info.isReadable())) {
                qCCritical(lc).noquote() << QCoreApplication::translate("requireDirs",
                    "%1 directory is not readable: %2").arg(label, dir);
                success = false;
            }
            if ((rw.testFlag(QDir::Writable)) && (!info.isWritable())) {
                qCCritical(lc).noquote() << QCoreApplication::translate("requireDirs",
                    "%1 directory is not writable: %2").arg(label, dir);
                success = false;
            }
        }
    }
    return success;
}

inline bool checkRequiredDirs(const QCommandLineParser &parser)
{
    bool success = true;
    const QMap<QString,QDir::Filters> options{
        { QLatin1String("models"),    QDir::Readable },
        { QLatin1String("templates"), QDir::Readable },
        { QLatin1String("output"),    QDir::Writable },
    };
    for (auto iter = options.constBegin(); iter != options.constEnd(); ++iter) {
        if (!requireDirs(parser, iter.key(), iter.value())) {
            success = false;
        }
    }
    return success;
}

int loadModels(const QString &dir, smithy::Model &model)
{
    int count = 0;
    qCInfo(lc).noquote()
        << QCoreApplication::translate("loadModels", "Loading Smithy models from %1").arg(dir);
    QDirIterator iter{dir, {QStringLiteral("*.json")}, QDir::Files, QDirIterator::Subdirectories};
    while (iter.hasNext()) {
        QFile file{iter.next()};
        qCDebug(lc).noquote() << QCoreApplication::translate("loadModels", "Loading Smithy JSON: %1")
                                 .arg(file.fileName());
        if (!file.open(QFile::ReadOnly)) {
            qCCritical(lc).noquote() << QCoreApplication::translate("loadModels",
                "Failed to open JSON file: %1").arg(file.fileName());
            return -count;
        }
        QJsonParseError error{};
        QJsonDocument json = QJsonDocument::fromJson(file.readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qCCritical(lc).noquote() << QCoreApplication::translate("loadModels",
                "Failed to parse JSON file: %1").arg(file.fileName());
            return -count;
        }
        if (!json.isObject()) {
            qCCritical(lc).noquote() << QCoreApplication::translate("loadModels",
                "File is not a JSON object: %1").arg(file.fileName());
            return -count;
        }
        if (!model.insert(json.object())) {
            qCCritical(lc).noquote() << QCoreApplication::translate("loadModels",
                "Failed to parse Smithy JSON AST: %1").arg(file.fileName());
            return -count;
        }
        ++count;
    }
    qCDebug(lc).noquote() << QCoreApplication::translate("loadModels", "Loaded %n model(s) from %1",
                                                         nullptr, count).arg(dir);
    if (count == 0) {
        qCCritical(lc).noquote() << QCoreApplication::translate("loadModels",
            "Failed to find any JSON AST models in %1").arg(dir);
    }
    return count;
}

int loadModels(const QStringList &dirs, smithy::Model &model)
{
    int count = 0;
    for (const QString &dir: dirs) {
        const int thisCount = loadModels(dir, model);
        if (thisCount <= 0) {
            return thisCount - count;
        } else {
            count += thisCount;
        }
    }
    qCDebug(lc).noquote() << QCoreApplication::translate("loadModels", "Loaded %n model(s) in total",
                                                         nullptr, count);
    return count;
}

int main(int argc, char *argv[])
{
    // Setup the core application.
    QCoreApplication app(argc, argv);
    app.setApplicationName(QStringLiteral(PROJECT_NAME));
    #ifdef PROJECT_PRE_RELEASE
    app.setApplicationVersion(QStringLiteral(PROJECT_VERSION "-" PROJECT_PRE_RELEASE));
    #else
    app.setApplicationVersion(QStringLiteral(PROJECT_VERSION));
    #endif

    // Parse the command line options.
    QCommandLineParser parser;
    parseCommandLineOptions(app, parser);
    configureLogging(parser);
    qCDebug(lc).noquote() << QCoreApplication::applicationName() << QCoreApplication::applicationVersion();
    qCDebug(lc).noquote() << "Qt " QT_VERSION_STR " compile-time";
    qCDebug(lc).noquote() << "Qt" << qVersion() << "runtime";
    #if defined USE_CUTELEE
    qCDebug(lc).noquote() << "Cutelee " CUTELEE_VERSION_STRING " compile-time";
    #elif defined USE_GRANTLEE
    qCDebug(lc).noquote() << "Grantlee " GRANTLEE_VERSION_STRING " compile-time";
    #elif defined USE_KTEXTTEMPLATE
    qCDebug(lc).noquote() << "KTextTemplate " KTEXTTEMPLATE_VERSION_STRING " compile-time";
    #endif

    if (!checkRequiredOptions(parser)) return 1;
    if (!checkRequiredDirs(parser))    return 2;

    // Load the Smithy model files.
    smithy::Model model;
    const int modelsCount = loadModels(parser.values(QStringLiteral("models")), model);
    if (modelsCount <= 0) return 3; // loadModels() will have already logged the (criticial) error.
    if ((!model.finish()) || (!model.isValid())) {
        qCCritical(lc).noquote() << QCoreApplication::translate("main",
            "Failed to merge Smithy model files into a valid Smithy model");
        return 4;
    }

    // Setup the renderer.
    Renderer renderer;
    if (!renderer.loadTemplates(parser.value(QStringLiteral("templates")))) {
        return 5;
    }

    // Setup the generator.
    const QString outputDir = QDir(parser.value(QStringLiteral("output"))).absolutePath();
    Generator generator(&model, &renderer);
    if (!parser.isSet(QStringLiteral("force"))) {
        qCWarning(lc).noquote() << QCoreApplication::translate("main", "About to generate approximately"
            " %n file/s in %2", nullptr, generator.expectedFileCount()).arg(outputDir);
        qCInfo(lc).noquote() << QCoreApplication::translate("main", "Press Enter to contine");
        QTextStream stream(stdin);
        stream.readLine();
    }
    qCInfo(lc).noquote() << QCoreApplication::translate("main", "Rendering approximately"
        " %n file/s in %2", nullptr, generator.expectedFileCount()).arg(outputDir);
    if (!generator.generate(outputDir, parser.isSet(QStringLiteral("force"))
            ? Generator::ClobberMode::Overwrite : Generator::ClobberMode::Prompt)) {
        return 6;
    }
    qCInfo(lc).noquote() << QCoreApplication::translate("main",
        "Rendered %n file/s (and skipped %1) in %2", nullptr, generator.renderedFiles().count())
        .arg(generator.skippedFiles().size()).arg(outputDir);
    return 0;
}
