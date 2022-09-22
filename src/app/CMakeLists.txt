# SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.0)

set(SmithyQtAppSources
  generator.cpp
  generator.h
  renderer.cpp
  renderer.h
)

if(${CMAKE_VERSION} VERSION_LESS "3.12.0")
  add_executable(smithy-qt main.cpp ${SmithyQtAppSources})
else()
  # We put all but main.cpp into a shared 'object' library so
  # our app unit tests can link against the same binary objects.
  add_library(SmithyQtApp OBJECT ${SmithyQtAppSources})

  target_link_libraries(
    SmithyQtApp
    PRIVATE QtSmithy
    PRIVATE Qt${QT_VERSION_MAJOR}::Core)

  add_executable(smithy-qt main.cpp)

  target_link_libraries(smithy-qt PRIVATE SmithyQtApp)
endif()

target_link_libraries(
  smithy-qt
  PRIVATE QtSmithy
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Grantlee${QT_VERSION_MAJOR}::Templates)
