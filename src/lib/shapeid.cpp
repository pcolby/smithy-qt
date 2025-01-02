// SPDX-FileCopyrightText: 2013-2025 Paul Colby <git@colby.id.au>
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
ShapeId::ShapeId() : d_ptr(new ShapeIdPrivate(this))
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
 * Constructs a ShapeId object by parsing the Smithy Shape ID given by \a shapeId.
 *
 * To be considered valid, \a shapeId must contain at least a valid shape name, but may also contain
 * optional namespace and member name components. Use isValid() to verify \a shapeId's validity.
 *
 * \see isValid
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
    delete d_ptr;
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

/*!
 * Set the Shape ID's member name to \a name, which may be an empty or null string.
 */
void ShapeId::setMemberName(const QString &name)
{
    Q_D(ShapeId);
    d->memberName = name;
}

/*!
 * Set the Shape ID's namespace to \a name, which may be an empty or null string.
 */
void ShapeId::setNameSpace(const QString &name)
{
    Q_D(ShapeId);
    d->nameSpace = name;
}

/*!
 * Set the Shape ID's shape name to \a name.
 *
 * Note, a Shape ID is considered invalid if it has no shape name, so \a name should typically be
 * non-empty.
 */
void ShapeId::setShapeName(const QString &name)
{
    Q_D(ShapeId);
    d->shapeName = name;
}

/*!
 * Returns this object as an absolute Smithy Shape ID if this object has a namespace, otherwise a
 * null string.
 *
 * \note, Smithy defines an absolute Shape ID as one that begins with a namespace, therefore it is
 * not possible to return an absolute Shape ID if no namespace has been set.
 *
 * \note, if the Shape ID is invalid (ie isValid() returns \c false) it still safe to invoke this
 * method, but the result is undefined.
 *
 * \see setNameSpace
 * \see isValid
 */
QString ShapeId::absoluteShapeId() const
{
    return hasNameSpace() ? QStringLiteral("%1#%2").arg(nameSpace(), relativeShapeId()) : QString();
}

/*!
 * Returns this object as a relative Smithy Shape ID, that one without a leading namespace.
 *
 * \note, if the Shape ID is invalid (ie isValid() returns \c false) it still safe to invoke this
 * method, but the result is undefined.
 *
 * \see isValid
 */
QString ShapeId::relativeShapeId() const
{
    return hasMemberName() ? QStringLiteral("%1$%2").arg(shapeName(), memberName()) : shapeName();
}

/*!
 * Returns this object as an *absolute* Smithy Shape ID if this object has a namespace, otherwise a
 * *relative* Smithy Shape ID.
 *
 * \note, if the Shape ID is invalid (ie isValid() returns \c false) it still safe to invoke this
 * method, but the result is undefined.
 *
 * \see absoluteShapeId
 * \see relativeShapeId
 * \see isValid
 */
QString ShapeId::toString() const
{
    return hasNameSpace() ? absoluteShapeId() : relativeShapeId();
}

/*!
 * Returns \c true if this Shape ID has a non-empty namespace, otherwise \c false otherwise.
 *
 * \see nameSpace.
 * \see setNameSpace.
 */
bool ShapeId::hasNameSpace() const
{
    return !nameSpace().isEmpty();
}

/*!
 * Returns \c true if this Shape ID has a non-empty member name, otherwise \c false otherwise.
 *
 * \see memberName.
 * \see setMemberName.
 */
bool ShapeId::hasMemberName() const
{
    return !memberName().isEmpty();
}

/*!
 * Returns \c true if this Shape ID is a *root* Shape ID, and has a namespace, \c false otherwise.
 *
 * \see isRootShapeId.
 * \see hasNameSpace.
 */
bool ShapeId::isAbsoluteRootShapeId() const
{
    return isRootShapeId() && hasNameSpace();
}

/*!
 * Returns \c true if this Shape ID is a *root* Shape ID, \c false otherwise.
 *
 * \note, Smithy defines a root Shape ID as one that does not have a member name.
 *
 * \see hasMemberName.
 */
bool ShapeId::isRootShapeId() const
{
    return !hasMemberName();
}

/*!
 * Returns \c true if this Shape ID is a *relative* Shape ID, \c false otherwise.
 *
 * \note, Smithy defines a relative Shape ID as one that does not have a namespace.
 *
 * \see hasNameSpace.
 */
bool ShapeId::isRelativeShapeId() const
{
    return !hasNameSpace();
}

/*!
 * Returns true if this object represents a valid, non-empty Smithy Shape ID.
 *
 * \see https://awslabs.github.io/smithy/2.0/spec/model.html#shape-id
 */
bool ShapeId::isValid() const
{
    Q_D(const ShapeId);
    // Validate the (optional) namespace.
    if (!d->nameSpace.isEmpty()) {
        static QRegularExpression namespacePattern(QStringLiteral("^(_*[a-zA-Z]\\w*\\.?)+(?<!\\.)$"));
        Q_ASSERT(namespacePattern.isValid());
        if (!namespacePattern.match(d->nameSpace).hasMatch()) {
            return false;
        }
    }

    // Validate the (required) shape name.
    static QRegularExpression identifierPattern(QStringLiteral("^_*[a-zA-Z]\\w*$"));
    Q_ASSERT(identifierPattern.isValid());
    if ((d->shapeName.isEmpty()) || (!identifierPattern.match(d->shapeName).hasMatch())) {
        return false;
    }

    // Validate the (optional) member name.
    if ((!d->memberName.isEmpty()) && (!identifierPattern.match(d->memberName).hasMatch())) {
        return false;
    }
    return true; // Valid.
}

bool ShapeId::operator==(const ShapeId &other) const
{
    Q_D(const ShapeId);
    return (d->memberName == other.d_ptr->memberName) &&
           (d->nameSpace == other.d_ptr->nameSpace)   &&
           (d->shapeName == other.d_ptr->shapeName);
}

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
uint qHash(const ShapeId &key, uint seed)
#else
size_t qHash(const ShapeId &key, size_t seed)
#endif
{
    return ::qHash(key.toString(), seed);
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

/*!
 * Splits \a shapeId into its components (namespace, shape name and member name) and assigns them
 * to the equivalent object members.
 *
 * Both the namespace and member name are optional; their equivalent object members will be set to
 * empty strings if not present in \a shapeId.
 */
void ShapeIdPrivate::setShapeId(const QString &shapeId)
{
    const int sep1 = shapeId.indexOf(QLatin1Char('#'));
    const int sep2 = shapeId.lastIndexOf(QLatin1Char('$'));
    if (sep1 > 0) nameSpace = shapeId.mid(0, sep1);
    if (sep2 > 0) memberName = shapeId.mid(sep2+1);
    shapeName = shapeId.mid(sep1+1, sep2-sep1-1);
}

/// \endcond

QTSMITHY_END_NAMESPACE
