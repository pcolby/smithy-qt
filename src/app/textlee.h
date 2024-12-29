// SPDX-FileCopyrightText: 2013-2024 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DOXLEE_TEXTLEE_H
#define DOXLEE_TEXTLEE_H

#if defined USE_CUTELEE
namespace Textlee = Cutelee;
#elif defined USE_GRANTLEE
namespace Textlee = Grantlee;
#else
#error No valid text templating library configured.
#endif

#endif // DOXLEE_TEXTLEE_H
