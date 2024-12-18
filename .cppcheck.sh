#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2013-2024 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

cppcheck --version
for platform in native unix32 unix64 win32A win32W win64 unspecified; do
  for defines in -{D,U}QTSMITHY_LIBRARY\ -{DPROJECT_PRE_RELEASE=pre,UPROJECT_PRE_RELEASE}; do
    echo "$platform $defines"
    cppcheck $defines --enable=all --error-exitcode=1 --force --inline-suppr \
      --language=c++ --platform=$platform --std=c++{03,11,14,17,20} \
      --suppress={missingInclude,unknownMacro}  \
      --suppress=unusedFunction:test/unit/*.cpp \
      --quiet "$@" src test
  done
done
