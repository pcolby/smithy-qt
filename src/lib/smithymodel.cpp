// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Defines the SmithyModel and SmithyModelPrivate classes.
 */

#include <qtsmithy/smithymodel.h>
#include "smithymodel_p.h"

/*!
 * \class SmithyModel
 *
 * The SmithyModel class provides a Qt representation of a Smithy semantic model.
 *
 * \see https://awslabs.github.io/smithy/2.0/spec/model.html#semantic-model
 */

/*!
 * Constructs a new Smithy model with \a parent.
 */
SmithyModel::SmithyModel(QObject * parent) : QObject(parent)
{

}

/*!
 * \cond internal
 * Constructs a new Smithy model with \a parent, and private implementation \a d.
 */
SmithyModel::SmithyModel(SmithyModelPrivate * const d, QObject * const parent)
    : QObject(parent), d_ptr(d)
{

}
/// \endcond

/*!
 * Destroys this SmithyModel object.
 */
SmithyModel::~SmithyModel()
{

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
