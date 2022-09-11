// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "testshapeid.h"

#include "qtsmithy/shapeid.h"
#include "shapeid_p.h"

void TestShapeId::initTestCase_data()
{
    QTest::addColumn<QString>("shapeId");
    QTest::addColumn<bool>("isValid");
    QTest::addColumn<QString>("nameSpace");
    QTest::addColumn<QString>("shapeName");
    QTest::addColumn<QString>("memberName");

    QTest::addRow("null") << QString() << false << QString() << QString() << QString();

    // Valid cases.

    QTest::addRow("shape")
        << QStringLiteral("ExampleShapeName") << true
        << QString()
        << QStringLiteral("ExampleShapeName")
        << QString();

    QTest::addRow("shape$member")
        << QStringLiteral("ExampleShapeName$memberName") << true
        << QString()
        << QStringLiteral("ExampleShapeName")
        << QStringLiteral("memberName");

    QTest::addRow("ns#shape")
        << QStringLiteral("smithy.example.foo#ExampleShapeName") << true
        << QStringLiteral("smithy.example.foo")
        << QStringLiteral("ExampleShapeName")
        << QString();

    QTest::addRow("ns#shape$member")
        << QStringLiteral("smithy.example.foo#ExampleShapeName$memberName") << true
        << QStringLiteral("smithy.example.foo")
        << QStringLiteral("ExampleShapeName")
        << QStringLiteral("memberName");

    /// \todo Invalid cases.
}


void TestShapeId::isValid()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(bool, isValid);
    QCOMPARE(smithy::ShapeId(shapeId).isValid(), isValid);
}

//    QFETCH_GLOBAL(QString, nameSpace); // Not under test.
//    QFETCH_GLOBAL(QString, shapeName); // Not under test.
//    QFETCH_GLOBAL(QString, memberName); // Not under test.

QTEST_MAIN(TestShapeId)
