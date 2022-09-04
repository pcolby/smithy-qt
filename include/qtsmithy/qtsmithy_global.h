// SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

/*!
 * \file
 * Global QtSmithy library macros.
 */

#ifndef QTSMITHY_GLOBAL_H
#define QTSMITHY_GLOBAL_H

#include <QtGlobal>

/// \cond internal

/*!
 * QtSmithy library export/import macro.
 */
#if defined(QTSMITHY_SHARED) || !defined(QTSMITHY_STATIC)
#  ifdef QTSMITHY_STATIC
#    error "Both QTSMITHY_SHARED and QTSMITHY_STATIC defined."
#  endif
#  if defined(QTSMITHY_LIBRARY)
#    define QTSMITHY_EXPORT Q_DECL_EXPORT
#  else
#    define QTSMITHY_EXPORT Q_DECL_IMPORT
#  endif
#else
#    define QTSMITHY_EXPORT
#endif

/*!
 * \def QTSMITHY_BEGIN_NAMESPACE
 *
 * Macro for starting the QtSmithy library's top-most namespace (if one is defined).
 *
 * \sa QTSMITHY_END_NAMESPACE
 * \sa QTSMITHY_NAMESPACE
 */

/*!
 * \def QTSMITHY_END_NAMESPACE
 *
 * Macro for ending the QtSmithy library's top-most namespace (if one is defined).
 *
 * \sa QTSMITHY_BEGIN_NAMESPACE
 * \sa QTSMITHY_NAMESPACE
 */

#ifdef QTSMITHY_NAMESPACE
  #define QTSMITHY_BEGIN_NAMESPACE namespace QTSMITHY_NAMESPACE {
  #define QTSMITHY_END_NAMESPACE }
#else
  #define QTSMITHY_BEGIN_NAMESPACE
  #define QTSMITHY_END_NAMESPACE
#endif

/// \endcond

#endif // QTSMITHY_GLOBAL_H
