QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app
TARGET = tst_task3_test

SOURCES += \
    tst_task3_test.cpp \
    ../task3.cpp

HEADERS += \
    ../task3.h
