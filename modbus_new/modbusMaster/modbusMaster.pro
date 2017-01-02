#-------------------------------------------------
#
# Project created by QtCreator 2016-08-03T11:38:23
#
#-------------------------------------------------

QT       += core gui serialbus serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = modbusMaster
TEMPLATE = app


SOURCES += main.cpp\
        modbusmaster.cpp

HEADERS  += modbusmaster.h

FORMS    += modbusmaster.ui
