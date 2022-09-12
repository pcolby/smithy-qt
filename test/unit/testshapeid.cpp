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

    // Invalid cases.

    QTest::addRow("ns#$member")
        << QStringLiteral("smithy.example.foo#$memberName") << false
        << QStringLiteral("smithy.example.foo")
        << QString()
        << QStringLiteral("memberName");

    QTest::addRow("###")
        << QStringLiteral("###") << false
        << QString()
        << QStringLiteral("##")
        << QString();

    QTest::addRow("$$$")
        << QStringLiteral("$$$") << false
        << QString()
        << QStringLiteral("$$")
        << QString();

    QTest::addRow("reversed")
        << QStringLiteral("ns$shape#member") << false
        << QStringLiteral("ns$shape")
        << QStringLiteral("member")
        << QStringLiteral("shape#member");
}


void TestShapeId::nullConstruction()
{
    smithy::ShapeId sid;
    QVERIFY(!sid.isValid());
    QVERIFY(sid.nameSpace().isNull());
    QVERIFY(sid.shapeName().isNull());
    QVERIFY(sid.memberName().isNull());
}

void TestShapeId::copyConstruction()
{

}

void TestShapeId::moveConstruction()
{

}

void TestShapeId::stringConstruction()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(bool, isValid);
    QFETCH_GLOBAL(QString, nameSpace);
    QFETCH_GLOBAL(QString, shapeName);
    QFETCH_GLOBAL(QString, memberName);

    const smithy::ShapeId sid{shapeId};
    QCOMPARE(sid.isValid(), isValid);
    QCOMPARE(sid.nameSpace(), nameSpace);
    QCOMPARE(sid.shapeName(), shapeName);
    QCOMPARE(sid.memberName(), memberName);
}

void TestShapeId::copyAssignment()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(bool, isValid);
    QFETCH_GLOBAL(QString, nameSpace);
    QFETCH_GLOBAL(QString, shapeName);
    QFETCH_GLOBAL(QString, memberName);

    smithy::ShapeId sid;
    QVERIFY(!sid.isValid());
    QVERIFY(sid.nameSpace().isNull());
    QVERIFY(sid.shapeName().isNull());
    QVERIFY(sid.memberName().isNull());

    sid = smithy::ShapeId{shapeId};
    QCOMPARE(sid.isValid(), isValid);
    QCOMPARE(sid.nameSpace(), nameSpace);
    QCOMPARE(sid.shapeName(), shapeName);
    QCOMPARE(sid.memberName(), memberName);
}

void TestShapeId::moveAssignment()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(bool, isValid);
    QFETCH_GLOBAL(QString, nameSpace);
    QFETCH_GLOBAL(QString, shapeName);
    QFETCH_GLOBAL(QString, memberName);

    smithy::ShapeId sid;
    QVERIFY(!sid.isValid());
    QVERIFY(sid.nameSpace().isNull());
    QVERIFY(sid.shapeName().isNull());
    QVERIFY(sid.memberName().isNull());

    sid = std::move(smithy::ShapeId{shapeId});
    QCOMPARE(sid.isValid(), isValid);
    QCOMPARE(sid.nameSpace(), nameSpace);
    QCOMPARE(sid.shapeName(), shapeName);
    QCOMPARE(sid.memberName(), memberName);
}

void TestShapeId::stringAssignment()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(bool, isValid);
    QFETCH_GLOBAL(QString, nameSpace);
    QFETCH_GLOBAL(QString, shapeName);
    QFETCH_GLOBAL(QString, memberName);

    smithy::ShapeId sid;
    QVERIFY(!sid.isValid());
    QVERIFY(sid.nameSpace().isNull());
    QVERIFY(sid.shapeName().isNull());
    QVERIFY(sid.memberName().isNull());

    sid = shapeId;
    QCOMPARE(sid.isValid(), isValid);
    QCOMPARE(sid.nameSpace(), nameSpace);
    QCOMPARE(sid.shapeName(), shapeName);
    QCOMPARE(sid.memberName(), memberName);
}

void TestShapeId::memberName()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(QString, memberName);
    QVERIFY(smithy::ShapeId{}.memberName().isNull());
    QCOMPARE(smithy::ShapeId{shapeId}.memberName(), memberName);
}

void TestShapeId::nameSpace()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(QString, nameSpace);
    QVERIFY(smithy::ShapeId{}.nameSpace().isNull());
    QCOMPARE(smithy::ShapeId{shapeId}.nameSpace(), nameSpace);
}

