# SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

cmake_minimum_required(VERSION 3.0)

add_library(
  QtSmithy SHARED
  ${CMAKE_SOURCE_DIR}/include/qtsmithy/model.h
  ${CMAKE_SOURCE_DIR}/include/qtsmithy/shape.h
  ${CMAKE_SOURCE_DIR}/include/qtsmithy/shapeid.h
  ${CMAKE_SOURCE_DIR}/include/qtsmithy/qtsmithy_global.h
  model.cpp
  model_p.h
  shape.cpp
  shape_p.h
  shapeid.cpp
  shapeid_p.h
)

target_include_directories(QtSmithy PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(
  QtSmithy
  PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(QtSmithy PRIVATE QTSMITHY_LIBRARY)
