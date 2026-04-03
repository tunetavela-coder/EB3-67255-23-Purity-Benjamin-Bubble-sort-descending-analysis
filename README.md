# Bubble Sort vs Quick Sort — Visualizer

A real-time side-by-side visualizer comparing **Bubble Sort** and **Quick Sort**, built with OpenGL 2.1, GLFW, and GLEW in C++11.

Each algorithm runs at its **natural pace** — Quick Sort finishes dramatically faster, making the O(n log n) vs O(n²) difference immediately visible.

| Feature | Detail |
|---|---|
| Bars | 80 elements, randomly seeded |
| Bubble Sort | ~30 s (anchors the scale) |
| Quick Sort | ~4–5 s (proportional to step count) |
| Window | Resizable — all layout scales with size |
| Colours | Blue = unsorted · Red = active · Green = sorted |

---

## Algorithm Analysis

This assignment visualizes two fundamental sorting algorithms with contrasting characteristics:

### Bubble Sort
- **Time Complexity:** O(n²) best, average, and worst case
- **Space Complexity:** O(1) — in-place, no extra memory
- **Stable:** Yes — maintains relative order of equal elements
- **Adaptive:** Somewhat — can detect sorted sequences and terminate early
- **Characteristics:** Simple implementation, intuitive behavior, poor performance on large datasets
- **Use Cases:** Educational purposes, small datasets, nearly-sorted data, embedded systems with memory constraints
- **Visual Pattern:** Slow rightward "bubble" of largest unsorted element toward the end each pass; very early detection of sorted elements at tail

### Quick Sort  
- **Time Complexity:** O(n log n) average, O(n²) worst case (rare with good pivot selection)
- **Space Complexity:** O(log n) average — recursive call stack depth  
- **Stable:** No (standard implementation) — relative order may not be preserved
- **Adaptive:** No — performance does not improve on nearly-sorted data
- **Characteristics:** Efficient divide-and-conquer, practical for large datasets, highly optimized in production code
- **Use Cases:** General-purpose sorting, production systems, large datasets, embedded in most standard libraries
- **Visual Pattern:** Rapid partitioning into disjoint regions; dramatically faster convergence; logarithmic depth of recursion visible as parallel sorting of independent regions

### Complexity Comparison

| Metric | Bubble Sort | Quick Sort |
|--------|------------|-----------|
| Best Case | O(n) | O(n log n) |
| Average Case | O(n²) | O(n log n) |
| Worst Case | O(n²) | O(n²) |
| Space | O(1) | O(log n) |
| Stable | ✓ Yes | ✗ No |
| In-Place | ✓ Yes | ✓ Yes |
| Cache Friendly | ✗ Poor | ✓ Good |

### Assignment Learning Objectives

This visualizer demonstrates:
- **Algorithmic Efficiency:** How algorithm choice dramatically impacts execution time at scale (80 elements; imagine 1 million)
- **Big-O Analysis in Practice:** Theoretical O(n²) vs O(n log n) made empirically visible—not just a formula
- **Comparison-Based Sorting:** Both algorithms rely only on element comparisons; no assumptions about data values
- **Divide-and-Conquer vs Brute-Force:** QSort's recursive partitioning vs Bubble Sort's iterative comparison model
- **Trade-offs:** Bubble Sort is simpler and uses no extra stack; Quick Sort needs recursion but is exponentially faster

---

## Demo

Watch the visualizer in action: [Bubble Sort vs Quick Sort (Loom)](https://www.loom.com/share/6ce238a0a339449abdda826edb37e3d2)

## Prerequisites

### Windows (MSYS2 / MinGW-w64)

1. **Install MSYS2** from https://www.msys2.org  
   Default install path: `C:\msys64`

2. Open the **MSYS2 MinGW 64-bit** terminal and install dependencies:

   ```bash
   pacman -S mingw-w64-x86_64-gcc \
             mingw-w64-x86_64-glfw \
             mingw-w64-x86_64-glew
   ```

3. The runtime DLLs required at runtime are **not included in this repo** (excluded by `.gitignore`). Copy them from your MSYS2 installation into the project folder before running:

   ```
   C:\msys64\mingw64\bin\glfw3.dll
   C:\msys64\mingw64\bin\glew32.dll
   C:\msys64\mingw64\bin\libgcc_s_seh-1.dll
   C:\msys64\mingw64\bin\libstdc++-6.dll
   C:\msys64\mingw64\bin\libwinpthread-1.dll
   ```

### Linux

Install GLFW and GLEW via your distro's package manager:

```bash
# Debian / Ubuntu
sudo apt install g++ libglfw3-dev libglew-dev

# Fedora / RHEL
sudo dnf install gcc-c++ glfw-devel glew-devel

# Arch
sudo pacman -S gcc glfw glew
```

---

## Build

### Windows — double-click

Double-click **`build.bat`** in the project folder.  
The executable `sort_visualizer.exe` will be produced in the project root.

### Windows — command line (MSYS2 shell)

```bash
# From the MSYS2 MinGW 64-bit terminal
cd /c/Users/<you>/Desktop/bubble_sort_descending
bash build2.sh
```

Or with `make` (if available in your MSYS2 environment):

```bash
make          # windowed build (no console)
make console  # build with console output (shows step counts)
```

### Linux

```bash
g++ -std=c++11 -O2 -Wall \
    -o sort_visualizer \
    src/main.cpp \
    -lglfw -lGLEW -lGL

./sort_visualizer
```

> **Note:** On Linux the output binary is `sort_visualizer` (no `.exe`). The build scripts target Windows; use the command above directly.

---

## Run

### Windows

```bat
sort_visualizer.exe
```

Or double-click **`run.bat`**.

### Linux

```bash
./sort_visualizer
```

**Controls:** Press `Esc` to exit.

---

## Project Structure

```
bubble_sort_descending/
├── src/
│   └── main.cpp          # All source code (single translation unit)
├── build.bat             # Windows one-click build (calls build2.sh via MSYS2)
├── build2.sh             # Shell build script (MSYS2 / MinGW-w64 paths)
├── build.sh              # Alternate shell build script
├── build.ps1             # PowerShell build script
├── Makefile              # Make rules for MSYS2 MinGW-w64
├── run.bat               # Windows one-click run
└── .gitignore
```

---

## How It Works

Steps for each algorithm are **pre-computed** before the window opens. During playback the visualizer advances each panel's step pointer at a rate proportional to its total step count, so every algorithm consumes its fair share of clock time relative to its actual complexity.

The heat-spotlight effect (bars near the active comparison warm from blue toward red) uses a `1 / (1 + d²)` distance falloff, making each algorithm's scan pattern clearly visible — Bubble Sort's slow rightward wave vs Quick Sort's rapid partitioning.
