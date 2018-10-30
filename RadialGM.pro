#-------------------------------------------------
#
# Project created by QtCreator 2017-12-02T19:38:28
#
#-------------------------------------------------

QT       += core gui printsupport
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32:RC_ICONS += images/icon.ico

TARGET = RadialGM
TEMPLATE = app
VERSION = 0.0.0.0

QMAKE_TARGET_COMPANY = ENIGMA Dev Team
QMAKE_TARGET_PRODUCT = RadialGM IDE
QMAKE_TARGET_DESCRIPTION = ENIGMA Development Environment
QMAKE_TARGET_COPYRIGHT = "Copyright \\251 2007-2018 ENIGMA Dev Team"

# Uncomment if you want QPlainTextEdit used in place of QScintilla
#CONFIG += rgm_disable_syntaxhighlight

rgm_disable_syntaxhighlight {
  SOURCES += Widgets/CodeWidgetPlain.cpp
} else {
  SOURCES += Widgets/CodeWidgetScintilla.cpp
  CONFIG += qscintilla2
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /usr/include/qt/ \
               $$PWD/Submodules/enigma-dev/CommandLine/libEGM/ \
               $$PWD/Submodules/enigma-dev/CommandLine/protos \
               $$PWD/Submodules/enigma-dev/CommandLine/protos/codegen
LIBS += -L$$PWD/Submodules/enigma-dev/CommandLine/libEGM/ \
        -lEGM \
        -lprotobuf \
        -Wl,--rpath=$$PWD/Submodules/enigma-dev/ \
        -L$$PWD/Submodules/enigma-dev/ \
        -lProtocols \
        -lpugixml \
        -lgrpc++

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    Dialogs/PreferencesDialog.cpp \
    Editors/BaseEditor.cpp \
    Editors/BackgroundEditor.cpp \
    Editors/ObjectEditor.cpp \
    Editors/SoundEditor.cpp \
    Editors/FontEditor.cpp \
    Editors/PathEditor.cpp \
    Editors/TimelineEditor.cpp \
    Editors/RoomEditor.cpp \
    Editors/SpriteEditor.cpp \
    Widgets/BackgroundView.cpp \
    Widgets/CodeWidget.cpp \
    Widgets/ColorPicker.cpp \
    Widgets/AssetView.cpp \
    Models/TreeModel.cpp \
    Components/ArtManager.cpp \
    Models/ProtoModel.cpp \
    Models/ImmediateMapper.cpp \
    Models/SpriteModel.cpp \
    Components/Utility.cpp \
    Plugins/RGMPlugin.cpp \
    Plugins/ServerPlugin.cpp \
    Components/RecentFiles.cpp \
    Editors/CodeEditor.cpp \
    Editors/ScriptEditor.cpp \
    Models/ResourceModelMap.cpp \
    Models/ModelMapper.cpp \
    Models/RepeatedProtoModel.cpp \
    Widgets/RoomView.cpp \
    Components/QMenuView.cpp \
    Models/TreeSortFilterProxyModel.cpp

HEADERS += \
    MainWindow.h \
    Dialogs/PreferencesDialog.h \
    Editors/BaseEditor.h \
    Editors/BackgroundEditor.h \
    Editors/ObjectEditor.h \
    Editors/FontEditor.h \
    Editors/PathEditor.h \
    Editors/SoundEditor.h \
    Editors/TimelineEditor.h \
    Editors/RoomEditor.h \
    Editors/SpriteEditor.h \
    Widgets/BackgroundView.h \
    Widgets/CodeWidget.h \
    Widgets/ColorPicker.h \
    Widgets/AssetView.h \
    Models/TreeModel.h \
    Components/ArtManager.h \
    Models/ProtoModel.h \
    Models/ImmediateMapper.h \
    Models/SpriteModel.h \
    Components/Utility.h \
    Plugins/RGMPlugin.h \
    Plugins/ServerPlugin.h \
    Components/RecentFiles.h \
    main.h \
    Dialogs/PreferencesKeys.h \
    Editors/CodeEditor.h \
    Editors/ScriptEditor.h \
    Models/ResourceModelMap.h \
    Models/ModelMapper.h \
    Models/RepeatedProtoModel.h \
    Widgets/RoomView.h \
    Components/QMenuView.h \
    Components/QMenuView_p.h \
    Models/TreeSortFilterProxyModel.h

FORMS += \
    MainWindow.ui \
    Dialogs/PreferencesDialog.ui \
    Dialogs/AddImageDialog.ui \
    Editors/BackgroundEditor.ui \
    Editors/ObjectEditor.ui \
    Editors/FontEditor.ui \
    Editors/PathEditor.ui \
    Editors/TimelineEditor.ui \
    Editors/RoomEditor.ui \
    Editors/SpriteEditor.ui \
    Editors/SoundEditor.ui \
    Editors/CodeEditor.ui

RESOURCES += \
    images.qrc

DISTFILES +=
