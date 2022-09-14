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
 * Add the logical content of the JSON AST model file \a json into this semantic model.
 *
 * A Smithy semantic model is split into one or more model files. Use this method to add all model
 * files that comprise this semantic model.
 *
 * Note, Smithy model files may be in JSON AST or Smithy IDL formats. This method expects the
 * former, however, a method will be provided (outside of this class) for converting Smithy IDL
 * model file data to JSON objects.
 *
 * \todo Implement static QJsonObject smithy::smithyIdlToJsonAst(const QByteArray &idl,
 *                                                           IdlParseError *error = nullptr);
 *
 * \see https://awslabs.github.io/smithy/2.0/spec/model.html
 */
bool Model::addModelFile(const QJsonObject &json)
{
    Q_UNUSED(json); /// \todo Implement.
    return false;
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

/// \endcond

QTSMITHY_END_NAMESPACE
