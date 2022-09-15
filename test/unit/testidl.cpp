// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testidl.h"

#include "qtsmithy/idl.h"

#include <QDirIterator>
#include <QJsonDocument>

#define PATH_TO_AWS_SMITHY_SRC "../testdata/aws-labs-smithy"
#define PATH_TO_AWS_SMITHY_MODEL_TESTS \
    PATH_TO_AWS_SMITHY_SRC "/smithy-model/src/test/resources/software/amazon/smithy/model"

void TestIdl::idlToAst_data()
{
    QTest::addColumn<QString>("idlFileName");
    QTest::addColumn<QString>("astFileName");

    const QString dataBasePath = QStringLiteral(PATH_TO_AWS_SMITHY_MODEL_TESTS "/loader/valid");
    const QString dataPathName = QFINDTESTDATA(dataBasePath);
    QVERIFY2(!dataPathName.isEmpty(),
             qUtf8Printable(QStringLiteral("failed to locate test data: %1").arg(dataBasePath)));

    QDirIterator iter{QDir::cleanPath(dataPathName), {QStringLiteral("*.json")},
                      QDir::AllDirs|QDir::Files|QDir::NoDotAndDotDot|QDir::Readable,
                      QDirIterator::Subdirectories};
    while (iter.hasNext()) {
        const QString jsonFilePath = iter.next();
        if (iter.fileInfo().isDir()) continue;
        Q_ASSERT(jsonFilePath.endsWith(QStringLiteral(".json")));
        const QString smithyFilePath = jsonFilePath.left(jsonFilePath.size() - 5) +
                                       QStringLiteral(".smithy");
        if (QFile::exists(smithyFilePath)) {
            QTest::addRow("%s", qUtf8Printable(iter.fileInfo().completeBaseName()))
                << smithyFilePath << jsonFilePath;
        }
    }
}

void TestIdl::idlToAst()
{
    QFETCH(const QString, idlFileName);
    QFETCH(const QString, astFileName);

    QFile astFile(astFileName);
    QVERIFY(astFile.open(QFile::ReadOnly));
    QJsonParseError jsonError{};
    const QJsonDocument expectedAst = QJsonDocument::fromJson(astFile.readAll(), &jsonError);
    QEXPECT_FAIL("traits", "Qt fails to handle big decimal 1.7976931348623157E+309", Abort);
    QCOMPARE(jsonError.error, QJsonParseError::NoError);
    QVERIFY(expectedAst.isObject());

    QFile idlFile(idlFileName);
    QVERIFY(idlFile.open(QFile::ReadOnly));
    smithy::IdlParseError idlError{};
    const QJsonObject actualAst = smithy::idlToAst(idlFile.readAll(), &idlError);
    QCOMPARE(idlError.error, smithy::IdlParseError::NoError);
    QEXPECT_FAIL("", "Not implemented yet", Continue);
    QCOMPARE(actualAst, expectedAst.object());
}

QTEST_MAIN(TestIdl)
