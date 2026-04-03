@echo off
REM ─────────────────────────────────────────────────────────────────────────
REM  run.bat  –  Launch the Sorting Algorithm Visualizer
REM  Double-click this file OR run from command prompt
REM ─────────────────────────────────────────────────────────────────────────
cd /d "%~dp0"
if not exist sort_visualizer.exe (
    echo ERROR: sort_visualizer.exe not found
    echo Run build.bat first
    pause
    exit /b 1
)
echo Starting Sorting Algorithm Visualizer...
echo Controls: ESC = exit after algorithms complete
echo.
sort_visualizer.exe
