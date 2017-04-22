#-------------------------------------------------
#
# Project created by QtCreator 2017-04-12T20:10:01
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ihm2
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    playbutton.cpp \
    audioprogressbar.cpp \
    volumecontroller.cpp \
    client.cpp

HEADERS  += mainwindow.h \
    playbutton.h \
    audioprogressbar.h \
    volumecontroller.h \
    client.h \
    ../constantes.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc

CONFIG += c++11

LIBS += -lz

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/release/ -ltag
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lib/debug/ -ltag
else:unix: LIBS += -L$$PWD/../lib/ -ltag

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../lib/release/libtag.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../lib/debug/libtag.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../lib/release/tag.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../lib/debug/tag.lib
else:unix: PRE_TARGETDEPS += $$PWD/../lib/libtag.a
