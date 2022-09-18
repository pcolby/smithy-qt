// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the ShapePrivate class.
 */

#ifndef QTSMITHY_SHAPE_P_H
#define QTSMITHY_SHAPE_P_H

#include <qtsmithy/shape.h>
#include <qtsmithy/shapeid.h>

#include <QLoggingCategory>

QTSMITHY_BEGIN_NAMESPACE

class QTSMITHY_EXPORT ShapePrivate
{
Q_DECLARE_TR_FUNCTIONS(Shape);

public:
    explicit ShapePrivate(Shape * const q);

    static Shape::Type getType(const QJsonObject &ast);
    static Shape::Type getType(const QString &type);

protected:
    Shape * q_ptr; ///< Internal q-pointer.
    static Q_LOGGING_CATEGORY(lc, "smithy.Shape", QtInfoMsg); ///< Logging category for UI commands.

private:
    ShapeId id;
    Shape::Type type;

    Q_DECLARE_PUBLIC(Shape)
    QTSMITHY_BEFRIEND_TEST(Shape)
};

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_SHAPE_P_H
