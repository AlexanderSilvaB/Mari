#-------------------------------------------------
#
# Project created by QtCreator 2018-08-23T18:18:44
#
#-------------------------------------------------

QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = supervisorio
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    message.cpp \
    robotmanager.cpp \
    logger.cpp \
    imagemessage.cpp \
    tcpclient.cpp \
    camerasettingmessage.cpp

HEADERS  += mainwindow.h \
    robot.h \
    message.h \
    robotmanager.h \
    logger.h \
    tcpclient.h \
    imagemessage.h \
    camerasettingmessage.h

FORMS    += mainwindow.ui
