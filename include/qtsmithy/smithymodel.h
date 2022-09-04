// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the SmithyModel class.
 */

#ifndef QTSMITHY_SMITHYMODEL_H
#define QTSMITHY_SMITHYMODEL_H

#include "qtsmithy_global.h"

#include <QObject>

QTSMITHY_BEGIN_NAMESPACE

class SmithyModelPrivate;

class QTSMITHY_EXPORT SmithyModel : public QObject
{
    Q_OBJECT

public:
    SmithyModel(QObject * parent = nullptr);
    ~SmithyModel() override;

signals:

protected:
    /// \cond internal
    SmithyModelPrivate * d_ptr; ///< Internal d-pointer.
    SmithyModel(SmithyModelPrivate * const d, QObject * const parent);
    /// \endcond

private:
    Q_DECLARE_PRIVATE(SmithyModel)
    Q_DISABLE_COPY(SmithyModel)
    friend class TestSmithyModel;
};

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_SMITHYMODEL_H
