// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the Idl class.
 */

#ifndef QTSMITHY_IDL_H
#define QTSMITHY_IDL_H

#include "qtsmithy_global.h"

#include <QJsonObject>

QTSMITHY_DECLARE_TEST(Idl)

QTSMITHY_BEGIN_NAMESPACE

class IdlParseError
{

};

QJsonObject smithyIdlToJsonAst(const QByteArray &idl, IdlParseError *error = nullptr);

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_IDL_H
