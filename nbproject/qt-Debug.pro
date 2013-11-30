# This file is generated automatically. Do not edit.
# Use project properties -> Build -> Qt -> Expert -> Custom Definitions.
TEMPLATE = app
DESTDIR = dist/Debug/GNU-Linux-x86
TARGET = HDMIViewer
VERSION = 1.0.0
CONFIG -= debug_and_release app_bundle lib_bundle
CONFIG += debug 
PKGCONFIG +=
QT = core gui widgets
SOURCES += DataCallback.cpp MainWindow.cpp RenderingTest.cpp VideoSurface.cpp main.cpp
HEADERS += /mnt/data/home/XXXXXX/NetBeansProjects/HDMIViewer/MainWindow.h DataCallback.h RenderingTest.h VideoSurface.h
FORMS +=
RESOURCES +=
TRANSLATIONS +=
OBJECTS_DIR = build/Debug/GNU-Linux-x86
MOC_DIR = 
RCC_DIR = 
UI_DIR = 
QMAKE_CC = gcc
QMAKE_CXX = g++
DEFINES += 
INCLUDEPATH += "/mnt/data/home/XXXXXX/Downloads/Blackmagic DeckLink SDK 9.7.7/Linux/include" 
LIBS += 
QT += multimedia
