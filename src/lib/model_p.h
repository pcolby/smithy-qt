// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the ModelPrivate class.
 */

#ifndef QTSMITHY_MODEL_P_H
#define QTSMITHY_MODEL_P_H

#include <qtsmithy/model.h>
#include <qtsmithy/shapeid.h>

QTSMITHY_BEGIN_NAMESPACE

class QTSMITHY_EXPORT ModelPrivate
{
public:
    explicit ModelPrivate(Model * const q);

protected:
    Model * q_ptr; ///< Internal q-pointer.

private:
    QJsonObject metadata;
    QStringList nameSpaces;
    QHash<ShapeId, Model::Shape> shapes;

    Q_DECLARE_PUBLIC(Model)
    QTSMITHY_BEFRIEND_TEST(Model)
};

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_MODEL_P_H
