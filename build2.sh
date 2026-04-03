#!/bin/bash
# Build script for Sorting Algorithm Visualizer
# Run via: C:\msys64\usr\bin\bash.exe "/c/Users/vietnam/Desktop/bubble_sort_descending/build2.sh"
export PATH=/mingw64/bin:/usr/bin:$PATH
PROJ=/c/Users/vietnam/Desktop/bubble_sort_descending

echo "=== Sorting Visualizer Build ==="
g++ --version
echo ""
echo "Compiling src/main.cpp..."

g++ \
    -std=c++11 -O2 -Wall \
    -I/mingw64/include \
    -o "$PROJ/sort_visualizer.exe" \
    "$PROJ/src/main.cpp" \
    -L/mingw64/lib \
    -lglfw3 -lglew32 -lopengl32 -lgdi32 -luser32 -lkernel32

RESULT=$?
if [ $RESULT -eq 0 ]; then
    echo ""
    echo "======================================"
    echo " SUCCESS: sort_visualizer.exe created"
    echo "======================================"
    echo "Run: .\\sort_visualizer.exe (or double-click run.bat)"
else
    echo ""
    echo "BUILD FAILED (exit $RESULT)"
fi
exit $RESULT
