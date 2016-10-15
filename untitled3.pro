#-------------------------------------------------
#
# Project created by QtCreator 2016-10-04T22:16:38
#
#-------------------------------------------------

QT       += core gui network positioning

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled3
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h

FORMS    += dialog.ui

CONFIG += mobility
MOBILITY = location sql

