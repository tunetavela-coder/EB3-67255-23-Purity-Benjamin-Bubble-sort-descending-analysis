#!/bin/bash
cd /c/Users/vietnam/Desktop/bubble_sort_descending
/mingw64/bin/g++ -std=c++11 -O2 -Wall \
    -I/mingw64/include \
    -o sort_visualizer.exe \
    src/main.cpp \
    -L/mingw64/lib \
    -lglfw3 -lglew32 -lopengl32 -lgdi32 -luser32 -lkernel32 2>&1
echo "BUILD_EXIT:$?"
