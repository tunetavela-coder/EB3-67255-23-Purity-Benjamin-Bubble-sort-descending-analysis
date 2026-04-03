# ─────────────────────────────────────────────────────────────────────────────
# Sorting Visualizer - Makefile for MSYS2 MinGW-w64
# ─────────────────────────────────────────────────────────────────────────────

CXX      = C:/msys64/mingw64/bin/g++.exe
CXXFLAGS = -std=c++11 -O2 -Wall -IC:/msys64/mingw64/include
LDFLAGS  = -LC:/msys64/mingw64/lib \
           -lglfw3 -lglew32 -lopengl32 -lgdi32 -luser32 -lkernel32 \
           -mwindows
# -mwindows: prevents console window from appearing on Windows
# Remove -mwindows if you want to see printf output in a console

TARGET   = sort_visualizer.exe
SRC      = src/main.cpp

.PHONY: all clean run console

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
	@echo Build successful: $(TARGET)

# Build with console window (shows OpenGL info + step counts)
console: $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $^ $(LDFLAGS:-mwindows=)
	@echo Console build successful: $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	del /f $(TARGET) 2>nul || rm -f $(TARGET)
