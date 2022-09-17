// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the Shape class.
 */

#ifndef QTSMITHY_SHAPE_H
#define QTSMITHY_SHAPE_H

#include <QCoreApplication>
#include <QHash>
#include <QJsonObject>
#include <QObject>

#include "shapeid.h"

QTSMITHY_DECLARE_TEST(Shape)

QTSMITHY_BEGIN_NAMESPACE

class ShapePrivate;

class QTSMITHY_EXPORT Shape
{
Q_DECLARE_TR_FUNCTIONS(Shape);

public:
    // https://awslabs.github.io/smithy/2.0/spec/model.html#shape-types
    enum class Type {
        // Simply Types
        Blob       = 0x101, ///< Uninterpreted binary data.
        Boolean    = 0x102, ///< Boolean value type.
        String     = 0x103, ///< UTF-8 encoded string.
        Enum       = 0x104, ///< A string with a fixed set of values.
        Byte       = 0x105, ///< 8-bit signed integer ranging from -128 to 127 (inclusive).
        Short      = 0x106, ///< 16-bit signed integer ranging from -32,768 to 32,767 (inclusive).
        Integer    = 0x107, ///< 32-bit signed integer ranging from -2^31 to (2^31)-1 (inclusive).
        IntEnum    = 0x108, ///< An integer with a fixed set of values.
        Long       = 0x109, ///< 64-bit signed integer ranging from -2^63 to (2^63)-1 (inclusive).
        Float      = 0x10A, ///< Single precision IEEE-754 floating point number.
        Double     = 0x10B, ///< Double precision IEEE-754 floating point number.
        BigInteger = 0x10C, ///< Arbitrarily large signed integer.
        BigDecimal = 0x10D, ///< Arbitrary precision signed decimal number.
        Timestamp  = 0x10E, ///< An instant in time with no UTC offset or timezone.
        Document   = 0x10F, ///< Open content that functions as a kind of "any" type.

        // Aggregate Types
        List      = 0x201, ///< Ordered collection of homogeneous values.
        Map       = 0x202, ///< Map data structure that maps string keys to homogeneous values.
        Structure = 0x203, ///< Fixed set of named heterogeneous members.
        Union     = 0x204, ///< Tagged union data structure that can take on one of several
                           ///< different, but fixed, types

        // Service Types
        Service   = 0x301, ///< Entry point of an API that aggregates resources and operations together.
        Operation = 0x302, ///< Represents the input, output, and errors of an API operation.
        Resource  = 0x303, ///< Entity with an identity that has a set of operations
    };

    Shape();
    Shape(const ShapeId &id, const QJsonObject &ast);
    Shape(Shape &&other);
    Shape(const Shape &other);
    Shape& operator=(const Shape &shape);
    Shape& operator=(const Shape &&shape);
    ~Shape();

    ShapeId id() const;

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
