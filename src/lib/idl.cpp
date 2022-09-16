// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Implements the Smithy IDL parsing functions.
 */

#include <qtsmithy/idl.h>

QTSMITHY_BEGIN_NAMESPACE

// https://awslabs.github.io/smithy/1.0/spec/core/idl.html#grammar-token-smithy-Comment
inline bool isComment(const QString &line) {
    return line.startsWith(QStringLiteral("//"));
}

// https://awslabs.github.io/smithy/1.0/spec/core/idl.html#grammar-token-smithy-ControlStatement
inline bool isControlStatement(const QString &line) {
    return line.startsWith(QLatin1Char('$'));
}

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

    const QStringList lines = QString::fromUtf8(idl).split(QLatin1Char('\n'));

    enum {
        ControlSection = 1,  // Always comes first (may be empty).
        MetadataSection = 2, // Always comes second (may be empty).
        ShapeSection = 3,    // Always comes last, must at least contain a namespace.
    } section = ControlSection;

    // Parse the Control section.
    int lineNumber = 0;
    for (const QString &line: lines) {
        ++lineNumber;

        // Recognise when its time to move on the next section.
        switch (section) {
        case ControlSection:
            if (line.startsWith(QLatin1String("metadata"))) {
                section = MetadataSection;
            } else if (line.startsWith(QLatin1String("namespace"))) {
                section = ShapeSection;
            }
            break;
        case MetadataSection:
            if (line.startsWith(QLatin1String("namespace"))) {
                section = ShapeSection;
            }
            break;
        case ShapeSection:
            break;
        }

        // Parse the Control section.
        // https://awslabs.github.io/smithy/2.0/spec/idl.html#grammar-token-smithy-ControlSection
        if (section == ControlSection) {
            // Ignore blank lines, and comments.
            if ((line.trimmed().isEmpty()) || (line.startsWith(QLatin1String("//")))) continue;

            // Control statements must begin with '$'.
            if (!line.startsWith(QLatin1Char('$'))) {
                qWarning() << "InvalidControlStatement:" << line;
                return QJsonObject{};
            }

            // Control statements may use quoted-text for identiifers.
            if (!line.startsWith(QStringLiteral("$\""))) {
                static QRegularExpression regex(QLatin1String(""));
                const QRegularExpressionMatch match = regex.match(line);
            }


        }

        // Parse the Metadata section.
        // https://awslabs.github.io/smithy/2.0/spec/idl.html#grammar-token-smithy-MetadataSection

        // Parse the Shape section.
        // https://awslabs.github.io/smithy/2.0/spec/idl.html#grammar-token-smithy-ShapeSection




    }

//    int index=0;
//    for (; isComment(lines[index]) || isControlStatement(lines[index]); ++index) {


//    }

    /// \todo Parse the thing.

    Q_UNUSED(idl); /// \todo
    Q_UNUSED(error); /// \todo
    return ast;
}

QTSMITHY_END_NAMESPACE
