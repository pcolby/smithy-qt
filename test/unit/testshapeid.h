// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QTest>

class TestShapeId : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase_data();

    void nullConstruction();

    void copyConstruction();

    void moveConstruction();

    void stringConstruction();

    void copyAssignment();

    void moveAssignment();

    void stringAssignment();

    void memberName();

    void nameSpace();

    void shapeName();

    void setMemberName();

    void setNameSpace();

    void setShapeName();

    void absoluteShapeId();

    void relativeShapeId();

    void toString();

    void hasNameSpace();

    void hasMemberName();

    void isAbsoluteRootShapeId();

    void isRootShapeId();

    void isRelativeShapeId();

    void isValid();

    void setShapeId();
};
