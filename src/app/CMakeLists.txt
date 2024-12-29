# SPDX-FileCopyrightText: 2013-2024 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

set(SmithyQtAppSources
  generator.cpp
  generator.h
  renderer.cpp
  renderer.h
)

add_library(smithy-qt-lib OBJECT ${SmithyQtAppSources})

target_link_libraries(
  smithy-qt-lib
  PRIVATE QtSmithy
  PRIVATE Qt${QT_VERSION_MAJOR}::Core)

add_executable(smithy-qt main.cpp)

# \todo Add a custom Info.plist file to the app bundle.

# \todo Include localisations.

target_link_libraries(
  smithy-qt
  PRIVATE smithy-qt-lib
  PRIVATE QtSmithy
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE ${TEXT_TEMPLATE_TARGET})

# \todo linuxdeploy

# \todo windeployqt
