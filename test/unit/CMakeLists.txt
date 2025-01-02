# SPDX-FileCopyrightText: 2013-2025 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)

function(add_smithy_qt_unit_test name)
  add_executable(test${name} ${ARGN})

  target_include_directories(test${name} PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)

  target_link_libraries(
    test${name}
    PRIVATE QtSmithy
    PRIVATE Qt${QT_VERSION_MAJOR}::Test)

if(${QT_VERSION} VERSION_LESS "5.12")
  add_test(NAME ${name} COMMAND test${name})
else() # Support for TAP format added to Qt Test in 5.12.
  add_test(NAME ${name} COMMAND test${name} -o ${name}.tap,tap -o -,txt)
endif()
  set_tests_properties(${name} PROPERTIES LABELS "lib;unit")

  if (ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang$")
    # Tell LLVM's instrumentation hook where to write the profile data when the test is executed.
    set_tests_properties(${name} PROPERTIES ENVIRONMENT "LLVM_PROFILE_FILE=${name}.profraw")
    # Add the full path to the profile data files list for the llvm-profdata tool to merge in the 'coverage' target.
    file(REAL_PATH "${name}.profraw" LLVM_PROFILE_FILE BASE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    list(APPEND LLVM_PROFILE_FILES "${LLVM_PROFILE_FILE}")
    set(LLVM_PROFILE_FILES "${LLVM_PROFILE_FILES}" CACHE INTERNAL "Internal")
  endif()
endfunction()

add_subdirectory(lib)

if(NOT TEXT_TEMPLATE_LIBRARY STREQUAL None)
  add_subdirectory(app)
endif()
