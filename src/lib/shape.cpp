// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defines the Shape and ShapePrivate classes.
 */

#include <qtsmithy/shape.h>
#include "shape_p.h"

#include <QJsonArray>

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
    Q_D(Shape);
    d->type = Type::Undefined;
}

Shape::Shape(const QJsonObject &ast, const ShapeId &id) : d_ptr(new ShapePrivate(this))
{
    Q_D(Shape);
    d->id = id;
    d->type = ShapePrivate::getType(ast);

    // Warn on any unsupported properties.
    const QStringList supportedProperties = ShapePrivate::supportedProperties(d->type);
    const QStringList keys = ast.keys();
    for (const QString &key: keys) {
        if (!supportedProperties.contains(key)) {
            qCWarning(d->lc).noquote() << tr("Ignoring unsupported Shape property: %1").arg(key);
        }
    }

    // Error on any missing required properties.
    const QStringList requiredProperties = ShapePrivate::requiredProperties(d->type);
    for (const QString &key: requiredProperties) {
        if (!keys.contains(key)) {
            qCritical(d->lc).noquote() << tr("Missing required Shape property: %1").arg(key);
            /// \todo Set an error flag.
        }
    }

    // Validate all present properties.
    for (auto iter = ast.constBegin(); iter != ast.constEnd(); ++iter) {
        if (!supportedProperties.contains(iter.key())) {
            continue;
        }
        if (!ShapePrivate::validateProperty(iter.key(), iter.value())) {
            /// \todo Set an error flag.
        }
    }
}

Shape::Shape(Shape &&other) : d_ptr(new ShapePrivate(this))
{
    Q_D(Shape);
    d->id = std::move(other.d_ptr->id);
    d->type = std::move(other.d_ptr->type);
}

Shape::Shape(const Shape &other) : d_ptr(new ShapePrivate(this))
{
    Q_D(Shape);
    d->id = other.d_ptr->id;
    d->type = other.d_ptr->type;
}

Shape& Shape::operator=(const Shape &shape)
{
    Q_D(Shape);
    d->id = shape.d_ptr->id;
    d->type = shape.d_ptr->type;
    return *this;
}

Shape& Shape::operator=(const Shape &&shape)
{
    Q_D(Shape);
    d->id = std::move(shape.d_ptr->id);
    d->type = std::move(shape.d_ptr->type);
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
    //Q_D(const Shape);
    return true; /// \todo
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

    // Special Types
    if (type == QLatin1String("apply"))      return Shape::Type::Apply;

    qCWarning(lc).noquote() << tr("Unknown shape type: %1").arg(type);
    return Shape::Type::Undefined;
}

QStringList ShapePrivate::supportedProperties(const Shape::Type &type)
{
    switch (type) {
    case Shape::Type::Undefined:
        return QStringList{};

    // Simple Types:
    case Shape::Type::Blob:
    case Shape::Type::Boolean:
    case Shape::Type::String:
  //case Shape::Type::Enum: <- Same as strucutre and union below.
    case Shape::Type::Byte:
    case Shape::Type::Short:
    case Shape::Type::Integer:
  //case Shape::Type::IntEnum: <- Same as strucutre and union below.
    case Shape::Type::Long:
    case Shape::Type::Float:
    case Shape::Type::Double:
    case Shape::Type::BigInteger:
    case Shape::Type::BigDecimal:
    case Shape::Type::Timestamp:
    case Shape::Type::Document:
        return QStringList{
            QLatin1String("type"),
            QLatin1String("traits"),
            QLatin1String("mixins"),
        };
        break;

    // Aggregate Types
    case Shape::Type::List:
  //case Shape::Type::Set:  <- Set is synonym for List.
        return QStringList{
            QLatin1String("type"),
            QLatin1String("traits"),
            QLatin1String("member"),
            QLatin1String("mixins"),
        };
        break;
    case Shape::Type::Map:
        return QStringList{
            QLatin1String("type"),
            QLatin1String("traits"),
            QLatin1String("key"),
            QLatin1String("value"),
            QLatin1String("mixins"),
        };
        break;
    case Shape::Type::Structure:
    case Shape::Type::Union:
    case Shape::Type::Enum:
    case Shape::Type::IntEnum:
        return QStringList{
            QLatin1String("type"),
            QLatin1String("traits"),
            QLatin1String("members"),
            QLatin1String("mixins"),
        };

    // Service Types
    case Shape::Type::Service:
        return QStringList{
            QLatin1String("type"),
            QLatin1String("traits"),
            QLatin1String("version"),
            QLatin1String("operations"),
            QLatin1String("resources"),
            QLatin1String("errors"),
            QLatin1String("rename"),
            QLatin1String("mixins"),
        };
    case Shape::Type::Operation:
        return QStringList{
            QLatin1String("type"),
            QLatin1String("traits"),
            QLatin1String("input"),
            QLatin1String("output"),
            QLatin1String("errors"),
            QLatin1String("mixins"),
        };
    case Shape::Type::Resource:
        return QStringList{
            QLatin1String("type"),
            QLatin1String("traits"),
            QLatin1String("identifiers"),
            QLatin1String("properties"),
            QLatin1String("create"),
            QLatin1String("put"),
            QLatin1String("read"),
            QLatin1String("update"),
            QLatin1String("delete"),
            QLatin1String("list"),
            QLatin1String("operations"),
            QLatin1String("collectionOperations"),
            QLatin1String("resources"),
            QLatin1String("mixins"),
        };

    // Special Types
    case Shape::Type::Apply:
        return QStringList{
            QLatin1String("type"),
            QLatin1String("traits"),
        };
        break;
    }
    qCWarning(lc).noquote() << tr("Unknown shape type: 0x%1").arg((int)type, 0, 16);
    return QStringList{};
}

