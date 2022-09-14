// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Declares the Smithy IDL parsing functions.
 */

#ifndef QTSMITHY_IDL_H
#define QTSMITHY_IDL_H

#include "qtsmithy_global.h"

#include <QJsonObject>

QTSMITHY_DECLARE_TEST(Idl)

QTSMITHY_BEGIN_NAMESPACE

struct QTSMITHY_EXPORT IdlParseError
{
    enum class ParseError {
        NoError = 0,
    } error;

    QString errorString();
};

QTSMITHY_EXPORT QJsonObject idlToAst(const QByteArray &idl, IdlParseError *error = nullptr);

QTSMITHY_END_NAMESPACE

#endif // QTSMITHY_IDL_H
