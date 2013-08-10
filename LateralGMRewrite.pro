#-------------------------------------------------
#
# Project created by QtCreator 2013-06-03T03:51:27
#
#-------------------------------------------------

QT       += core gui

LIBS	+= -L$$[QT_INSTALL_LIBS] -lqscintilla2

QMAKE_CXXFLAGS += -std=c++0x

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LateralGMRewrite
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    widgets/fontwidget.cpp \
    widgets/pathwidget.cpp \
    widgets/scriptwidget.cpp \
    widgets/objectwidget.cpp \
    widgets/timelinewidget.cpp \
    widgets/spritewidget.cpp \
    widgets/soundwidget.cpp \
    widgets/backgroundwidget.cpp \
    widgets/roomwidget.cpp \
    widgets/modelwidget.cpp \
    widgets/materialwidget.cpp \
    customwidgets/qttoolbar.cpp \
    dialogs/aboutdialog.cpp \
    serializer/serializer.cpp \
    serializer/iconmanager.cpp

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
    widgets/modelwidget.h \
    widgets/materialwidget.h \
    customwidgets/qttoolbar.h \
    dialogs/aboutdialog.h \
    serializer/serializer.h \
    serializer/iconmanager.h

RESOURCES += \
    resources.qrc

OTHER_FILES +=

FORMS += \
    widgets/pathwidget.ui \
    widgets/objectwidget.ui \
    widgets/spritewidget.ui \
    widgets/roomwidget.ui \
    widgets/materialwidget.ui
