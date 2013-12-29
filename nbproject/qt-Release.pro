# This file is generated automatically. Do not edit.
# Use project properties -> Build -> Qt -> Expert -> Custom Definitions.
TEMPLATE = app
DESTDIR = dist/Release/GNU-Linux-x86
TARGET = HDMIViewer
VERSION = 1.0.0
CONFIG -= debug_and_release app_bundle lib_bundle
CONFIG += release 
PKGCONFIG +=
QT = core gui widgets
SOURCES += DataCallback.cpp DummyReceptionCallback.cpp EBMLElement.cpp EBMLTreeNode.cpp EncodingRingBuffer.cpp JPEGEncoder.cpp MainWindow.cpp MatroskaCuePoint.cpp MatroskaElementDefinitions.cpp MatroskaEncoder.cpp MuxFeeder.cpp PulseAudioSink.cpp QueueingEncoder.cpp RenderingTest.cpp TimedPacket.cpp VideoSurface.cpp main.cpp
HEADERS += /mnt/data/home/XXXXXX/NetBeansProjects/HDMIViewer/MainWindow.h DataCallback.h DelayedReceptionCallback.h DummyReceptionCallback.h EBMLElement.h EBMLTreeNode.h EncodingRingBuffer.h JPEGEncoder.h MatroskaCuePoint.h MatroskaEBMLElementDefinitions.h MatroskaEncoder.h MuxFeeder.h PulseAudioSink.h QueueingEncoder.h RenderingTest.h TimedPacket.h VideoSurface.h global_macros.h
FORMS +=
RESOURCES +=
TRANSLATIONS +=
OBJECTS_DIR = build/Release/GNU-Linux-x86
MOC_DIR = 
RCC_DIR = 
UI_DIR = 
QMAKE_CC = gcc
QMAKE_CXX = g++
DEFINES += 
INCLUDEPATH += 
LIBS += 
