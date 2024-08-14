RadialGM [![Build Status](https://dev.azure.com/enigma-dev/RadialGM/_apis/build/status/enigma-dev.RadialGM?branchName=master)](https://dev.azure.com/enigma-dev/RadialGM/_build/latest?definitionId=5&branchName=master)
==================
This project is an experimental C++ IDE for ENIGMA written in the [Qt Framework](https://www.qt.io/). The design of the program is a multilayer architecture using the [Google protocol buffer](https://developers.google.com/protocol-buffers/) format from ENIGMA's compiler backend for the data model layer in the resource editors. The resource editors are designed using Qt Creator or Qt Designer and saved in UI layout files. Their corresponding C++ classes bind the widgets in the layout files to the data model using QDataWidgetMapper and a custom resource model that hooks into arbitrary protobuf messages.

RadialGM has been made possible by your contributions & the support you've given us on Patreon!

[![Patreon](https://enigma-dev.org/site/images/v4/patreon.png)](https://www.patreon.com/m/enigma_dev) 


Building This Thing
==================
You're going to want to make sure you do a recursive git clone of the repo so you get the enigma-dev submodules initialized properly. Otherwise the architecture is pretty straight forward.

1) the editors have forms which are declarative so you can edit them in the Qt designer
2) the editors have matching C++ classes to add functionality and data bindings to the forms
3) the main window has a plugin system with external facing API for plugins
 - a builtin emake/enigma plugin is provided out of the box
 - emake functions as a server (see enigma-dev `server.proto`) and can service JDI requests remotely or over localhost
