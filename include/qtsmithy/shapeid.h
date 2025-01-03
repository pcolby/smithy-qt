// SPDX-FileCopyrightText: 2013-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the ShapeId class.
 */

#ifndef QTSMITHY_SHAPEID_H
#define QTSMITHY_SHAPEID_H

#include "qtsmithy_global.h"

#include <QtGlobal> // \todo Remove if/when dropping support for Qt 6.0.x.
#if (QT_VERSION_CHECK(6, 0, 0) <= QT_VERSION) && (QT_VERSION < QT_VERSION_CHECK(6, 1, 0))
#include <limits> // https://bugreports.qt.io/browse/QTBUG-89977
#endif

#include <QCoreApplication>
#include <QHash>

QTSMITHY_DECLARE_TEST(ShapeId)

QTSMITHY_BEGIN_NAMESPACE

class ShapeIdPrivate;

class QTSMITHY_EXPORT ShapeId
{
Q_DECLARE_TR_FUNCTIONS(ShapeId);

public:
    ShapeId();
    ShapeId(ShapeId &&other);
    ShapeId(const ShapeId &other);
    ShapeId(const QString &shapeId);
    ShapeId& operator=(const ShapeId &shapeId);
    ShapeId& operator=(const ShapeId &&shapeId);
    ShapeId& operator=(const QString &shapeId);
    ~ShapeId();

    QString memberName() const;
    QString nameSpace() const;
    QString shapeName() const;

    void setMemberName(const QString &name);
    void setNameSpace(const QString &name);
    void setShapeName(const QString &name);

    QString absoluteShapeId() const;
    QString relativeShapeId() const;
    QString toString() const;

    bool hasNameSpace() const;
    bool hasMemberName() const;
    bool isAbsoluteRootShapeId() const;
    bool isRootShapeId() const;
    bool isRelativeShapeId() const;
    bool isValid() const;

    bool operator==(const ShapeId &other) const;

private:
    /// \cond internal
    ShapeIdPrivate * d_ptr; ///< Internal d-pointer.
    /// \endcond

    Q_DECLARE_PRIVATE(ShapeId)
    QTSMITHY_BEFRIEND_TEST(ShapeId)
};

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
QTSMITHY_EXPORT uint qHash(const ShapeId &key, uint seed = 0);
#else
QTSMITHY_EXPORT size_t qHash(const ShapeId &key, size_t seed = 0);
#endif

typedef QHash<ShapeId, QString> ShapeIdStringMap;

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_SHAPEID_H
