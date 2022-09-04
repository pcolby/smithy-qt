// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLoggingCategory>

#if defined(Q_OS_UNIX)
#include <unistd.h>
#elif defined(Q_OS_WIN)
#include <Windows.h>
#endif

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
        messagePattern.prepend(QStringLiteral("%{time process} %{type} %{function} "));
        QLoggingCategory::defaultCategory()->setEnabled(QtDebugMsg, true);
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

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QStringLiteral(PROJECT_NAME));
    app.setApplicationVersion(QStringLiteral(PROJECT_VERSION));

    // Parse the command line options.
    QCommandLineParser parser;
    parser.addOptions({
        {{QStringLiteral("m"), QStringLiteral("models")},
          QCoreApplication::translate("main", "Read Smithy models from dir"),
          QStringLiteral("dir")},
        {{QStringLiteral("t"), QStringLiteral("templates")},
          QCoreApplication::translate("main", "Read Grantlee templates from dir"),
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
    configureLogging(parser);

    // Check for any missing (but required) command line options.
    QStringList missingOptions {
        QStringLiteral("models"),
        QStringLiteral("templates"),
        QStringLiteral("output"),
    };
    for (auto iter = missingOptions.begin(); iter != missingOptions.end();) {
        if (parser.isSet(*iter)) iter=missingOptions.erase(iter); else ++iter;
    }
    if (!missingOptions.empty()) {
        qWarning().noquote() << QCoreApplication::translate("main", "Missing required option(s): %1")
            .arg(missingOptions.join(QLatin1Char(' ')));
        return 2;
    }

    // Verify that the directories exist.
    const QFileInfo modelsDir (QDir::cleanPath(parser.value(QStringLiteral("models"))));
    const QFileInfo templatesDir (QDir::cleanPath(parser.value(QStringLiteral("templates"))));
    const QFileInfo outputDir(QDir::cleanPath(parser.value(QStringLiteral("output-dir"))));

    if ((!modelsDir.exists()) || (!modelsDir.isDir()) || (!modelsDir.isReadable())) {
        qWarning().noquote() << QCoreApplication::translate("main",
            "Models directory does not exist, is not a directory, or is not readable: %1")
            .arg(modelsDir.absoluteFilePath());
        return 2;
    }
    if ((!templatesDir.exists()) || (!templatesDir.isDir()) || (!templatesDir.isReadable())) {
        qWarning().noquote() << QCoreApplication::translate("main",
            "Theme directory does not exist, is not a directory, or is not readable: %1")
            .arg(templatesDir.absoluteFilePath());
        return 2;
    }
    if ((!outputDir.exists()) || (!outputDir.isDir()) || (!outputDir.isWritable())) {
        qWarning().noquote() << QCoreApplication::translate("main",
            "Output directory does not exist, is not a directory, or is not writable: %1")
            .arg(outputDir.absoluteFilePath());
        return 2;
    }

    // Setup the renderer.
//    Renderer renderer(inputDir.absoluteFilePath());
//    if (!renderer.loadTemplates(templatesDir.absoluteFilePath())) {
//        return 3;
//    }

    // Let the user know we're about to generate a lot of files, then do it!
//    qWarning().noquote() << QCoreApplication::translate("main",
//        "About to generate approximately %1 file(s) in: %2")
//        .arg(renderer.expectedFileCount()).arg(outputDir.absoluteFilePath());
//    if (!parser.isSet(QStringLiteral("force"))) {
//        qInfo().noquote() << QCoreApplication::translate("main", "Press Enter to contine");
//        QTextStream stream(stdin);
//        stream.readLine();
//    }
//    if (!renderer.render(outputDir.absoluteFilePath(), clobberMode)) {
//        return 4;
//    }
//    qInfo().noquote() << QCoreApplication::translate("main",
//        "Rendered %1 file(s) in %2").arg(renderer.outputFileCount()).arg(outputDir.absoluteFilePath());
    return 0;
}
