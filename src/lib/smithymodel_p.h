// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the SmithyModelPrivate class.
 */

#ifndef QTSMITHY_SMITHYMODEL_P_H
#define QTSMITHY_SMITHYMODEL_P_H

#include <qtsmithy/smithymodel.h>

QTSMITHY_BEGIN_NAMESPACE

class QTSMITHY_EXPORT SmithyModelPrivate : public QObject
{
    Q_OBJECT

public:
    explicit SmithyModelPrivate(SmithyModel * const q);

protected:
    SmithyModel * q_ptr; ///< Internal q-pointer.

private:
    Q_DECLARE_PUBLIC(SmithyModel)
    Q_DISABLE_COPY(SmithyModelPrivate)
    friend class TestSmithyModel;
};

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_SMITHYMODEL_P_H
