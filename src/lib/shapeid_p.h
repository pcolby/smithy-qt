// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the ShapeIdPrivate class.
 */

#ifndef QTSMITHY_SHAPEID_P_H
#define QTSMITHY_SHAPEID_P_H

#include <qtsmithy/shapeid.h>

#include <QCoreApplication>
#include <QLoggingCategory>

QTSMITHY_BEGIN_NAMESPACE

class QTSMITHY_EXPORT ShapeIdPrivate
{
Q_DECLARE_TR_FUNCTIONS(ShapeId);

public:
    explicit ShapeIdPrivate(ShapeId * const q);

    void setShapeId(const QString &shapeId);

protected:
    ShapeId * q_ptr; ///< Internal q-pointer.
    static Q_LOGGING_CATEGORY(lc, "smithy.ShapeId", QtInfoMsg); ///< Logging category for UI commands.

private:
    QString memberName;
    QString nameSpace;
    QString shapeName;

    Q_DECLARE_PUBLIC(ShapeId)
    QTSMITHY_BEFRIEND_TEST(ShapeId)
};

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_SHAPEID_P_H
