// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testidl.h"

#include "qtsmithy/idl.h"

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
    QDir dir(dataPathName);
    QVERIFY2(dir.exists(),
             qUtf8Printable(QStringLiteral("found data does not exist: %1").arg(dataPathName)));
    qDebug() << "Loading test data from" << dir.canonicalPath();

    /// \todo load data.

    QTest::addRow("null") << QString{} << QString{};
}

void TestIdl::idlToAst()
{
    QFETCH(const QString, idlFileName);
    QFETCH(const QString, astFileName);

    QSKIP("Not yet implemented.");

    QFile idlFile(idlFileName), astFile(astFileName);
    QVERIFY(idlFile.open(QFile::ReadOnly));
    QVERIFY(astFile.open(QFile::ReadOnly));

    smithy::IdlParseError error{};
    const QJsonObject ast = smithy::idlToAst(idlFile.readAll(), &error);
    QCOMPARE(error.error, smithy::IdlParseError::NoError);
    QCOMPARE(ast, QJsonDocument::fromJson(astFile.readAll()).object());
}

QTEST_MAIN(TestIdl)
