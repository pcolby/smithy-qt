// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testidl.h"

#include "qtsmithy/idl.h"

#include <QJsonDocument>

#ifndef QT_TESTCASE_SOURCEDIR
#define QT_TESTCASE_SOURCEDIR // Just to make QtCreator happy for now.
#endif

void TestIdl::idlToAst_data()
{
    QTest::addColumn<QString>("idlFileName");
    QTest::addColumn<QString>("astFileName");

    QDir dir(QStringLiteral(QT_TESTCASE_SOURCEDIR "/../testdata/aws-labs-smithy/smithy-model/src/"
                            "test/resources/software/amazon/smithy/model/loader/valid/"));

    qDebug() << dir.canonicalPath();

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