void TestShapeId::shapeName()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(QString, shapeName);
    QVERIFY(smithy::ShapeId{}.shapeName().isNull());
    QCOMPARE(smithy::ShapeId{shapeId}.shapeName(), shapeName);
}

void TestShapeId::setMemberName()
{
    QFETCH_GLOBAL(QString, memberName);
    smithy::ShapeId shapeId;
    QVERIFY(shapeId.memberName().isNull());
    shapeId.setMemberName(memberName);
    QCOMPARE(shapeId.memberName(), memberName);
}

void TestShapeId::setNameSpace()
{
    QFETCH_GLOBAL(QString, nameSpace);
    smithy::ShapeId shapeId;
    QVERIFY(shapeId.nameSpace().isNull());
    shapeId.setNameSpace(nameSpace);
    QCOMPARE(shapeId.nameSpace(), nameSpace);
}

void TestShapeId::setShapeName()
{
    QFETCH_GLOBAL(QString, shapeName);
    smithy::ShapeId shapeId;
    QVERIFY(shapeId.shapeName().isNull());
    shapeId.setShapeName(shapeName);
    QCOMPARE(shapeId.shapeName(), shapeName);
}

void TestShapeId::absoluteShapeId()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(QString, nameSpace);
    QFETCH_GLOBAL(QString, shapeName);
    QFETCH_GLOBAL(QString, memberName);

    QString expected = shapeName;
    if (!memberName.isEmpty()) {
        expected.append(QLatin1Char('$') + memberName);
    }
    if (!nameSpace.isEmpty()) {
        expected.prepend(nameSpace + QLatin1Char('#'));
    } else {
        expected.clear();
    }

    QCOMPARE(smithy::ShapeId{shapeId}.absoluteShapeId(), expected);
}

void TestShapeId::relativeShapeId()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(QString, shapeName);
    QFETCH_GLOBAL(QString, memberName);

    QString expected = shapeName;
    if (!memberName.isEmpty()) {
        expected.append(QLatin1Char('$') + memberName);
    }

    QCOMPARE(smithy::ShapeId{shapeId}.relativeShapeId(), expected);
}

void TestShapeId::toString()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(QString, nameSpace);
    QFETCH_GLOBAL(QString, shapeName);
    QFETCH_GLOBAL(QString, memberName);

    QString expected = shapeName;
    if (!nameSpace.isEmpty()) {
        expected.prepend(nameSpace + QLatin1Char('#'));
    }
    if (!memberName.isEmpty()) {
        expected.append(QLatin1Char('$') + memberName);
    }

    QCOMPARE(smithy::ShapeId{shapeId}.toString(), expected);
}

void TestShapeId::hasNameSpace()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(QString, nameSpace);
    QCOMPARE(smithy::ShapeId{shapeId}.hasNameSpace(), !nameSpace.isEmpty());
}

void TestShapeId::hasMemberName()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(QString, memberName);
    QCOMPARE(smithy::ShapeId{shapeId}.hasMemberName(), !memberName.isEmpty());
}

void TestShapeId::isAbsoluteRootShapeId()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(QString, nameSpace);
    QFETCH_GLOBAL(QString, memberName);
    QCOMPARE(smithy::ShapeId{shapeId}.isAbsoluteRootShapeId(),
             !nameSpace.isEmpty() && memberName.isEmpty());
}

void TestShapeId::isRootShapeId()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(bool, isValid);
    QFETCH_GLOBAL(QString, memberName);
    if (isValid) {
        QCOMPARE(smithy::ShapeId{shapeId}.isRootShapeId(), memberName.isEmpty());
    }
}

void TestShapeId::isRelativeShapeId()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(QString, nameSpace);
    QCOMPARE(smithy::ShapeId{shapeId}.isRelativeShapeId(), nameSpace.isEmpty());
}

void TestShapeId::isValid()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(bool, isValid);
    QCOMPARE(smithy::ShapeId{shapeId}.isValid(), isValid);
}

void TestShapeId::setShapeId()
{
    QFETCH_GLOBAL(QString, shapeId);
    QFETCH_GLOBAL(bool, isValid);
    QFETCH_GLOBAL(QString, nameSpace);
    QFETCH_GLOBAL(QString, shapeName);
    QFETCH_GLOBAL(QString, memberName);

    smithy::ShapeId sid;
    sid.d_ptr->setShapeId(shapeId);
    QCOMPARE(sid.isValid(), isValid);
    QCOMPARE(sid.nameSpace(), nameSpace);
    QCOMPARE(sid.shapeName(), shapeName);
    QCOMPARE(sid.memberName(), memberName);
}

QTEST_MAIN(TestShapeId)