QStringList ShapePrivate::requiredProperties(const Shape::Type &type)
{
    switch (type) {
    case Shape::Type::List:
  //case Shape::Type::Set: <- Set is synonym for List.
        return QStringList{
            QLatin1String("type"),
            QLatin1String("member"),
        };
        break;

    case Shape::Type::Map:
        return QStringList{
            QLatin1String("type"),
            QLatin1String("key"),
            QLatin1String("value"),
        };
        break;

    case Shape::Type::Union:
    case Shape::Type::Enum:
    case Shape::Type::IntEnum:
        return QStringList{
            QLatin1String("type"),
            QLatin1String("members"),
        };

    case Shape::Type::Service:
        return QStringList{
            QLatin1String("type"),
            QLatin1String("version"),
        };

    case Shape::Type::Apply:
        return QStringList{
            QLatin1String("type"),
            QLatin1String("traits"),
        };
        break;

    default:
        return QStringList{
            QLatin1String("type"),
        };
    }
}

bool ShapePrivate::validateIdentifier(const QString &id)
{
    const ShapeId shapeId(id);
    return ((shapeId.isValid()) && (!shapeId.hasNameSpace()) && (!shapeId.hasMemberName()));
}

bool ShapePrivate::validateProperty(const QString &name, const QJsonValue &value)
{
    // Type string
    if (name == QLatin1String("type")) {
        if (!value.isString()) {
            qCCritical(lc).noquote() << tr("%1 property is not a JSON string").arg(name);
            return false;
        }
        return (getType(value.toString()) != Shape::Type::Undefined);
    }

    // TraitsMap
    else if (name == QLatin1String("traits")) {
        if (!value.isObject()) {
            qCCritical(lc).noquote() << tr("%1 property is not a JSON object").arg(name);
            return false;
        }
        const QJsonObject traits = value.toObject();
        for (auto iter = traits.constBegin(); iter != traits.constEnd(); ++iter) {
            const ShapeId id(iter.key());
            if (!id.isValid()) {
                qCCritical(lc).noquote() << tr("%1 property has trait with invalid shape ID %2")
                                            .arg(name, iter.key());
                return false;
            }
            if (id.isRelativeShapeId()) {
                qCCritical(lc).noquote() << tr("%1 property has trait with relative shape ID %2")
                                            .arg(name, iter.key());
                return false;
            }
            // It doesn't matter (at this point) what type iter->value() is.
        }
    }

    // Member (aka ShapeReference plus optional TraitsMap)
    else if ((name == QLatin1String("member")) ||
             (name == QLatin1String("key")) ||
             (name == QLatin1String("value")))
    {
        if (!value.isObject()) {
            qCCritical(lc).noquote() << tr("%1 property is not a JSON object").arg(name);
            return false;
        }
        if (!validateProperty(QStringLiteral("ShapeReference"), value)) {
            qCCritical(lc).noquote() << tr("%1 property is not valid ShapeReference").arg(name);
            return false;
        }
        const QJsonObject member = value.toObject();
        const auto traits = member.constFind(QLatin1String("traits")); // Optional.
        if ((traits != member.constEnd()) && (!validateProperty(traits.key(), traits.value()))) {
            qCCritical(lc).noquote() << tr("%1 property has invalid traits property").arg(name);
            return false;
        }
    }

    // Members (ie Set of Member instances)
    else if (name == QLatin1String("members")) {
        if (!value.isObject()) {
            qCCritical(lc).noquote() << tr("%1 property is not a JSON object").arg(name);
            return false;
        }
        QStringList names;
        const QJsonObject members = value.toObject();
        for (auto iter = members.constBegin(); iter != members.constEnd(); ++iter) {
            if (!validateIdentifier(iter.key())) {
                qCCritical(lc).noquote() << tr("%1 property has invalid member name %2")
                                            .arg(name, iter.key());
                return false;
            }
            if (names.contains(iter.key().toLower())) {
                qCCritical(lc).noquote() << tr("%1 property has non-unique member name %2")
                                            .arg(name, iter.key().toLower());
                return false;
            }
            names.append(iter.key().toLower());
            if (!validateProperty(QLatin1String("member"), iter.value())) {
                qCCritical(lc).noquote() << tr("%1 property has invalid value for %2 property")
                                            .arg(name, iter.key());
                return false;
            }
        }
    }

    // Plain string
    else if (name == QLatin1String("version")) {
        if (!value.isString()) {
            qCCritical(lc).noquote() << tr("%1 property is not a JSON string").arg(name);
            return false;
        }
        // No further validation required; the string is free-form.
    }

    // ShapeReferences
    else if ((name == QLatin1String("operations")) ||
             (name == QLatin1String("collectionOperations")) ||
             (name == QLatin1String("resources")) ||
             (name == QLatin1String("errors")) ||
             (name == QLatin1String("mixins"))) {
        if (!value.isArray()) {
            qCCritical(lc).noquote() << tr("%1 property is not a JSON array").arg(name);
            return false;
        }
        const QJsonArray shapeRefs = value.toArray();
        for (const QJsonValue &shapeRef: shapeRefs) {
            if (!validateProperty(QLatin1String("ShapeReference"), shapeRef)) {
                qCCritical(lc).noquote() << tr("%1 property has invalid entry").arg(name);
            }
        }
    }

    // ShapeIdStringMap
    else if (name == QLatin1String("rename")) {
        if (!value.isObject()) {
            qCCritical(lc).noquote() << tr("%1 property is not a JSON object").arg(name);
            return false;
        }
        const QJsonObject rename = value.toObject();
        for (auto iter = rename.constBegin(); iter != rename.constEnd(); ++iter) {
            const ShapeId shapeId(iter.key());
            if (!shapeId.isValid()) {
                qCCritical(lc).noquote() << tr("%1 property has invalid shape ID %2")
                                            .arg(name, iter.key());
                return false;
            }
            if (!iter.value().isString()) {
                qCCritical(lc).noquote() << tr("%1 property has shape ID %2 with invalid value")
                                            .arg(name, iter.key());
                return false;
            }
            const QString indentifier = iter.value().toString();
            if (validateIdentifier(indentifier)) {
                qCCritical(lc).noquote() << tr("%1 property has shape ID %2 with invalid identifier %3")
                                            .arg(name, iter.key(), indentifier);
                return false;
            }
        }
    }

    // ShapeReference
    else if ((name == QLatin1String("ShapeReference")) ||
             (name == QLatin1String("create")) ||
             (name == QLatin1String("put")) ||
             (name == QLatin1String("read")) ||
             (name == QLatin1String("update")) ||
             (name == QLatin1String("delete")) ||
             (name == QLatin1String("list")) ||
             (name == QLatin1String("input")) ||
             (name == QLatin1String("output"))) {
        if (!value.isObject()) {
            qCCritical(lc).noquote() << tr("%1 property is not a JSON object").arg(name);
            return false;
        }
        const QJsonObject shapeRef = value.toObject();
        const auto target = shapeRef.constFind(QLatin1String("target")); // Required.
        if (target == shapeRef.constEnd()) {
            qCCritical(lc).noquote() << tr("%1 property has no target property").arg(name);
            return false;
        }
        if (!target.value().isString()) {
            qCCritical(lc).noquote() << tr("%1 property has target value that not a JSON string")
                                        .arg(name);
            return false;
        }
        const QString shapeIdString = target.value().toString();
        const ShapeId shapeId(shapeIdString);
        if (!shapeId.isValid()) {
            qCCritical(lc).noquote() << tr("%1 property has target with invalid shape ID %2")
                                        .arg(name, shapeIdString);
            return false;
        }
        if (shapeId.isRelativeShapeId()) {
            qCCritical(lc).noquote() << tr("%1 property has target with relative shape ID %2")
                                        .arg(name, shapeIdString);
            return false;
        }
    }

    // StringShapeRefMap
    else if (name == QLatin1String("identifiers")) {
        /// \todo Validate StringShapeRefMap
    }

    // StringShapeIdMap
    else if (name == QLatin1String("properties")) {
        /// \todo Validate StringShapeIdMap
    }

    else {
        qCWarning(lc).noquote() << tr("Validation of %1 property not yet implemented").arg(name);
        return false;
    }
    return true;
}

/// \endcond

QTSMITHY_END_NAMESPACE
