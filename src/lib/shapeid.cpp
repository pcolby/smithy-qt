// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defines the ShapeId and ShapeIdPrivate classes.
 */

#include <qtsmithy/shapeid.h>
#include "shapeid_p.h"

#include <QRegularExpression>

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
    d->memberName = std::move(other.d_ptr->memberName);
    d->nameSpace = std::move(other.d_ptr->nameSpace);
    d->shapeName = std::move(other.d_ptr->shapeName);
}

/*!
 * Constructs a ShapeId object by copying \a other.
 */
ShapeId::ShapeId(const ShapeId &other) : d_ptr(new ShapeIdPrivate(this))
{
    Q_D(ShapeId);
    d->memberName = other.d_ptr->memberName;
    d->nameSpace = other.d_ptr->nameSpace;
    d->shapeName = other.d_ptr->shapeName;
}

/*!
 * Constructs a ShapeId object by parsing a Smithy Shape ID given by \a shapeId.
 *
 * (?:(?<ns>_*[a-zA-Z]\w*\.?)+(?<!\.)#)?(?<shape>_*[a-zA-Z]\w*)(?:\$(?<member>_*[a-zA-Z]\w*))?$
 *
 * namepspace: ^(_*[a-zA-Z]\w*\.?)+(?<!\.)$
 * shape name:  ^_*[a-zA-Z]\w*$
 * member name: ^_*[a-zA-Z]\w*$
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
    d->memberName = shapeId.d_ptr->memberName;
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
    d->memberName = std::move(shapeId.d_ptr->memberName);
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
 * Returns the last error.
 */
ShapeId::Error ShapeId::error() const
{
    Q_D(const ShapeId);
    return d->error;
}

/*!
 * Returns a human-readable description of the last error.
 */
QString ShapeId::errorString() const
{
    Q_D(const ShapeId);
    return d->errorString();
}

/*!
 * Returns the Shape ID's member name, if it has one, otherwise a null string.
 */
QString ShapeId::memberName() const
{
    Q_D(const ShapeId);
    return d->memberName;
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

void ShapeId::setMemberName(const QString &name)
{
    Q_D(ShapeId);
    static QRegularExpression regex(QStringLiteral("^_*[a-zA-Z]\\w*$"));
    if ((!name.isEmpty()) && (!regex.match(name).hasMatch())) {
        d->error = Error::InvalidMemberName;
        return;
    }
    d->memberName = name;
}

void ShapeId::setNameSpace(const QString &name)
{
    Q_D(ShapeId);
    static QRegularExpression regex(QStringLiteral("^(_*[a-zA-Z]\\w*\\.?)+(?<!\\.)$"));
    if ((!name.isEmpty()) && (!regex.match(name).hasMatch())) {
        d->error = Error::InvalidNamespace;
        return;
    }
    d->nameSpace = name;
}

void ShapeId::setShapeName(const QString &name)
{
    Q_D(ShapeId);
    if (name.isEmpty()) {
        d->error = Error::EmptyShapeName;
        return;
    }
    static QRegularExpression regex(QStringLiteral("^_*[a-zA-Z]\\w*$"));
    if (!regex.match(name).hasMatch()) {
        d->error = Error::InvalidShapeName;
        return;
    }
    d->shapeName = name;
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
            ? QStringLiteral("%1$%2").arg(d->shapeName, d->memberName)
            : d->shapeName
        : QString();
}

QString ShapeId::toString() const
{
    return hasNameSpace() ? absoluteShapeId() : relativeShapeId();
}

bool ShapeId::hasNameSpace() const
{
    return !nameSpace().isEmpty();
}

bool ShapeId::hasMemberName() const
{
    return !memberName().isEmpty();
}

bool ShapeId::isAbsoluteRootShapeId() const
{
    return isRootShapeId() && hasNameSpace();
}

bool ShapeId::isRootShapeId() const
{
    return !hasMemberName();
}

bool ShapeId::isRelativeShapeId() const
{
    return !hasNameSpace();
}

bool ShapeId::isValid() const
{
    return (error() == Error::NoError) && (!shapeName().isEmpty());
}

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
