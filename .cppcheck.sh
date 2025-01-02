#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2013-2025 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

cppcheck --version
RC=0
for platform in native unix{32,64} win{32{A,W},64} unspecified; do
  for defines in -{D,U}QTSMITHY_LIBRARY' '-{DQTSMITHY_NAMESPACE=smithy,UQTSMITHY_NAMESPACE}' '-{DPROJECT_BUILD_ID=\"1234\",UPROJECT_BUILD_ID}' '-{DPROJECT_PRE_RELEASE=pre,UPROJECT_PRE_RELEASE}; do
    echo "$platform $defines"
    read -a definesArray -r <<< "$defines"
    cppcheck -UQ_DECLARE_PRIVATE "${definesArray[@]}" \
      --enable=all --error-exitcode=1 -Isrc \
      --language=c++ --platform="$platform" --std=c++{17,20} \
      --suppress={missingInclude,missingIncludeSystem,unknownMacro} \
      --quiet "$@" src test | grep -Eve '^Checking' | uniq --skip-fields=1
    RC=$((RC+$?))
  done
done
exit $RC
