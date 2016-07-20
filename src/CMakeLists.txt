configure_file(
    "link/include_DeckLinkAPIDispatch.cpp.in"
    "link/include_DeckLinkAPIDispatch.cpp"
)

set(SOURCES
    encoder/EBMLElement.cpp
    encoder/EBMLTreeNode.cpp
    encoder/EncodingRingBuffer.cpp
    encoder/JPEGEncoder.cpp
    encoder/MatroskaCuePoint.cpp
    encoder/MatroskaElementDefinitions.cpp
    encoder/MatroskaEncoder.cpp
    encoder/MuxFeeder.cpp
    encoder/QueueingEncoder.cpp
    gui/MainWindow.cpp
    gui/RenderingTest.cpp
    gui/VideoSurface.cpp
    link/DataCallback.cpp
    link/DummyReceptionCallback.cpp
    link/TimedPacket.cpp
    sound/PulseAudioSink.cpp
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
