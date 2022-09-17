// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defines the Model and ModelPrivate classes.
 */

#include <qtsmithy/model.h>
#include "model_p.h"

#include <QJsonArray>

QTSMITHY_BEGIN_NAMESPACE

/*!
 * \class Model
 *
 * The Model class provides a Qt representation of a Smithy semantic model.
 *
 * \see https://awslabs.github.io/smithy/2.0/spec/model.html#semantic-model
 */

/*!
 * Constructs a new, empty Smithy model.
 */
Model::Model() : d_ptr(new ModelPrivate(this))
{
    /// \todo Load the Smithy prelude here?
}

Model::Model(Model &&other) : d_ptr(new ModelPrivate(this))
{
    Q_D(Model);
    d->metadata = std::move(other.d_ptr->metadata);
    d->nameSpaces = std::move(other.d_ptr->nameSpaces);
    d->shapes = std::move(other.d_ptr->shapes);
}

Model::Model(const Model &other) : d_ptr(new ModelPrivate(this))
{
    Q_D(Model);
    d->metadata = other.d_ptr->metadata;
    d->nameSpaces = other.d_ptr->nameSpaces;
    d->shapes = other.d_ptr->shapes;
}

Model& Model::operator=(const Model &model)
{
    Q_D(Model);
    d->metadata = model.d_ptr->metadata;
    d->nameSpaces = model.d_ptr->nameSpaces;
    d->shapes = model.d_ptr->shapes;
    return *this;
}

Model& Model::operator=(const Model &&model)
{
    Q_D(Model);
    d->metadata = std::move(model.d_ptr->metadata);
    d->nameSpaces = std::move(model.d_ptr->nameSpaces);
    d->shapes = std::move(model.d_ptr->shapes);
    return *this;
}

/*!
 * Destroys this Model object.
 */
Model::~Model()
{

}

/*!
 * Add the logical content of the JSON AST model file given by \a ast into this semantic model.
 *
 * A Smithy semantic model is split into one or more model files. Use this method to add all model
 * files that comprise this semantic model.
 *
 * \see https://awslabs.github.io/smithy/2.0/spec/model.html
 */
bool Model::insert(const QJsonObject &ast)
{
    Q_D(Model);

    // Fetch the Smithy version.
    const QVersionNumber version = d->smithyVersion(ast);
    if (version.majorVersion() > 2) {
        qCWarning(d->lc).noquote() << tr("Unknown Smithy version %1").arg(version.toString());
    }

    // Warn on any unrecognised top-level Smithy AST properties.
    // https://awslabs.github.io/smithy/2.0/spec/json-ast.html#top-level-properties
    const QStringList keys = ast.keys();
    for (const QString &key: keys) {
        const QStringList knownKeys{
            QStringLiteral("smithy"),QStringLiteral("metadata"), QStringLiteral("shapes") };
        if (!knownKeys.contains(key)) {
            qCWarning(d->lc).noquote() << tr("Ignoring unknown Smithy AST property %1").arg(key);
        }
    }

    // Process the (optional) metadata.
    const QJsonValue metadata = ast.value(QStringLiteral("metadata"));
    if (metadata != QJsonValue::Undefined) {
        if (!metadata.isObject()) {
            qCCritical(d->lc).noquote() << tr("Smithy AST metadata is not an object");
            qDebug().noquote() << metadata;
            return false;
        }
        const QJsonObject object = metadata.toObject();
        qCDebug(d->lc).noquote() << tr("Processing %n metadata entry(s)", nullptr, object.length());
        for (auto iter = object.constBegin(); iter != object.constEnd(); ++iter) {
            d->metadata.insert(iter.key(), iter.value());
        }
    }

    // Process the (optional) shapes.
    const QJsonValue shapes = ast.value(QStringLiteral("shapes"));
    if (shapes != QJsonValue::Undefined) {
        if (!shapes.isObject()) {
            qCCritical(d->lc).noquote() << tr("Smithy AST shapes is not an object");
            return false;
        }
        const QJsonObject object = shapes.toObject();
        qCDebug(d->lc).noquote() << tr("Processing %n shape(s)", nullptr, object.length());
        for (auto iter = object.constBegin(); iter != object.constEnd(); ++iter) {
            const ShapeId shapeId(iter.key());
            if (!shapeId.isValid()) {
                qCCritical(d->lc).noquote() << tr("Failed to parse shape ID %1").arg(iter.key());
                return false;
            }
            if (!shapeId.hasNameSpace()) {
                qCCritical(d->lc).noquote() << tr("Shape ID %1 has no namespace").arg(iter.key());
                return false;
            }
            if (!iter.value().isObject()) {
                qCCritical(d->lc).noquote() << tr("Shape %1 is not a JSON object").arg(iter.key());
                return false;
            }
            const Shape shape{iter.value().toObject()};
            if (!shape.isValid()) {
                qCCritical(d->lc).noquote() << tr("Failed to process shape %1").arg(iter.key());
                return false;
            }
            d->shapes.insert(iter.key(), shape);
        }
    }
    return true;
}

