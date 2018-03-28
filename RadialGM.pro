#-------------------------------------------------
#
# Project created by QtCreator 2017-12-02T19:38:28
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++11 qscintilla2

win32:RC_ICONS += images/icon.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RadialGM
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /usr/include/qt/ $$PWD/Submodules/enigma-dev/CommandLine/libGMX/ $$PWD/Submodules/enigma-dev/CommandLine/protos $$PWD/Submodules/enigma-dev/CommandLine/protos/codegen
LIBS += -L$$PWD/Submodules/enigma-dev/CommandLine/libGMX/ -lGMX -lprotobuf -Wl,--rpath=$$PWD/Submodules/enigma-dev/ -L$$PWD/Submodules/enigma-dev/ -lProtocols -lpugixml -lgrpc++

SOURCES += \
        main.cpp \
    MainWindow.cpp \
    Dialogs/PreferencesDialog.cpp \
    Editors/BackgroundEditor.cpp \
    Editors/ObjectEditor.cpp \
    Editors/FontEditor.cpp \
    Editors/PathEditor.cpp \
    Editors/TimelineEditor.cpp \
    Editors/RoomEditor.cpp \
    Editors/SpriteEditor.cpp \
    Widgets/BackgroundRenderer.cpp \
    Models/TreeModel.cpp \
    Components/ArtManager.cpp \
    Editors/BaseEditor.cpp \
    Models/ProtoModel.cpp \
    Models/ImmediateMapper.cpp \
    Components/Utility.cpp \
    Plugins/RGMPlugin.cpp \
    Plugins/ServerPlugin.cpp \
    Models/SpriteModel.cpp

HEADERS += \
    MainWindow.h \
    Dialogs/PreferencesDialog.h \
    Editors/BackgroundEditor.h \
    Editors/ObjectEditor.h \
    Editors/FontEditor.h \
    Editors/PathEditor.h \
    Editors/TimelineEditor.h \
    Editors/RoomEditor.h \
    Editors/SpriteEditor.h \
    Widgets/BackgroundRenderer.h \
    Models/TreeModel.h \
    Components/ArtManager.h \
    Models/ProtoModel.h \
    Models/ImmediateMapper.h \
    Components/Utility.h \
    Editors/BaseEditor.h \
    Plugins/RGMPlugin.h \
    Plugins/ServerPlugin.h \
    Models/SpriteModel.h

FORMS += \
    MainWindow.ui \
    Dialogs/PreferencesDialog.ui \
    Editors/BackgroundEditor.ui \
    Editors/ObjectEditor.ui \
    Editors/FontEditor.ui \
    Editors/PathEditor.ui \
    Editors/TimelineEditor.ui \
    Editors/RoomEditor.ui \
    Dialogs/AddImageDialog.ui \
    Editors/SpriteEditor.ui

RESOURCES += \
    images.qrc

DISTFILES +=
