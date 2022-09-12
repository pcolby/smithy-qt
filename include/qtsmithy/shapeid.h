// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the ShapeId class.
 */

#ifndef QTSMITHY_SHAPEID_H
#define QTSMITHY_SHAPEID_H

#include "qtsmithy_global.h"

#include <QObject>

QTSMITHY_DECLARE_TEST(ShapeId)

QTSMITHY_BEGIN_NAMESPACE

class ShapeIdPrivate;

class QTSMITHY_EXPORT ShapeId
{
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

private:
    /// \cond internal
    ShapeIdPrivate * d_ptr; ///< Internal d-pointer.
    /// \endcond

    Q_DECLARE_PRIVATE(ShapeId)
    QTSMITHY_BEFRIEND_TEST(ShapeId)
};

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_SHAPEID_H
