// SPDX-FileCopyrightText: 2013-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the ModelPrivate class.
 */

#ifndef QTSMITHY_MODEL_P_H
#define QTSMITHY_MODEL_P_H

#include <qtsmithy/model.h>
#include <qtsmithy/shapeid.h>

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QVersionNumber>

QTSMITHY_BEGIN_NAMESPACE

class QTSMITHY_EXPORT ModelPrivate
{
Q_DECLARE_TR_FUNCTIONS(Model);

public:
    explicit ModelPrivate(Model * const q);

    static QJsonObject mergeMetadata(const QMultiHash<QString, QJsonValue> &metadata);
    static QVersionNumber smithyVersion(const QJsonObject &ast);

protected:
    Model * q_ptr; ///< Internal q-pointer.
    static Q_LOGGING_CATEGORY(lc, "smithy.Model", QtInfoMsg); ///< Logging category for Model.

private:
    Model::Error error;
    QMultiHash<QString, QJsonValue> allMetadata;
    QMultiHash<ShapeId, Shape> allShapes;
    QJsonObject mergedMetadata;
    QHash<ShapeId, Shape> mergedShapes;

    Q_DECLARE_PUBLIC(Model)
    QTSMITHY_BEFRIEND_TEST(Model)
};

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_MODEL_P_H
