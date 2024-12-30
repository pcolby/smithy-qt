<!-- SPDX-FileCopyrightText: 2013-2024 Paul Colby <git@colby.id.au> -->
<!-- SPDX-License-Identifier: LGPL-3.0-or-later -->
# Smithy Qt

[![Build Status](https://github.com/pcolby/smithy-qt/actions/workflows/build.yaml/badge.svg?branch=main)](
  https://github.com/pcolby/smithy-qt/actions/workflows/build.yaml?query=branch%3Amain)
[![Static Analysis](https://github.com/pcolby/smithy-qt/actions/workflows/static.yaml/badge.svg?branch=main)](
  https://github.com/pcolby/smithy-qt/actions/workflows/static.yaml?query=branch%3Amain)
[![Documentation](https://github.com/pcolby/smithy-qt/actions/workflows/docs.yaml/badge.svg?branch=main)](
  https://github.com/pcolby/smithy-qt/actions/workflows/docs.yaml?query=branch%3Amain)
[![Codacy Grade](https://app.codacy.com/project/badge/Grade/ca79f90b9ef249b389c980d93fa25d9d)](
  https://app.codacy.com/gh/pcolby/smithy-qt/dashboard)
[![Codacy Coverage](https://app.codacy.com/project/badge/Coverage/ca79f90b9ef249b389c980d93fa25d9d)](
  https://app.codacy.com/gh/pcolby/smithy-qt/dashboard)

Smithy Qt is (or will be, eventually) two things:

1. `QtSmithy` - a cross-platform naitve [Qt] library for reading [Smithy] models; and
2. `smithy-qt` - a cross-platform CLI tool for rendering text files in response to [Smithy] models.

Smithy Qt is written in Qt, and optimised for generating Qt code (specifically for replacing
[QtAws]'s bespoke code generator), but by using a generic text templating engine, could be
used to generate code (or indeed non-code) for any language or technology.

```mermaid
graph LR
  model(["Smithy Model"]) --QtSmithy--> smithy-qt("smithy-qt")
  templates(["Text<br/>Templates"]) --Template<br/>Engine--> smithy-qt
  smithy-qt --Template<br/>Engine--> output(["Rendered Files"])
```

```mermaid
graph LR
  model(["AWS Models"]) --> smithy-qt("smithy-qt")
  templates(["qt-aws-sdk<br/>Templates"]) --> smithy-qt
  smithy-qt --> output(["Generated Code"]) --> aws-sdk-qt["AWS SDK for Qt"]
  code(["qt-aws-sdk Code"]) --> aws-sdk-qt

  classDef aws-sdk-qt color:#00414A,fill:lightgreen,stroke:#2CDE85
  class templates,code,aws-sdk-qt aws-sdk-qt
  style model color:black,fill:#FF9900,stroke:darkorange

  click aws-sdk-qt https://github.com/pcolby/aws-sdk-qt
  click code https://github.com/pcolby/aws-sdk-qt
  click templates https://github.com/pcolby/aws-sdk-qt
  click model https://github.com/aws/aws-sdk-js-v3/tree/main/codegen/sdk-codegen/aws-models
```

### Text Templates

Smithy Qt uses a text template language that is very similar to [Django templates]. Specfically, `smithy-qt` uses either
the [Grantlee] or [Cutelee] library (the latter is a Qt6-compatible fork of the former).

> [!NOTE]
> Smithy Qt began using the [Grantlee] library with Qt5. However, the [Grantlee] author has moved the library to [KDE],
> where it is now known as [KTextTemplate], and the original [Grantlee] library has been retired. Meanwhile, the
> [Cutelyst] project has forked [Grantlee] to [Cutelee], and ported it to Qt6. Today, Smithy Qt supports both [Grantlee]
> (with Qt5) and [Cutelee] (with Qt6). Smithy Qt will most likely support [KTextTemplate] too, when that library becomes
> more readily available.

You can find the template language documention [here](https://pcolby.github.io/cutelee/for_themers.html). You can also
have a look at some of the [demo](demo) templates.

[CMake]:            https://cmake.org/ "CMake"
[Cutelee]:          https://github.com/cutelyst/cutelee "The Cutelee Libraries"
[Cutelyst]:         https://cutelyst.org/ "Cutelyst! The Qt Web Framework"
[Django templates]: https://docs.djangoproject.com/en/5.0/ref/templates/ "Django Templates"
[Grantlee]:         https://github.com/steveire/grantlee "The Grantlee Libraries"
[KDE]:              https://kde.org/ "KDE Community"
[KTextTemplate]:    https://invent.kde.org/frameworks/ktexttemplate "The KTextTemplate Library"
[Qt]:               https://www.qt.io/ "Qt cross-platform application development framework"
[QtAws]:            https://github.com/pcolby/aws-sdk-qt "AWS SDK for Qt"
[Smithy]:           https://smithy.io/ "Smithy language for defining services and SDKs"
