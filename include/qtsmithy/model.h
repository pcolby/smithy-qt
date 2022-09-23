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
    enum class Error {
        NoError             = 0,
        NoData              = 1,
        InvalidMetadata     = 2,
        InvalidShapes       = 3,
        InvalidShapeId      = 4,
        InvalidShape        = 5,
        ConflictingMetadata = 6,
    };

    Model();
    Model(Model &&other);
    Model(const Model &other);
    Model& operator=(const Model &model);
    Model& operator=(const Model &&model);
    ~Model();

    void clear();
    bool insert(const QJsonObject &ast);
    bool finish();

    Error error() const;
    bool isValid() const;

    QJsonObject metadata() const;
    Shape shape(const ShapeId &shapeId) const;
    QHash<ShapeId, Shape> shapes(const Shape::Type &type = Shape::Type::Undefined) const;

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
