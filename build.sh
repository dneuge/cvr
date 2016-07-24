#!/bin/bash

basedir="$(dirname $(realpath $0))"
#decklink_dir="${basedir}/../../Downloads/Blackmagic DeckLink SDK 9.7.7/Linux/include"

#cd ${basedir} && rm -Rf build && mkdir build && cd build && cmake .. -DDECKLINK_INCLUDE_DIRS="${decklink_dir}" && make
cd ${basedir} && rm -Rf build && mkdir build && cd build && cmake .. && make
