#-------------------------------------------------
#
# Project created by QtCreator 2014-03-06T15:26:24
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Projekt
TEMPLATE = app


SOURCES += main.cpp\
    databaseconnection.cpp \
    pgformular.cpp \
    pgdiagram.cpp \
    dbcustomformular.cpp

HEADERS  += \
    databaseconnection.h \
    pgformular.h \
    pgdiagram.h \
    dbcustomformular.h

FORMS    += \
    pgformular.ui \
    pgdiagram.ui \
    dbcustomformular.ui
