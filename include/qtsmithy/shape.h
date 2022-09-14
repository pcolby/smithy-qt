// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the Shape class.
 */

#ifndef QTSMITHY_SHAPE_H
#define QTSMITHY_SHAPE_H

#include <QHash>
#include <QJsonObject>
#include <QObject>

#include "shapeid.h"

QTSMITHY_DECLARE_TEST(Shape)

QTSMITHY_BEGIN_NAMESPACE

class ShapePrivate;

class QTSMITHY_EXPORT Shape
{
public:
    Shape();
    Shape(Shape &&other);
    Shape(const Shape &other);
    Shape& operator=(const Shape &shape);
    Shape& operator=(const Shape &&shape);
    ~Shape();

    bool isValid() const;

protected:
    /// \cond internal
    ShapePrivate * d_ptr; ///< Internal d-pointer.
    /// \endcond

private:
    Q_DECLARE_PRIVATE(Shape)
    QTSMITHY_BEFRIEND_TEST(Shape)
};

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_SHAPE_H
