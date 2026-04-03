# ─────────────────────────────────────────────────────────────────────────────
#  build.ps1  –  PowerShell build script for sorting visualizer
# ─────────────────────────────────────────────────────────────────────────────

$MinGW  = "C:\msys64\mingw64\bin"
$CXX    = "$MinGW\g++.exe"
$Target = "sort_visualizer.exe"

Write-Host "=== Sorting Algorithm Visualizer Builder ===" -ForegroundColor Cyan
Write-Host "Compiler: $CXX"
Write-Host ""

if (-not (Test-Path $CXX)) {
    Write-Host "ERROR: MinGW-w64 g++ not found at $CXX" -ForegroundColor Red
    Write-Host "MSYS2 packages needed:"
    Write-Host "  C:\msys64\usr\bin\bash.exe -lc 'pacman -S --noconfirm mingw-w64-x86_64-gcc mingw-w64-x86_64-glfw mingw-w64-x86_64-glew'"
    exit 1
}

# Check GLFW and GLEW headers
$glfwHeader = "C:\msys64\mingw64\include\GLFW\glfw3.h"
$glewHeader = "C:\msys64\mingw64\include\GL\glew.h"

if (-not (Test-Path $glfwHeader)) {
    Write-Host "ERROR: GLFW header not found. Install with:" -ForegroundColor Red
    Write-Host "  C:\msys64\usr\bin\bash.exe -lc 'pacman -S --noconfirm mingw-w64-x86_64-glfw'"
    exit 1
}

if (-not (Test-Path $glewHeader)) {
    Write-Host "ERROR: GLEW header not found. Install with:" -ForegroundColor Red
    Write-Host "  C:\msys64\usr\bin\bash.exe -lc 'pacman -S --noconfirm mingw-w64-x86_64-glew'"
    exit 1
}

Write-Host "GLFW: found" -ForegroundColor Green
Write-Host "GLEW: found" -ForegroundColor Green
Write-Host ""
Write-Host "Compiling src\main.cpp ..."

$args_list = @(
    "-std=c++11", "-O2", "-Wall",
    "-IC:\msys64\mingw64\include",
    "-o", $Target,
    "src\main.cpp",
    "-LC:\msys64\mingw64\lib",
    "-lglfw3", "-lglew32", "-lopengl32", "-lgdi32", "-luser32", "-lkernel32"
)

& $CXX @args_list

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "BUILD FAILED" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host " BUILD SUCCEEDED: $Target"              -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "Run it:"
Write-Host "  .\sort_visualizer.exe"
Write-Host ""
Write-Host "Controls:"
Write-Host "  ESC  - Exit when all algorithms are done"
