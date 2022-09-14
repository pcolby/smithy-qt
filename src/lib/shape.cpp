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
 * The Shape class provides a Qt representation of a Smithy semantic shape.
 *
 * \see https://awslabs.github.io/smithy/2.0/spec/shape.html#semantic-shape
 */

/*!
 * Constructs a new, empty Smithy shape.
 */
Shape::Shape() : d_ptr(new ShapePrivate(this))
{
    /// \todo Load the Smithy prelude here?
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

bool Shape::isValid() const
{
    /// \todo
    return false;
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

/// \endcond

QTSMITHY_END_NAMESPACE
