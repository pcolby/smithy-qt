<!-- SPDX-FileCopyrightText: 2013-2022 Paul Colby <git@colby.id.au> -->
<!-- SPDX-License-Identifier: LGPL-3.0-or-later -->
# Smithy Qt

[![Build Status](https://github.com/pcolby/smithy-qt/actions/workflows/build.yaml/badge.svg?branch=main)](
  https://github.com/pcolby/smithy-qt/actions/workflows/build.yaml?query=branch%3Amain)
[![Static Analysis](https://github.com/pcolby/smithy-qt/actions/workflows/static.yaml/badge.svg?branch=main)](
  https://github.com/pcolby/smithy-qt/actions/workflows/static.yaml?query=branch%3Amain)
[![Documentation](https://github.com/pcolby/smithy-qt/actions/workflows/docs.yaml/badge.svg?branch=main)](
  https://github.com/pcolby/smithy-qt/actions/workflows/docs.yaml?query=branch%3Amain)

Smithy Qt is (or will be, eventually) two things:

1. `QtSmithy` - a cross-platform naitve [Qt] library for reading [Smithy] models; and
2. `smithy-qt` - a cross-platform CLI tool for rendering text files in response to [Smithy] models.

Smithy Qt is written in Qt, and optimised for generating Qt code (specifically for replacing
[QtAws]'s bespoke code generator), but by using the generic [Grantlee] templating engine, could be
used to generate code (or indeed non-code) for any language or technology.

```mermaid
graph LR
  model(["Smithy Model"]) --QtSmithy--> smithy-qt("smithy-qt")
  templates(["Grantlee<br/>Templates"]) --Grantlee--> smithy-qt
  smithy-qt --Grantlee--> output(["Rendered Files"])
```

```mermaid
graph LR
  model(["Amazon's<br/>AWS Models"]) --> smithy-qt("smithy-qt")
  templates(["qt-aws-sdk<br/>Templates"]) --> smithy-qt
  smithy-qt --> output(["Generated Code"]) --> aws-sdk-qt["AWS SDK for Qt"]
  code(["Handwritten Code"]) --> aws-sdk-qt

  classDef aws-sdk-qt fill:lightgreen,stroke:green
  class templates,code,aws-sdk-qt aws-sdk-qt
  style model fill:orange,stroke:darkorange
```

[CMake]:    https://cmake.org/
[Grantlee]: https://github.com/steveire/grantlee
[Qt]:       https://www.qt.io/
[QtAws]:    https://github.com/pcolby/aws-sdk-qt "AWS SDK for Qt"
[Smithy]:   https://awslabs.github.io/smithy/
