QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

TARGET = tst_task2_test

SOURCES +=  tst_task2_test.cpp \
    ../task2.cpp

HEADERS += \
    ../task2.h
