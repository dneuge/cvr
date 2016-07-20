configure_file(
    "include_DeckLinkAPIDispatch.cpp.in"
    "include_DeckLinkAPIDispatch.cpp"
)

set(SOURCES
    DataCallback.cpp
    DummyReceptionCallback.cpp
    EBMLElement.cpp
    EBMLTreeNode.cpp
    EncodingRingBuffer.cpp
    JPEGEncoder.cpp
    MainWindow.cpp
    MatroskaCuePoint.cpp
    MatroskaElementDefinitions.cpp
    MatroskaEncoder.cpp
    MuxFeeder.cpp
    PulseAudioSink.cpp
    QueueingEncoder.cpp
    RenderingTest.cpp
    TimedPacket.cpp
    VideoSurface.cpp
)

add_library(cvr_lib STATIC ${SOURCES})
target_link_libraries(cvr_lib
    Qt4::QtCore Qt4::QtGui Qt4::QtMultimedia
    ${CMAKE_THREAD_LIBS_INIT}
    ${X11_X11_LIB} ${X11_Xv_LIB} ${X11_XTest_LIB}
    ${JPEG_LIBRARIES}
    ${PULSESIMPLE_LIBRARIES}
)

add_executable(cvr main.cpp)
target_link_libraries(cvr cvr_lib)
