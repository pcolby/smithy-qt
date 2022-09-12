// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defines the SmithyModel and SmithyModelPrivate classes.
 */

#include <qtsmithy/smithymodel.h>
#include "smithymodel_p.h"

QTSMITHY_BEGIN_NAMESPACE

/*!
 * \class SmithyModel
 *
 * The SmithyModel class provides a Qt representation of a Smithy semantic model.
 *
 * \see https://awslabs.github.io/smithy/2.0/spec/model.html#semantic-model
 */

/*!
 * Constructs a new, empty Smithy model.
 */
SmithyModel::SmithyModel() : d_ptr(new SmithyModelPrivate(this))
{
    /// \todo Load the Smithy prelude here?
}

SmithyModel::SmithyModel(SmithyModel &&other) : d_ptr(new SmithyModelPrivate(this))
{
    Q_D(SmithyModel);
    d->metadata = std::move(other.d_ptr->metadata);
    d->nameSpaces = std::move(other.d_ptr->nameSpaces);
    d->shapes = std::move(other.d_ptr->shapes);
}

SmithyModel::SmithyModel(const SmithyModel &other) : d_ptr(new SmithyModelPrivate(this))
{
    Q_D(SmithyModel);
    d->metadata = other.d_ptr->metadata;
    d->nameSpaces = other.d_ptr->nameSpaces;
    d->shapes = other.d_ptr->shapes;
}

SmithyModel& SmithyModel::operator=(const SmithyModel &model)
{
    Q_D(SmithyModel);
    d->metadata = model.d_ptr->metadata;
    d->nameSpaces = model.d_ptr->nameSpaces;
    d->shapes = model.d_ptr->shapes;
    return *this;
}

SmithyModel& SmithyModel::operator=(const SmithyModel &&model)
{
    Q_D(SmithyModel);
    d->metadata = std::move(model.d_ptr->metadata);
    d->nameSpaces = std::move(model.d_ptr->nameSpaces);
    d->shapes = std::move(model.d_ptr->shapes);
    return *this;
}

/*!
 * Destroys this SmithyModel object.
 */
SmithyModel::~SmithyModel()
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
 * \todo Implement static QJsonObject smithy::parseModelFile(const QByteArray &idl,
 *                                                           IdlParseError *error = nullptr);
 *
 * \see https://awslabs.github.io/smithy/2.0/spec/model.html
 */
bool SmithyModel::addModelFile(const QJsonObject &json)
{
    Q_UNUSED(json); /// \todo Implement.
    return false;
}

bool SmithyModel::isValid() const
{
    /// \todo Check for any metadata clashes?
    /// \todo Check for any unknown Shape ID references.
    return false;
}

QJsonObject SmithyModel::metadata() const
{
    Q_D(const SmithyModel);
    return d->metadata;
}

QStringList SmithyModel::nameSpaces() const
{
    Q_D(const SmithyModel);
    return d->nameSpaces;
}

QHash<ShapeId, SmithyModel::Shape> SmithyModel::shapes(const QString &nameSpace) const
{
    Q_D(const SmithyModel);
    Q_UNUSED(nameSpace); ///< \todo
    return d->shapes;
}

/*!
 * \cond internal
 * \class SmithyModelPrivate
 *
 * The SmithyModelPrivate class provides private implementation for SmithyModel.
 */

/*!
 * \internal
 * Constructs a new SmithyModelPrivate object with public implementation \a q.
 */
SmithyModelPrivate::SmithyModelPrivate(SmithyModel * const q) : q_ptr(q)
{

}

/// \endcond

QTSMITHY_END_NAMESPACE
