cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(cvr)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ldl -Wall")

find_package(PkgConfig REQUIRED)

pkg_check_modules(PULSE REQUIRED libpulse)
include_directories(${PULSE_INCLUDE_DIRS})

pkg_check_modules(PULSESIMPLE REQUIRED libpulse-simple)
include_directories(${PULSESIMPLE_INCLUDE_DIRS})

find_package(Qt4 REQUIRED)

find_package(Threads REQUIRED)

find_package(X11 REQUIRED)
include_directories(${X11_X11_INCLUDE_PATH})

find_package(JPEG REQUIRED)

find_path(DECKLINK_INCLUDE_DIRS NAMES include/DeckLinkAPI.h)
include_directories(${DECKLINK_INCLUDE_DIRS})

add_subdirectory(src)
