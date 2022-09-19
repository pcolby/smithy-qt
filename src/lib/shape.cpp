// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defines the Shape and ShapePrivate classes.
 */

#include <qtsmithy/shape.h>
#include "shape_p.h"

QTSMITHY_BEGIN_NAMESPACE

/*!
 * \class Shape
 *
 * The Shape class provides a Qt representation of a Smithy semantic shape. That is, essentially a
 * tagged union or variant class, with some specialised conversions for native Qt types.
 *
 * \see https://awslabs.github.io/smithy/2.0/spec/shape.html#semantic-shape
 */

/*!
 * Constructs a new, empty Smithy shape.
 */
Shape::Shape() : d_ptr(new ShapePrivate(this))
{

}

Shape::Shape(const ShapeId &id, const QJsonObject &ast) : d_ptr(new ShapePrivate(this))
{
    Q_D(Shape);
    d->id = id;
    d->type = ShapePrivate::getType(ast);
    Q_UNUSED(ast); /// \todo
}

Shape::Shape(Shape &&other) : d_ptr(new ShapePrivate(this))
{
    Q_D(Shape);
    Q_UNUSED(other); /// \todo
}

Shape::Shape(const Shape &other) : d_ptr(new ShapePrivate(this))
{
    Q_D(Shape);
    Q_UNUSED(other); /// \todo
}

Shape& Shape::operator=(const Shape &shape)
{
    Q_D(Shape);
    Q_UNUSED(shape); /// \todo
    return *this;
}

Shape& Shape::operator=(const Shape &&shape)
{
    Q_D(Shape);
    Q_UNUSED(shape); /// \todo
    return *this;
}

/*!
 * Destroys this Shape object.
 */
Shape::~Shape()
{

}

ShapeId Shape::id() const
{
    Q_D(const Shape);
    return d->id;
}

Shape::Type Shape::type() const
{
    Q_D(const Shape);
    return d->type;
}

bool Shape::isValid() const
{
    Q_D(const Shape);
    if (!d->id.isValid()) {
        return false;
    }
    return false; /// \todo
}

/*!
 * \cond internal
 * \class ShapePrivate
 *
 * The ShapePrivate class provides private implementation for Shape.
 */

/*!
 * \internal
 * Constructs a new ShapePrivate object with public implementation \a q.
 */
ShapePrivate::ShapePrivate(Shape * const q) : q_ptr(q)
{

}

Shape::Type ShapePrivate::getType(const QJsonObject &ast)
{
    const QJsonValue value = ast.value(QLatin1String("type"));
    if (value.isUndefined()) {
        qCCritical(lc).noquote() << tr("Shape has no type property");
        return Shape::Type::Undefined;
    }
    if (value.type() != QJsonValue::String) {
        qCCritical(lc).noquote() << tr("Shape type property is not a JSON string:") << value;
        return Shape::Type::Undefined;
    }
    return getType(value.toString());
}

Shape::Type ShapePrivate::getType(const QString &type)
{
    // Simple Types
    if (type == QLatin1String("blob"))       return Shape::Type::Blob;
    if (type == QLatin1String("boolean"))    return Shape::Type::Boolean;
    if (type == QLatin1String("string"))     return Shape::Type::String;
    if (type == QLatin1String("enum"))       return Shape::Type::Enum;
    if (type == QLatin1String("byte"))       return Shape::Type::Byte;
    if (type == QLatin1String("short"))      return Shape::Type::Short;
    if (type == QLatin1String("integer"))    return Shape::Type::Integer;
    if (type == QLatin1String("intEnum"))    return Shape::Type::IntEnum;
    if (type == QLatin1String("long"))       return Shape::Type::Long;
    if (type == QLatin1String("float"))      return Shape::Type::Float;
    if (type == QLatin1String("double"))     return Shape::Type::Double;
    if (type == QLatin1String("bigInteger")) return Shape::Type::BigInteger;
    if (type == QLatin1String("bigDecimal")) return Shape::Type::BigDecimal;
    if (type == QLatin1String("timestamp"))  return Shape::Type::Timestamp;
    if (type == QLatin1String("document"))   return Shape::Type::Document;

    // Aggregate Types
    if (type == QLatin1String("list"))       return Shape::Type::List;
    if (type == QLatin1String("set"))        return Shape::Type::Set;
    if (type == QLatin1String("map"))        return Shape::Type::Map;
    if (type == QLatin1String("structure"))  return Shape::Type::Structure;
    if (type == QLatin1String("union"))      return Shape::Type::Union;


    // Service Types
    if (type == QLatin1String("service"))    return Shape::Type::Service;
    if (type == QLatin1String("operation"))  return Shape::Type::Operation;
    if (type == QLatin1String("resource"))   return Shape::Type::Resource;

    qCWarning(lc).noquote() << tr("Unknown shape type: %1").arg(type);
    return Shape::Type::Undefined;
}

/// \endcond

QTSMITHY_END_NAMESPACE
