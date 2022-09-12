// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the SmithyModel class.
 */

#ifndef QTSMITHY_SMITHYMODEL_H
#define QTSMITHY_SMITHYMODEL_H

#include <QHash>
#include <QJsonObject>
#include <QObject>

#include "shapeid.h"

QTSMITHY_DECLARE_TEST(SmithyModel)

QTSMITHY_BEGIN_NAMESPACE

class SmithyModelPrivate;

class QTSMITHY_EXPORT SmithyModel
{
public:
    typedef int Shape; ///< Just temporary, until we define the real (class) type.

    SmithyModel();
    SmithyModel(SmithyModel &&other);
    SmithyModel(const SmithyModel &other);
    SmithyModel& operator=(const SmithyModel &model);
    SmithyModel& operator=(const SmithyModel &&model);
    ~SmithyModel();

    bool addModelFile(const QJsonObject &json);

    bool isValid() const;

    QJsonObject /*or QMultiHash<QString, QJsonValue>*/ metadata() const;
    QStringList nameSpaces() const;
    QHash<ShapeId, Shape> shapes(const QString &nameSpace = QString()) const;

protected:
    /// \cond internal
    SmithyModelPrivate * d_ptr; ///< Internal d-pointer.
    /// \endcond

private:
    Q_DECLARE_PRIVATE(SmithyModel)
    QTSMITHY_BEFRIEND_TEST(SmithyModel)
};

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_SMITHYMODEL_H
