// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the Shape class.
 */

#ifndef QTSMITHY_SHAPE_H
#define QTSMITHY_SHAPE_H

#include <QCoreApplication>
#include <QJsonObject>
#include <QJsonValue>

#include "shapeid.h"

QTSMITHY_DECLARE_TEST(Shape)

QTSMITHY_BEGIN_NAMESPACE

class ShapePrivate;

class QTSMITHY_EXPORT Shape
{
Q_DECLARE_TR_FUNCTIONS(Shape);

public:
    enum class Error {
        NoError                 = 0,
        UndefinedShapeType      = 1,
        MissingRequiredProperty = 2,
        InvalidPropertyValue    = 3,
    };

    // https://awslabs.github.io/smithy/2.0/spec/model.html#shape-types
    enum class Type {
        Undefined  = 0x000, ///< The shape is undefined, usually the result of an error condition.

        // Simple Types
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
        Set       = 0x201, ///< Deprecated; use a list with the uniqueItems trait instead.
        Map       = 0x202, ///< Map data structure that maps string keys to homogeneous values.
        Structure = 0x203, ///< Fixed set of named heterogeneous members.
        Union     = 0x204, ///< Tagged union data structure that can take on one of several
                           ///< different, but fixed, types

        // Service Types
        Service   = 0x301, ///< Entry point of an API that aggregates resources and operations together.
        Operation = 0x302, ///< Represents the input, output, and errors of an API operation.
        Resource  = 0x303, ///< Entity with an identity that has a set of operations

        // Special Types
        Apply     = 0x401, ///< Traits to be applied to shapes outside of their definition.
    };

    // Map of absolute shape ID of a trait shape to the value to assign to the trait.
    typedef QHash<ShapeId, QJsonValue> TraitsMap;

    // https://awslabs.github.io/smithy/2.0/spec/json-ast.html#ast-shape-reference
    struct ShapeReference {
        ShapeId target;
    };
    typedef QHash<QString, ShapeReference> StringShapeRefMap;
    typedef QSet<ShapeReference> ShapeReferences;

    // https://awslabs.github.io/smithy/2.0/spec/json-ast.html#ast-member
    struct Member : public ShapeReference {
        TraitsMap traits;
    };
    typedef QHash<QString, Member> StringMemberMap;

    Shape();
    Shape(const QJsonObject &ast, const ShapeId &id = ShapeId{});
    Shape(Shape &&other);
    Shape(const Shape &other);
    Shape& operator=(const Shape &shape);
    Shape& operator=(const Shape &&shape);
    ~Shape();

    Error error() const;
    bool isValid() const;

    ShapeId id() const;
    Type type() const;
    TraitsMap traits() const;

    // List (and set) shape https://awslabs.github.io/smithy/2.0/spec/json-ast.html#list-shapes
    Member member() const;

    // Map shape https://awslabs.github.io/smithy/2.0/spec/json-ast.html#map-shape
    Member key() const;
    Member value() const;

    // Structure, union, enum, and intEnum shapes
    // https://awslabs.github.io/smithy/2.0/spec/json-ast.html#structure-union-enum-and-intenum-shapes
    StringMemberMap members() const;

    // Service shape https://awslabs.github.io/smithy/2.0/spec/json-ast.html#service-shape
    QString version() const;
    ShapeReferences operations() const; // And resource shape.
    ShapeReferences resources() const;  // And resource shape.
    ShapeReferences errors() const;     // And operation shape.
    ShapeIdStringMap rename() const;

    // Resource shape https://awslabs.github.io/smithy/2.0/spec/json-ast.html#resource-shape
    StringShapeRefMap identifiers() const;
    StringShapeIdMap properties() const;
    ShapeReference create() const;
    ShapeReference put() const;
    ShapeReference read() const;
    ShapeReference update() const;
    ShapeReference Delete() const;
    ShapeReference list() const;
  //ShapeReferences operations() const; // And service shape.
    ShapeReferences collectionOperations() const;
  //ShapeReferences resources() const;  // And service shape.

    // Operation shape https://awslabs.github.io/smithy/2.0/spec/json-ast.html#operation-shape
    ShapeReference input() const;
    ShapeReference output() const;
  //ShapeReferences errors() const; // And service shape.

    // Mixins https://awslabs.github.io/smithy/2.0/spec/json-ast.html#mixins
    ShapeReferences mixins() const;

protected:
    /// \cond internal
    ShapePrivate * d_ptr; ///< Internal d-pointer.
    /// \endcond

private:
    Q_DECLARE_PRIVATE(Shape)
    QTSMITHY_BEFRIEND_TEST(Shape)
};

QTSMITHY_EXPORT size_t qHash(const Shape::ShapeReference &key, size_t seed = 0);

QTSMITHY_EXPORT bool operator==(const Shape::ShapeReference &lhs, const Shape::ShapeReference& rhs);

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_SHAPE_H
