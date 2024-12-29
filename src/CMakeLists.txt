# SPDX-FileCopyrightText: 2013-2024 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

add_subdirectory(lib)

if(NOT TEXT_TEMPLATE_LIBRARY STREQUAL None)
  add_subdirectory(app)
endif()
