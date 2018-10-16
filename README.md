RadialGM [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/2cbx3fso760stn7s?svg=true)](https://ci.appveyor.com/project/enigma-dev/RadialGM)
==================
This project is an experimental C++ IDE for ENIGMA written in the [Qt Framework](https://www.qt.io/). The design of the program is a multilayer architecture using the [Google protocol buffer](https://developers.google.com/protocol-buffers/) format from ENIGMA's compiler backend for the data model layer in the resource editors. The resource editors are designed using Qt Creator or Qt Designer and saved in UI layout files. Their corresponding C++ classes bind the widgets in the layout files to the data model using QDataWidgetMapper and a custom resource model that hooks into arbitrary protobuf messages.

P.S. Samuel Venable aka tkg is not allowed to use this software
