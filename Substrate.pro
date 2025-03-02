#-------------------------------------------------
#
# Project created by QtCreator 2013-01-14T19:56:14
#
#-------------------------------------------------

QT += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TEMPLATE = app
MOC_DIR = moc
VERSION = 2.0
RC_FILE += ressources.rc

CONFIG(debug, debug|release) {
    DESTDIR = debug
    OBJECTS_DIR = debug/obj
    TARGET = Substrated
} else {
    DESTDIR = release
    OBJECTS_DIR = release/obj
    TARGET = Substrate
}

CONFIG += qwt

DEFINES += QWT_DLL

win32:INCLUDEPATH += D:/Programmes/qwt-6.3.0/src D:/Programmes/eigen-master
unix:INCLUDEPATH += /usr/local/qwt-6.3.0-dev/include \
                    /usr/include/eigen3

win32:TR_EXCLUDE += D:/Programmes/eigen-master/*

win32:LIBS += -L'D:/Programmes/qwt-6.3.0/build/Desktop_Qt_6_7_0_MinGW_64_bit-Release/lib' -lqwt
unix:LIBS += -L'/usr/local/qwt-6.3.0-dev/lib' -lqwt

SOURCES += Crack.cpp \
        Degrade.cpp \
        FancyLineEdit.cpp \
        FenetreCouleurs.cpp \
        FenetreGenerateurs.cpp \
        FenetrePrincipale.cpp \
        Interpolation.cpp \
        main.cpp \
        Palette.cpp \
        SandPainter.cpp \
        Substrate.cpp \
        WidgetSubstrate.cpp

HEADERS += constantes.h \
        Crack.h \
        Degrade.h \
        FancyLineEdit.h \
        FenetreCouleurs.h \
        FenetreGenerateurs.h \
        FenetrePrincipale.h \
        Interpolation.h \
        Palette.h \
        SandPainter.h \
        Substrate.h  \
        WidgetSubstrate.h

RESOURCES += ressources.qrc
