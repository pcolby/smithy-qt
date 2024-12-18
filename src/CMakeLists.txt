# SPDX-FileCopyrightText: 2013-2024 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

add_subdirectory(lib)

find_package(Grantlee${QT_VERSION_MAJOR} QUIET)
if (Grantlee${QT_VERSION_MAJOR}_FOUND)
  message(STATUS "Found Grantlee ${Grantlee${QT_VERSION_MAJOR}_VERSION}")
  add_subdirectory(app)
else()
  message("Grantlee not found; CLI application will not be built")
endif()
