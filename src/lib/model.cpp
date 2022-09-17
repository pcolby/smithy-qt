// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defines the Model and ModelPrivate classes.
 */

#include <qtsmithy/model.h>
#include "model_p.h"

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
 * If \a name is non-empty, it will be used in diagnostic logging, but it otherwise opaque to this
 * function.
 *
 * \see https://awslabs.github.io/smithy/2.0/spec/model.html
 */
bool Model::insert(const QJsonObject &ast, const QString &name)
{
    Q_UNUSED(name); ///< \todo

    // Fetch the Smithy version.
    Q_D(const Model);
    const QVersionNumber version = d->smithyVersion(ast);
    if (version.majorVersion() > 2) {
        qCWarning(d->lc) << "Unknown Smithy version:" << version;
    }

    // Warn on any unrecognised top-level Smithy AST properties.
    // https://awslabs.github.io/smithy/2.0/spec/json-ast.html#top-level-properties
    const QStringList keys = ast.keys();
    for (const QString &key: keys) {
        const QStringList knownKeys{
            QStringLiteral("smithy"),QStringLiteral("metadata"), QStringLiteral("shapes") };
        if (!knownKeys.contains(key)) {
            qCWarning(d->lc) << "Ignoring unknown Smithy AST key:" << key;
        }
    }

    // Process the (optional) metadata.
    const QJsonValue metadata = ast.value(QStringLiteral("metadata"));
    if (metadata != QJsonValue::Undefined) {
        if (!metadata.isObject()) {
            qCCritical(d->lc) << "Smithy metadata has invalid type" << metadata;
            return false;
        }
        qCDebug(d->lc) << "Processing" << metadata.toObject().length() << "metadata entries";
        /// \todo
    }

    // Process the (optional) shapes.
    const QJsonValue shapes = ast.value(QStringLiteral("shapes"));
    if (shapes != QJsonValue::Undefined) {
        if (!shapes.isObject()) {
            qCCritical(d->lc) << "Smithy shapes has invalid type";
            return false;
        }
        qCDebug(d->lc) << "Processing" << shapes.toObject().length() << "shape/s";
        /// \todo
    }
    return true;
}

bool Model::isValid() const
{
    /// \todo Check for any metadata clashes?
    /// \todo Check for any unknown Shape ID references.
    return false;
}

QJsonObject Model::metadata() const
{
    Q_D(const Model);
    return d->metadata;
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

QVersionNumber ModelPrivate::smithyVersion(const QJsonObject &ast)
{
    const QString versionString = ast.value(QLatin1String("smithy")).toString();
    qCDebug(lc) << "Smithy version string:" << versionString;
    int suffixIndex = -1; // Initial value is ignored.
    const QVersionNumber versionNumber = QVersionNumber::fromString(versionString, &suffixIndex);
    qCDebug(lc) << "Smithy version number:" << versionNumber;
    if (versionNumber.isNull()) {
        qCWarning(lc) << "Failed to parse Smithy version:" << versionString;
    } else if (suffixIndex < versionString.length()) {
        qCWarning(lc) << "Ignoring Smithy version suffix:" << versionString.mid(suffixIndex);
    }
    return versionNumber;
}

/// \endcond

QTSMITHY_END_NAMESPACE