bool Model::isValid() const
{
    Q_D(const Model);
    if ((!d->metadata.isEmpty()) && (ModelPrivate::mergeMetadata(d->metadata).isEmpty())) {
        return false; // Metadata is invalid.
    }
    /// \todo Check for any unknown Shape ID references.
    return false;
}

QJsonObject Model::metadata() const
{
    Q_D(const Model);
    return ModelPrivate::mergeMetadata(d->metadata);
}

QStringList Model::nameSpaces() const
{
    Q_D(const Model);
    return d->nameSpaces;
}

QHash<ShapeId, Shape> Model::shapes(const QString &nameSpace) const
{
    Q_D(const Model);
    Q_UNUSED(nameSpace); ///< \todo
    return d->shapes;
}

/*!
 * \cond internal
 * \class ModelPrivate
 *
 * The ModelPrivate class provides private implementation for Model.
 */

/*!
 * \internal
 * Constructs a new ModelPrivate object with public implementation \a q.
 */
ModelPrivate::ModelPrivate(Model * const q) : q_ptr(q)
{

}

// https://awslabs.github.io/smithy/2.0/spec/model.html#merging-metadata
QJsonObject ModelPrivate::mergeMetadata(const QMultiHash<QString, QJsonValue> &metadata)
{
    qCDebug(lc).noquote() << tr("Merging %n metedata entry(s)", nullptr, metadata.size());
    QJsonObject merged;
    const QStringList keys = metadata.keys();
    for (const QString &key: keys) {
        // If values are arrays, concatenate them.
        const auto values = metadata.values(key);
        if (values.first().isArray()) {
            QJsonArray concatenatedArray;
            for (const QJsonValue &value: values) {
                if (!value.isArray()) {
                    qCCritical(lc).noquote() << tr("Metadata %1 has conflicting types").arg(key);
                    return QJsonObject{};
                }
                const QJsonArray thisArray = value.toArray();
                for (const QJsonValue &item: thisArray) {
                    concatenatedArray.append(item);
                }
            }
            merged.insert(key, concatenatedArray);
            continue;
        }

        // Otherwise all values must be identical.
        for (const QJsonValue &value: values) {
            qDebug() << values.first() << value;
            if (value != values.first()) {
                qCDebug(lc).noquote() << tr("Metatadata %1 has conflicting values").arg(key);
                return QJsonObject{};
            }
        }
        merged.insert(key, values.first());
    }
    qCDebug(lc).noquote() << tr("Merged %n metedata entry(s) to %1", nullptr, metadata.size())
                             .arg(merged.size());
    return merged;
}

QVersionNumber ModelPrivate::smithyVersion(const QJsonObject &ast)
{
    const QString versionString = ast.value(QLatin1String("smithy")).toString();
    qCDebug(lc).noquote() << tr("Smithy version string:") << versionString;
    int suffixIndex = -1; // Initial value is ignored.
    const QVersionNumber versionNumber = QVersionNumber::fromString(versionString, &suffixIndex);
    qCDebug(lc).noquote() << tr("Smithy version number:") << versionNumber;
    if (versionNumber.isNull()) {
        qCWarning(lc).noquote() << tr("Failed to parse Smithy version \"%1\"").arg(versionString);
    } else if (suffixIndex < versionString.length()) {
        qCWarning(lc).noquote() << tr("Ignoring Smithy version suffix \"%1\"")
                                   .arg(versionString.mid(suffixIndex));
    }
    return versionNumber;
}

/// \endcond

QTSMITHY_END_NAMESPACE
