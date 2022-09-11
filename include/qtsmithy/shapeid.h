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

QTSMITHY_BEGIN_NAMESPACE

class ShapeIdPrivate;

class QTSMITHY_EXPORT ShapeId
{
public:
    enum class Error {
        NoError = 0,
        EmptyShapeName = 1,
        InvalidShapeId = 2,
        InvalidNamespace = 3,
        InvalidShapeName = 4,
        InvalidMemberName = 5,
    };

    ShapeId();
    ShapeId(ShapeId &&other);
    ShapeId(const ShapeId &other);
    ShapeId(const QString &shapeId);
    ShapeId& operator=(const ShapeId &shapeId);
    ShapeId& operator=(const ShapeId &&shapeId);
    ShapeId& operator=(const QString &shapeId);
    ~ShapeId();

    Error error() const;
    QString errorString() const;

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
    friend class TestShapeId;
};

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_SHAPEID_H
