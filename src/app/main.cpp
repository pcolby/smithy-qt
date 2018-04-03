/*
    Copyright 2013-2018 Paul Colby

    This file is part of the AWS SDK for Qt Code Generator.

    AWS SDK for Qt Code Generator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    AWS SDK for Qt Code Generator is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the AWS SDK for Qt Code Generator.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

#include "generator.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Set default paths reletive to the codegen project's source directory.
    const QDir srcDir = QFileInfo(QFile::decodeName(__FILE__)).dir();
    const QString defaultTemplatesPath = QDir::cleanPath(srcDir.absoluteFilePath(
        QStringLiteral("../aws-sdk-cpp/code-generation/api-descriptions")));
    const QString defaultOutputPath = QDir::cleanPath(srcDir.absoluteFilePath(
        QStringLiteral("../../src")));

    // Parse the command line options.
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Generate code for the AWS SDK for Qt project."));
    parser.addHelpOption();
    parser.addOptions({
        {{QStringLiteral("a"), QStringLiteral("apis")},
          QStringLiteral("Read API descriptions from dir (default is %1)").arg(defaultTemplatesPath),
          QStringLiteral("dir"), defaultTemplatesPath},
        {{QStringLiteral("f"), QStringLiteral("force")},
         QStringLiteral("Dont prompt before generating files")},
        {{QStringLiteral("o"), QStringLiteral("output")},
         QStringLiteral("Write output to dir (default is %1)").arg(defaultOutputPath),
         QStringLiteral("dir"), defaultOutputPath},
    });
    parser.process(app);

    // Verify that the output directory exists.
    const QFileInfo outputDir(QDir::cleanPath(parser.value(QStringLiteral("output"))));
    if ((!outputDir.exists()) || (!outputDir.isDir()) || (!outputDir.isWritable())) {
        qWarning() << "output directory does not exist, is not a directory, or is not writeable"
                   << outputDir.absoluteFilePath();
        return 2;
    }

    // Let the user know we're about to generate a lot of files.
    if (!parser.isSet(QStringLiteral("force"))) {
        qInfo() << "About to generate a lots of files in" << outputDir.absoluteFilePath();
        qInfo() << "Press Enter to contine";
        QTextStream stream(stdin);
        stream.readLine();
    }

    // Generate code.
    /// @todo Allow position arguments to specify individual APIs.
    Generator generator(outputDir.absoluteFilePath());
    QFileInfoList descriptions = QDir(
        parser.value(QStringLiteral("apis")), QLatin1String("*-???\?-?\?-??.normal.json"),
        QDir::Name|QDir::IgnoreCase, QDir::Files|QDir::Readable).entryInfoList();
    for (int i = 1; i < descriptions.size(); ++i) {
        if (descriptions.at(i).fileName().chopped(23) == descriptions.at(i-1).fileName().chopped(23)) {
            qInfo() << "skipping " << descriptions.at(i-1).fileName()
                    << "superseded by" << descriptions.at(i).fileName();
            descriptions.removeAt(--i);
        }
    }
    if (descriptions.isEmpty()) {
        qWarning() << "no API descriptiosn found in" << outputDir.absoluteFilePath();
        return 3;
    }
    if (!generator.generate(descriptions))
    {
        return 3;
    }
    return 0;
}
