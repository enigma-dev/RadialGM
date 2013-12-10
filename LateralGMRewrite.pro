#-------------------------------------------------
#
# Project created by QtCreator 2013-06-03T03:51:27
#
#-------------------------------------------------

QT       += core gui
qtHaveModule(printsupport): QT += printsupport

INCLUDEPATH += gmk/src/include/ gmk/lib/zlib

LIBS	+= -L$$[QT_INSTALL_LIBS] -lqscintilla2 -Llib/zlib -lz

QMAKE_CXXFLAGS += -std=c++0x

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LateralGMRewrite
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
    gmk/src/gmk.cpp \
    gmk/src/gmkaction.cpp \
    gmk/src/gmkbackground.cpp \
    gmk/src/gmkexception.cpp \
    gmk/src/gmkfont.cpp \
    gmk/src/gmkgameinfo.cpp \
    gmk/src/gmkincludefile.cpp \
    gmk/src/gmkobject.cpp \
    gmk/src/gmkpath.cpp \
    gmk/src/gmkresource.cpp \
    gmk/src/gmkroom.cpp \
    gmk/src/gmkrypt.cpp \
    gmk/src/gmkscript.cpp \
    gmk/src/gmksettings.cpp \
    gmk/src/gmksound.cpp \
    gmk/src/gmksprite.cpp \
    gmk/src/gmktimeline.cpp \
    gmk/src/gmktree.cpp \
    gmk/src/gmktrigger.cpp \
    # gmk/src/main.cpp \
    gmk/src/stream.cpp \
    main.cpp \
    resource/gmobject.cpp \
    resource/sprite.cpp \
    resource/background.cpp \
    resource/shader.cpp \
    resource/timeline.cpp \
    resource/path.cpp \
    resource/room.cpp \
    resource/sound.cpp \
    resource/font.cpp \
    resource/script.cpp \
    serializer/projectmanager.cpp \
    dialogs/preferencesdialog.cpp \
    serializer/resourcenode.cpp \
    widgets/gameinfoeditor.cpp

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
    resource/gmobject.h \
    resource/sprite.h \
    resource/background.h \
    resource/shader.h \
    resource/timeline.h \
    resource/path.h \
    resource/room.h \
    resource/sound.h \
    resource/font.h \
    resource/script.h \
    serializer/projectmanager.h \
    dialogs/preferencesdialog.h \
    serializer/resourcenode.h \
    gmk/src/include/stream.h \
    gmk/src/include/gmktrigger.h \
    gmk/src/include/gmktree.h \
    gmk/src/include/gmktimeline.h \
    gmk/src/include/gmksprite.h \
    gmk/src/include/gmksound.h \
    gmk/src/include/gmksettings.h \
    gmk/src/include/gmkscript.h \
    gmk/src/include/gmkrypt.h \
    gmk/src/include/gmkroom.h \
    gmk/src/include/gmkresource.h \
    gmk/src/include/gmkpath.h \
    gmk/src/include/gmkobject.h \
    gmk/src/include/gmkincludefile.h \
    gmk/src/include/gmkgameinfo.h \
    gmk/src/include/gmkfont.h \
    gmk/src/include/gmkexception.h \
    gmk/src/include/gmkcommon.h \
    gmk/src/include/gmkbackground.h \
    gmk/src/include/gmkaction.h \
    gmk/src/include/gmk.h \
    widgets/gameinfoeditor.h

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    gmk/.gitignore \
    gmk/Makefile \
    gmk/lib/zlib/libz.a \
    gmk/lib/zlib/libzmac.a \
    gmk/lib/zlib/zlibstat.lib \
    gmk/lib/zlib/zlibstatd.lib

FORMS += \
    widgets/pathwidget.ui \
    widgets/spritewidget.ui \
    widgets/roomwidget.ui \
    dialogs/preferencesdialog.ui

VERSION = 0.0.1.0
QMAKE_TARGET_COMPANY = The LateralGM Development Team
QMAKE_TARGET_PRODUCT = LateralGM IDE
QMAKE_TARGET_DESCRIPTION = The free and open source game maker.
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2013 The LateralGM Development Team

ICON = lgm-logo.svg
RC_FILE = myapp.rc
