// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the Model class.
 */

#ifndef QTSMITHY_MODEL_H
#define QTSMITHY_MODEL_H

#include <QCoreApplication>
#include <QHash>
#include <QJsonObject>
#include <QObject>

#include "shape.h"
#include "shapeid.h"

QTSMITHY_DECLARE_TEST(Model)

QTSMITHY_BEGIN_NAMESPACE

class ModelPrivate;

class QTSMITHY_EXPORT Model
{
Q_DECLARE_TR_FUNCTIONS(Model);

public:
    Model();
    Model(Model &&other);
    Model(const Model &other);
    Model& operator=(const Model &model);
    Model& operator=(const Model &&model);
    ~Model();

    void clear();
    bool insert(const QJsonObject &ast);
    bool finish();

    bool isValid() const;

    QJsonObject metadata() const;
    QHash<ShapeId, Shape> shapes() const;

protected:
    /// \cond internal
    ModelPrivate * d_ptr; ///< Internal d-pointer.
    /// \endcond

private:
    Q_DECLARE_PRIVATE(Model)
    QTSMITHY_BEFRIEND_TEST(Model)
};

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_MODEL_H
