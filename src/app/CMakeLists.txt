# SPDX-FileCopyrightText: 2013-2024 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.0)

set(SmithyQtAppSources
  generator.cpp
  generator.h
  renderer.cpp
  renderer.h
)

add_library(SmithyQtApp OBJECT ${SmithyQtAppSources})

target_link_libraries(
  SmithyQtApp
  PRIVATE QtSmithy
  PRIVATE Qt${QT_VERSION_MAJOR}::Core)

add_executable(smithy-qt main.cpp)

  target_link_libraries(smithy-qt PRIVATE SmithyQtApp)
endif()

# \todo Add a custom Info.plist file to the app bundle.

# \todo Include localisations.

target_link_libraries(
  smithy-qt
  PRIVATE QtSmithy
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE Grantlee${QT_VERSION_MAJOR}::Templates)

# \todo linuxdeploy

# \todo windeployqt
