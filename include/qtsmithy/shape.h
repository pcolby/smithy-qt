// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the Shape class.
 */

#ifndef QTSMITHY_SHAPE_H
#define QTSMITHY_SHAPE_H

#include <QHash>
#include <QJsonObject>
#include <QObject>

#include "shapeid.h"

QTSMITHY_DECLARE_TEST(Shape)

QTSMITHY_BEGIN_NAMESPACE

class ShapePrivate;

class QTSMITHY_EXPORT Shape
{
public:
    // https://awslabs.github.io/smithy/2.0/spec/model.html#shape-types
    enum class Type {
        // Simply Types
        Blob       = 0x101,
        Boolean    = 0x102,
        String     = 0x103,
        Enum       = 0x104,
        Byte       = 0x105,
        Short      = 0x106,
        Integer    = 0x107,
        IntEnum    = 0x108,
        Long       = 0x109,
        Float      = 0x10A,
        Double     = 0x10B,
        BigInteger = 0x10C,
        BigDecimal = 0x10D,
        Timestamp  = 0x10E,
        Document   = 0x10F,

        // Aggregate Types
        List      = 0x201,
        Map       = 0x202,
        Structure = 0x203,
        Union     = 0x204,

        // Service Types
        Service   = 0x301,
        Operation = 0x302,
        Resource  = 0x303,
    };

    Shape();
    Shape(Shape &&other);
    Shape(const Shape &other);
    Shape& operator=(const Shape &shape);
    Shape& operator=(const Shape &&shape);
    ~Shape();

    bool isValid() const;

    Type type() const;

protected:
    /// \cond internal
    ShapePrivate * d_ptr; ///< Internal d-pointer.
    /// \endcond

private:
    Q_DECLARE_PRIVATE(Shape)
    QTSMITHY_BEFRIEND_TEST(Shape)
};

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_SHAPE_H
