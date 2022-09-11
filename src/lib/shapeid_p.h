// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the ShapeIdPrivate class.
 */

#ifndef QTSMITHY_SHAPEID_P_H
#define QTSMITHY_SHAPEID_P_H

#include <qtsmithy/shapeid.h>

QTSMITHY_BEGIN_NAMESPACE

class QTSMITHY_EXPORT ShapeIdPrivate
{
public:
    explicit ShapeIdPrivate(ShapeId * const q);
    ShapeIdPrivate(ShapeId * const q, const QString &shapeId);

    bool setShapeId(const QString &shapeId);

    QString errorString() const;

protected:
    ShapeId * q_ptr; ///< Internal q-pointer.

private:
    ShapeId::Error error;
    QString memberName;
    QString nameSpace;
    QString shapeName;

    Q_DECLARE_PUBLIC(ShapeId)
    friend class TestShapeId;
};

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_SHAPEID_P_H
