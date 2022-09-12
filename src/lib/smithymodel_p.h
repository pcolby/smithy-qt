// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the SmithyModelPrivate class.
 */

#ifndef QTSMITHY_SMITHYMODEL_P_H
#define QTSMITHY_SMITHYMODEL_P_H

#include <qtsmithy/smithymodel.h>
#include <qtsmithy/shapeid.h>

QTSMITHY_BEGIN_NAMESPACE

class QTSMITHY_EXPORT SmithyModelPrivate
{
public:
    explicit SmithyModelPrivate(SmithyModel * const q);

protected:
    SmithyModel * q_ptr; ///< Internal q-pointer.

private:
    QJsonObject metadata;
    QStringList nameSpaces;
    QHash<ShapeId, SmithyModel::Shape> shapes;

    Q_DECLARE_PUBLIC(SmithyModel)
    QTSMITHY_BEFRIEND_TEST(SmithyModel)
};

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_SMITHYMODEL_P_H
