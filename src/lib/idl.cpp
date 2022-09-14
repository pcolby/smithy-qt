// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Implements the Smithy IDL parsing functions.
 */

#include <qtsmithy/idl.h>

QTSMITHY_BEGIN_NAMESPACE

/*!
 * Returns a JSON AST representation of the Smithy \a idl.
 *
 * Smithy models written using the Smithy IDL have an isomorphic JSON abstract syntax tree (AST)
 * representation that can be used to more easily integrate Smithy into languages and tools.
 *
 * As the rest of the this QtSmithy library uses the JSON AST, this function may be used to convert
 * Smithy IDL content to Smithy JSON AST.
 *
 * Returns a valid (non-empty) QJsonObject if the parsing succeeds. If it fails, the returned object
 * will be empty, and the optional error variable will contain further details about the error.
 *
 * \see https://awslabs.github.io/smithy/2.0/spec/json-ast.html
 */
QJsonObject idlToAst(const QByteArray &idl, IdlParseError *error)
{
    QJsonObject ast{};

    /// \todo Parse the thing.

    Q_UNUSED(idl); /// \todo
    Q_UNUSED(error); /// \todo
    return ast;
}

QTSMITHY_END_NAMESPACE
