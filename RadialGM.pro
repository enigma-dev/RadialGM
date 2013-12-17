#-------------------------------------------------
#
# Project created by QtCreator 2013-06-03T03:51:27
#
#-------------------------------------------------

QT       += core gui
qtHaveModule(printsupport): QT += printsupport

LIBS	+= -L$$[QT_INSTALL_LIBS] -lqscintilla2

QMAKE_CXXFLAGS += -std=c++0x

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RadialGM
TEMPLATE = app

SOURCES += mainwindow.cpp \
    widgets/fontwidget.cpp \
    widgets/pathwidget.cpp \
    widgets/scriptwidget.cpp \
    widgets/objectwidget.cpp \
    widgets/timelinewidget.cpp \
    widgets/spritewidget.cpp \
    widgets/soundwidget.cpp \
    widgets/backgroundwidget.cpp \
    widgets/roomwidget.cpp \
    dialogs/aboutdialog.cpp \
    serializer/iconmanager.cpp \
    widgets/shaderwidget.cpp \
    main.cpp \
    serializer/projectmanager.cpp \
    dialogs/preferencesdialog.cpp \
    serializer/resourcenode.cpp \
    widgets/gameinfoeditor.cpp \
    serializer/resourcemanager.cpp \
    serializer/resource.cpp

HEADERS  += mainwindow.h \
    widgets/fontwidget.h \
    widgets/pathwidget.h \
    widgets/scriptwidget.h \
    widgets/objectwidget.h \
    widgets/timelinewidget.h \
    widgets/spritewidget.h \
    widgets/soundwidget.h \
    widgets/backgroundwidget.h \
    widgets/roomwidget.h \
    dialogs/aboutdialog.h \
    serializer/serializer.h \
    serializer/iconmanager.h \
    widgets/shaderwidget.h \
    gmk/lib/zlib/zconf.h \
    gmk/lib/zlib/zlib.h \
    serializer/projectmanager.h \
    dialogs/preferencesdialog.h \
    serializer/resourcenode.h \
    widgets/gameinfoeditor.h \
    serializer/resourcemanager.h \
    serializer/resource.h

RESOURCES += \
    resources.qrc

FORMS += \
    widgets/pathwidget.ui \
    widgets/spritewidget.ui \
    widgets/roomwidget.ui \
    dialogs/preferencesdialog.ui

VERSION = 0.0.1.0
QMAKE_TARGET_COMPANY = The RadialGM Development Team
QMAKE_TARGET_PRODUCT = RadialGM IDE
QMAKE_TARGET_DESCRIPTION = The free and open source game maker file editor.
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2013 The RadialGM Development Team

ICON = lgm-logo.svg
RC_FILE = myapp.rc
