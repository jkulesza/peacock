#!/bin/bash

g++ --std=c++11 \
  `Magick++-config --cxxflags --cppflags` \
  -g -o peacock_debug src/CB_Converter.cpp src/peacock.cpp \
  `Magick++-config --ldflags --libs`

g++ --std=c++11 \
  `Magick++-config --cxxflags --cppflags` \
  -O3 -o peacock_release src/CB_Converter.cpp src/peacock.cpp \
  `Magick++-config --ldflags --libs`

