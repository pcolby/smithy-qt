# SPDX-FileCopyrightText: 2013-2024 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

set(SmithyQtAppSources
  generator.cpp
  generator.h
  renderer.cpp
  renderer.h
  textlee.h
)

add_library(smithy-qt-lib OBJECT ${SmithyQtAppSources})

target_link_libraries(
  smithy-qt-lib
  PRIVATE QtSmithy
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE ${TEXT_TEMPLATE_TARGET})

add_executable(smithy-qt main.cpp)

# \todo Add a custom Info.plist file to the app bundle.

# \todo Include localisations.

target_link_libraries(
  smithy-qt
  PRIVATE smithy-qt-lib
  PRIVATE QtSmithy
  PRIVATE Qt${QT_VERSION_MAJOR}::Core
  PRIVATE ${TEXT_TEMPLATE_TARGET})

find_program(LINUXDEPLOY NAMES linuxdeploy linuxdeploy-x86_64.AppImage)
if (LINUXDEPLOY)
  message(STATUS "Found linuxdeploy: ${LINUXDEPLOY}")
  configure_file(desktop.in smithy-qt.desktop)
  add_custom_target(
    appimage
    COMMAND ${CMAKE_COMMAND} -E env
      OUTPUT=${CMAKE_BINARY_DIR}/smithy-qt-${PROJECT_VERSION}${PROJECT_VERSION_SUFFIX}.AppImage
      QMAKE=$<TARGET_FILE:Qt${QT_VERSION_MAJOR}::qmake>
      --
      ${LINUXDEPLOY}
      --appdir "${CMAKE_BINARY_DIR}/AppDir"
      --desktop-file "${CMAKE_CURRENT_BINARY_DIR}/smithy-qt.desktop"
      --executable "$<TARGET_FILE:smithy-qt>"
      --icon-file "${CMAKE_CURRENT_SOURCE_DIR}/smithy-qt.png"
      --output appimage
      --plugin qt
    COMMENT "Bundling AppImage"
    DEPENDS smithy-qt QtSmithy
    VERBATIM
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

# \todo windeployqt
