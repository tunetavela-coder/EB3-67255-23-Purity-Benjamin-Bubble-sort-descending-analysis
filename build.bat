@echo off
REM ─────────────────────────────────────────────────────────────────────────
REM  build.bat  –  Build sorting visualizer using MSYS2 MinGW-w64
REM  Simply double-click this file or run from command prompt
REM ─────────────────────────────────────────────────────────────────────────
cd /d "%~dp0"
echo === Sorting Visualizer Builder ===
C:\msys64\usr\bin\bash.exe "%~dp0build2.sh"
IF %ERRORLEVEL% NEQ 0 (
    echo.
    echo BUILD FAILED
    pause
    exit /b 1
)
echo.
echo Build complete! Run: sort_visualizer.exe
pause
