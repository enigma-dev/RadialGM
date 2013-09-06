#-------------------------------------------------
#
# Project created by QtCreator 2013-06-03T03:51:27
#
#-------------------------------------------------

QT       += core gui

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
    main.cpp

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
    gmk/src/include/gmk.hpp \
    gmk/src/include/gmkaction.hpp \
    gmk/src/include/gmkbackground.hpp \
    gmk/src/include/gmkcommon.hpp \
    gmk/src/include/gmkexception.hpp \
    gmk/src/include/gmkfont.hpp \
    gmk/src/include/gmkgameinfo.hpp \
    gmk/src/include/gmkincludefile.hpp \
    gmk/src/include/gmkobject.hpp \
    gmk/src/include/gmkpath.hpp \
    gmk/src/include/gmkresource.hpp \
    gmk/src/include/gmkroom.hpp \
    gmk/src/include/gmkrypt.hpp \
    gmk/src/include/gmkscript.hpp \
    gmk/src/include/gmksettings.hpp \
    gmk/src/include/gmksound.hpp \
    gmk/src/include/gmksprite.hpp \
    gmk/src/include/gmktimeline.hpp \
    gmk/src/include/gmktree.hpp \
    gmk/src/include/gmktrigger.hpp \
    gmk/src/include/stream.hpp \
    gmk/lib/zlib/zconf.h \
    gmk/lib/zlib/zlib.h

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
    widgets/objectwidget.ui \
    widgets/spritewidget.ui \
    widgets/roomwidget.ui
