// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the Model class.
 */

#ifndef QTSMITHY_MODEL_H
#define QTSMITHY_MODEL_H

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
public:
    Model();
    Model(Model &&other);
    Model(const Model &other);
    Model& operator=(const Model &model);
    Model& operator=(const Model &&model);
    ~Model();

    bool addModelFile(const QJsonObject &json);

    bool isValid() const;

    QJsonObject /*or QMultiHash<QString, QJsonValue>*/ metadata() const;
    QStringList nameSpaces() const;
    QHash<ShapeId, Shape> shapes(const QString &nameSpace = QString()) const;

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