// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defines the ShapeId and ShapeIdPrivate classes.
 */

#include <qtsmithy/shapeid.h>
#include "shapeid_p.h"

QTSMITHY_BEGIN_NAMESPACE

/*!
 * \class ShapeId
 *
 * The ShapeId class provides a Qt representation of a Smithy Shape ID.
 *
 * \see https://awslabs.github.io/smithy/2.0/spec/model.html#shape-id
 */

/*!
 * Constructs an empty ShapeId object.
 */
ShapeId::ShapeId()
{

}

/*!
 * Constructs a ShapeId object by moving resources from \a other.
 */
ShapeId::ShapeId(ShapeId &&other) : d_ptr(new ShapeIdPrivate(this))
{
    Q_D(ShapeId);
    d->membeName = std::move(other.d_ptr->membeName);
    d->nameSpace = std::move(other.d_ptr->nameSpace);
    d->shapeName = std::move(other.d_ptr->shapeName);
}

/*!
 * Constructs a ShapeId object by copying \a other.
 */
ShapeId::ShapeId(const ShapeId &other) : d_ptr(new ShapeIdPrivate(this))
{
    Q_D(ShapeId);
    d->membeName = other.d_ptr->membeName;
    d->nameSpace = other.d_ptr->nameSpace;
    d->shapeName = other.d_ptr->shapeName;
}

/*!
 * Constructs a ShapeId object by parsing a Smithy Shape ID given by \a shapeId.
 *
 * \see https://awslabs.github.io/smithy/2.0/spec/model.html#shape-id
 */
ShapeId::ShapeId(const QString &shapeId) : d_ptr(new ShapeIdPrivate(this))
{
    Q_D(ShapeId);
    d->setShapeId(shapeId);
}

/*!
 * Assigns the specified \a shapeId to this object.
 */
ShapeId& ShapeId::operator=(const ShapeId &shapeId)
{
    Q_D(ShapeId);
    d->membeName = shapeId.d_ptr->membeName;
    d->nameSpace = shapeId.d_ptr->nameSpace;
    d->shapeName = shapeId.d_ptr->shapeName;
    return *this;
}

/*!
 * Moves the specified \a shapeId to this object.
 */
ShapeId& ShapeId::operator=(const ShapeId &&shapeId)
{
    Q_D(ShapeId);
    d->membeName = std::move(shapeId.d_ptr->membeName);
    d->nameSpace = std::move(shapeId.d_ptr->nameSpace);
    d->shapeName = std::move(shapeId.d_ptr->shapeName);
    return *this;
}

/*!
 * Assigns the specified \a shapeId to this object.
 */
ShapeId& ShapeId::operator=(const QString &shapeId)
{
    Q_D(ShapeId);
    d->setShapeId(shapeId);
    return *this;
}

/*!
 * Destroys this ShapeId object.
 */
ShapeId::~ShapeId()
{

}

/*!
 * Returns the Shape ID's member name, if it has one, otherwise a null string.
 */
QString ShapeId::memberName() const
{
    Q_D(const ShapeId);
    return d->membeName;
}

/*!
 * Returns the Shape ID's namespace, if it has one, otherwise a null string.
 */
QString ShapeId::nameSpace() const
{
    Q_D(const ShapeId);
    return d->nameSpace;
}

/*!
 * Returns the Shape ID's shape name, if it has one, otherwise a null string.
 *
 * Note, a Shape ID is considered invalid if it has no shape name.
 *
 * \see isValid
 */
QString ShapeId::shapeName() const
{
    Q_D(const ShapeId);
    return d->shapeName;
}

bool ShapeId::setMemberName(const QString &name)
{
    Q_D(ShapeId);
    /// \todo validate.
}

bool ShapeId::setNameSpace(const QString &name)
{
    Q_D(ShapeId);
    /// \todo validate.
}

bool ShapeId::setShapeName(const QString &name)
{
    Q_D(ShapeId);
    /// \todo validate.
}

QString ShapeId::absoluteShapeId() const
{
    Q_D(const ShapeId);
    return (isValid() && hasNameSpace())
        ? QStringLiteral("%1#%2").arg(d->nameSpace, relativeShapeId())
        : QString();
}

QString ShapeId::relativeShapeId() const
{
    Q_D(const ShapeId);
    return isValid()
        ? (hasMemberName())
            ? QStringLiteral("%1$%2").arg(d->shapeName, d->membeName)
            : d->shapeName
        : QString();
}

QString ShapeId::toString() const
{
    return hasNameSpace() ? absoluteShapeId() : relativeShapeId();
}

// Various categorisations of Smithy Shape IDs.
bool hasNameSpace() const;
bool hasMemberName() const;
bool isAbsoluteRootShapeId() const; ///< ie hasNameSpace and isRootShapeId.
bool isRootShapeId() const; ///< Has no $memmber.
bool isRelativeShapeId() const; ///< !hasNameSpace.
bool isValid() const;

/*!
 * \cond internal
 * \class ShapeIdPrivate
 *
 * The ShapeIdPrivate class provides private implementation for ShapeId.
 */

/*!
 * \internal
 * Constructs a new ShapeIdPrivate object with public implementation \a q.
 */
ShapeIdPrivate::ShapeIdPrivate(ShapeId * const q) : q_ptr(q)
{

}

/// \endcond

QTSMITHY_END_NAMESPACE
