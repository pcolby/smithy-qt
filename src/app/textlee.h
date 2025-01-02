// SPDX-FileCopyrightText: 2013-2025 Paul Colby <git@colby.id.au>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef QTSMITHY_TEXTLEE_H
#define QTSMITHY_TEXTLEE_H

#if defined USE_CUTELEE
namespace Textlee = Cutelee;
#elif defined USE_GRANTLEE
namespace Textlee = Grantlee;
#elif defined USE_KTEXTTEMPLATE
namespace Textlee = KTextTemplate;
#else
#error No valid text templating library configured.
#endif

#if defined USE_CUTELEE + defined USE_GRANTLEE + defined USE_KTEXTTEMPLATE > 1
#error More than one text templating library macro defined.
#endif

#endif // QTSMITHY_TEXTLEE_H
