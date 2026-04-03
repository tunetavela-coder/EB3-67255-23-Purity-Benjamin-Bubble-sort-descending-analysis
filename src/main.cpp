/*
 * Sorting Algorithm Visualizer
 * Compares: Bubble Sort vs Quick Sort
 * OpenGL 2.1 / GLFW / GLEW, C++11
 *
 * Each algorithm plays at its natural pace — faster algorithms finish sooner.
 * Window is fully resizable; all layout and text scales with the framebuffer.
 *
 * Bar colours:
 *   Blue  = unsorted
 *   Red   = actively compared / swapped  (+ heat-spotlight on neighbours)
 *   Green = in final sorted position
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include <random>

// ─────────────────────────────────────────────────────────────────────────────
// Configuration
// ─────────────────────────────────────────────────────────────────────────────
static const int    WIN_W_INIT = 1200;
static const int    WIN_H_INIT = 700;
static const int    NUM_BARS   = 80;
static const int    NUM_PANELS = 2;
static const double BASE_SECS  = 30.0;   // slowest algo (Bubble) target
static const double MIN_SECS   = 3.5;    // floor so even Quick Sort is visible

// ─────────────────────────────────────────────────────────────────────────────
// Colour
// ─────────────────────────────────────────────────────────────────────────────
struct RGB { float r, g, b; };

static RGB lerp_color(RGB a, RGB b, float t) {
    if (t < 0.f) t = 0.f;
    if (t > 1.f) t = 1.f;
    return { a.r+(b.r-a.r)*t, a.g+(b.g-a.g)*t, a.b+(b.b-a.b)*t };
}

// ─────────────────────────────────────────────────────────────────────────────
// Step record
// ─────────────────────────────────────────────────────────────────────────────
struct Step {
    int a, b;
    std::vector<int>  arr;
    std::vector<bool> srt;
};

// ─────────────────────────────────────────────────────────────────────────────
// Algorithm pre-computation
// ─────────────────────────────────────────────────────────────────────────────
static std::vector<Step> compute_bubble(std::vector<int> arr) {
    std::vector<Step> out;
    int n = (int)arr.size();
    std::vector<bool> srt(n, false);
    for (int i = 0; i < n-1; ++i) {
        for (int j = 0; j < n-1-i; ++j) {
            if (arr[j] > arr[j+1]) std::swap(arr[j], arr[j+1]);
            out.push_back({j, j+1, arr, srt});
        }
        srt[n-1-i] = true;
    }
    srt[0] = true;
    if (!out.empty()) out.back().srt.assign(n, true);
    return out;
}

static void qs(std::vector<int>& arr, int lo, int hi,
               std::vector<Step>& out, std::vector<bool>& srt) {
    if (lo >= hi) { if (lo == hi) srt[lo] = true; return; }
    int pv = arr[hi], i = lo-1;
    for (int j = lo; j < hi; ++j) {
        if (arr[j] <= pv) { ++i; std::swap(arr[i], arr[j]); }
        out.push_back({j, hi, arr, srt});
    }
    std::swap(arr[i+1], arr[hi]);
    srt[i+1] = true;
    out.push_back({i+1, -1, arr, srt});
    qs(arr, lo, i,   out, srt);
    qs(arr, i+2, hi, out, srt);
}

static std::vector<Step> compute_quick(std::vector<int> arr) {
    std::vector<Step> out;
    int n = (int)arr.size();
    std::vector<bool> srt(n, false);
    qs(arr, 0, n-1, out, srt);
    if (!out.empty()) out.back().srt.assign(n, true);
    return out;
}

// ─────────────────────────────────────────────────────────────────────────────
// Panel state
// ─────────────────────────────────────────────────────────────────────────────
struct Panel {
    std::string       name;
    std::vector<int>  data;
    std::vector<bool> srt;
    int       ca = -1, cb = -1;  // active comparison indices
    bool      done       = false;
    long long steps      = 0;
    double    elapsed_s  = 0.0;
    double    finished_s = -1.0; // wall-clock second when this algo finished
    double    duration   = BASE_SECS;
    size_t    step_idx   = 0;
};

// ─────────────────────────────────────────────────────────────────────────────
// Globals
// ─────────────────────────────────────────────────────────────────────────────
static Panel             g_panels[NUM_PANELS];
static std::vector<Step> g_steps[NUM_PANELS];
static int               g_fb_w = WIN_W_INIT;
static int               g_fb_h = WIN_H_INIT;

// ─────────────────────────────────────────────────────────────────────────────
// Framebuffer resize — updates viewport and projection so layout adapts live
// ─────────────────────────────────────────────────────────────────────────────
static void on_resize(GLFWwindow*, int w, int h) {
    if (w < 1 || h < 1) return;
    g_fb_w = w; g_fb_h = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ─────────────────────────────────────────────────────────────────────────────
// Draw helpers
// ─────────────────────────────────────────────────────────────────────────────
static void quad(float x, float y, float w, float h, RGB c) {
    glColor3f(c.r, c.g, c.b);
    glBegin(GL_QUADS);
      glVertex2f(x,   y);   glVertex2f(x+w, y);
      glVertex2f(x+w, y+h); glVertex2f(x,   y+h);
    glEnd();
}

// 5×7 bitmap font (top-to-bottom row bitmasks)
static const unsigned char FONT5x7[][7] = {
    {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}, // 0
    {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E}, // 1
    {0x0E,0x11,0x01,0x06,0x08,0x10,0x1F}, // 2
    {0x0E,0x11,0x01,0x06,0x01,0x11,0x0E}, // 3
    {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}, // 4
    {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E}, // 5
    {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E}, // 6
    {0x1F,0x01,0x02,0x04,0x08,0x08,0x08}, // 7
    {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}, // 8
    {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C}, // 9
    {0x04,0x0A,0x11,0x1F,0x11,0x11,0x11}, // A
    {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E}, // B
    {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E}, // C
    {0x1E,0x09,0x09,0x09,0x09,0x09,0x1E}, // D
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F}, // E
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10}, // F
    {0x0E,0x11,0x10,0x10,0x13,0x11,0x0F}, // G
    {0x11,0x11,0x11,0x1F,0x11,0x11,0x11}, // H
    {0x0E,0x04,0x04,0x04,0x04,0x04,0x0E}, // I
    {0x07,0x02,0x02,0x02,0x02,0x12,0x0C}, // J
    {0x11,0x12,0x14,0x18,0x14,0x12,0x11}, // K
    {0x10,0x10,0x10,0x10,0x10,0x10,0x1F}, // L
    {0x11,0x1B,0x15,0x11,0x11,0x11,0x11}, // M
    {0x11,0x19,0x15,0x13,0x11,0x11,0x11}, // N
    {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}, // O
    {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10}, // P
    {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D}, // Q
    {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11}, // R
    {0x0E,0x11,0x10,0x0E,0x01,0x11,0x0E}, // S
    {0x1F,0x04,0x04,0x04,0x04,0x04,0x04}, // T
    {0x11,0x11,0x11,0x11,0x11,0x11,0x0E}, // U
    {0x11,0x11,0x11,0x11,0x0A,0x0A,0x04}, // V
    {0x11,0x11,0x11,0x15,0x15,0x1B,0x11}, // W
    {0x11,0x11,0x0A,0x04,0x0A,0x11,0x11}, // X
    {0x11,0x11,0x0A,0x04,0x04,0x04,0x04}, // Y
    {0x1F,0x01,0x02,0x04,0x08,0x10,0x1F}, // Z
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // ' ' [36]
    {0x00,0x04,0x04,0x00,0x04,0x04,0x00}, // ':' [37]
    {0x00,0x00,0x00,0x00,0x00,0x04,0x04}, // '.' [38]
    {0x01,0x02,0x02,0x04,0x08,0x08,0x10}, // '/' [39]
    {0x00,0x00,0x00,0x1F,0x00,0x00,0x00}, // '-' [40]
};

static int font_idx(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return 10 + c - 'A';
    if (c >= 'a' && c <= 'z') return 10 + c - 'a';
    if (c == ':') return 37;
    if (c == '.') return 38;
    if (c == '/') return 39;
    if (c == '-') return 40;
    return 36; // space / unknown
}

static void draw_char(char c, float px, float py, float sz, RGB col) {
    int idx = font_idx(c);
    for (int row = 0; row < 7; ++row) {
        unsigned char bits = FONT5x7[idx][row];
        for (int bit = 0; bit < 5; ++bit)
            if (bits & (0x10 >> bit))
                quad(px + bit*sz, py - row*sz, sz*0.9f, sz*0.9f, col);
    }
}

// Width of a rendered string at size sz (each char occupies 6 * sz pixels)
static float text_w(const std::string& s, float sz) {
    return (float)s.size() * sz * 6.f;
}

static void draw_text(const std::string& s, float px, float py,
                      float sz, RGB col) {
    for (char c : s) { draw_char(c, px, py, sz, col); px += sz * 6.f; }
}

// Draw text horizontally centred on cx
static void draw_text_c(const std::string& s, float cx, float py,
                        float sz, RGB col) {
    draw_text(s, cx - text_w(s, sz) * 0.5f, py, sz, col);
}

// ─────────────────────────────────────────────────────────────────────────────
// Number → string helpers
// ─────────────────────────────────────────────────────────────────────────────
static std::string ii(long long v) {
    if (!v) return "0";
    bool neg = v < 0; if (neg) v = -v;
    std::string r;
    while (v) { r = char('0' + v % 10) + r; v /= 10; }
    return neg ? "-" + r : r;
}

static std::string ff(double v) {  // 1 decimal place
    int a = (int)v;
    int b = (int)(std::fabs(v - a) * 10.);
    return ii(a) + "." + ii(b);
}

// ─────────────────────────────────────────────────────────────────────────────
// Palette  (accent per algo; neutral shared colours for chrome)
// ─────────────────────────────────────────────────────────────────────────────
static const RGB ACCENT[NUM_PANELS] = {
    {0.98f, 0.78f, 0.18f},  // Bubble – amber
    {0.72f, 0.38f, 0.98f},  // Quick  – violet
};

static const RGB C_BG       = {0.09f, 0.09f, 0.11f};
static const RGB C_HDR      = {0.13f, 0.13f, 0.16f};
static const RGB C_FTR      = {0.11f, 0.11f, 0.14f};
static const RGB C_TOPBAR   = {0.07f, 0.07f, 0.09f};
static const RGB C_DIVIDER  = {0.22f, 0.22f, 0.27f};
static const RGB C_TEXT     = {0.82f, 0.82f, 0.82f};
static const RGB C_COLD     = {0.28f, 0.50f, 0.88f};  // unsorted bar
static const RGB C_HOT      = {0.95f, 0.25f, 0.22f};  // active bar
static const RGB C_SORTED   = {0.18f, 0.82f, 0.35f};  // sorted bar
static const RGB C_DARK     = {0.09f, 0.09f, 0.11f};  // text-bg overlay

// ─────────────────────────────────────────────────────────────────────────────
// Render one panel
//   sc  = win_h / 800 — universal scale factor so text/chrome adapts to size
// ─────────────────────────────────────────────────────────────────────────────
static void render_panel(int idx, float px, float pw,
                         float win_h, float sc, const Panel& p) {
    // Zone heights (scale with window height)
    float top_h = std::floor(32.f * sc);  // topbar  — NOT drawn here
    float hdr_h = std::floor(52.f * sc);  // header strip
    float ftr_h = std::floor(48.f * sc);  // footer strip

    float bar_y0 = ftr_h;
    float bar_y1 = win_h - top_h - hdr_h;
    float bar_ht = bar_y1 - bar_y0;

    // ── Background
    quad(px, 0, pw, win_h, C_BG);

    // ── Header  (neutral — algorithm identity comes only from accent colour)
    quad(px, bar_y1, pw, hdr_h, C_HDR);

    // 3-pixel accent border at top edge of header (just below topbar)
    float ab = std::max(2.f, 3.f * sc);
    quad(px, win_h - top_h - ab, pw, ab, ACCENT[idx]);

    // Algorithm name centred in header, in its accent colour
    static const char* NAMES[NUM_PANELS] = {
        "BUBBLE SORT", "QUICK SORT"
    };
    float tsz = 2.4f * sc;
    float hdr_mid_y = bar_y1 + hdr_h * 0.5f;
    draw_text_c(NAMES[idx], px + pw * 0.5f,
                hdr_mid_y + tsz * 3.5f, tsz, ACCENT[idx]);

    // ── Footer
    quad(px, 0, pw, ftr_h, C_FTR);

    float fsz = 1.9f * sc;
    float fy1 = ftr_h * 0.78f;   // STEPS row  (top of char = fy1)
    float fy2 = ftr_h * 0.42f;   // TIME  row

    draw_text("STEPS: " + ii(p.steps), px + 4.f*sc, fy1, fsz, C_TEXT);

    // Show per-algo finish time once done, otherwise live clock
    double show_t = (p.finished_s > 0.0) ? p.finished_s : p.elapsed_s;
    draw_text("TIME: " + ff(show_t) + "S", px + 4.f*sc, fy2, fsz, C_TEXT);

    if (p.done) {
        const std::string dn = "DONE";
        draw_text(dn, px + pw - text_w(dn, fsz) - 5.f*sc,
                  fy1, fsz, C_SORTED);
    }

    // ── Bars
    int n = (int)p.data.size();
    if (!n) return;
    int mx = *std::max_element(p.data.begin(), p.data.end());
    float bw = pw / (float)n;

    for (int i = 0; i < n; ++i) {
        float bh = mx > 0 ? (float(p.data[i]) / mx) * bar_ht : 0.f;
        float bx = px + i * bw;

        RGB col;
        if (p.srt[i]) {
            col = C_SORTED;
        } else if (i == p.ca || i == p.cb) {
            col = C_HOT;
        } else {
            // Heat-spotlight: bars near the active comparison glow warm
            float heat = 0.f;
            auto contrib = [&](int act) {
                if (act < 0) return;
                float d = (float)std::abs(i - act);
                float h = 1.f / (1.f + d * d * 0.12f);
                if (h > heat) heat = h;
            };
            contrib(p.ca);
            contrib(p.cb);
            heat = heat * heat * 0.55f;  // square + scale: tight, visible
            col = lerp_color(C_COLD, C_HOT, heat);
        }

        float gap = std::max(0.5f, bw * 0.12f);
        quad(bx + gap * 0.5f, bar_y0, bw - gap, bh, col);
    }

    // ── Completion annotation (appears once the algo finishes)
    if (p.done && p.finished_s > 0.0) {
        float mid_y = bar_y0 + bar_ht * 0.5f;
        float lsz   = 2.8f * sc;
        std::string label = "SORTED IN " + ff(p.finished_s) + "S";
        float lw = text_w(label, lsz);

        // Thin horizontal rule across the panel at mid-height
        quad(px + pw * 0.08f, mid_y, pw * 0.84f, std::max(1.f, sc), ACCENT[idx]);

        // Small opaque background so the text is readable over the bars
        quad(px + pw * 0.5f - lw * 0.5f - 5.f*sc,
             mid_y - lsz * 8.f,
             lw + 10.f*sc, lsz * 10.f, C_DARK);

        draw_text_c(label, px + pw * 0.5f,
                    mid_y + lsz * 3.5f, lsz, ACCENT[idx]);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Render topbar (drawn last so it sits above all panels)
// ─────────────────────────────────────────────────────────────────────────────
static void render_topbar(float ww, float wh, float sc,
                          double elapsed, bool all_done) {
    float h = std::floor(32.f * sc);
    quad(0, wh - h, ww, h, C_TOPBAR);

    float tsz = 2.2f * sc;
    float ty  = wh - h * 0.5f + tsz * 3.5f;

    std::string s = all_done
        ? "ALL ALGORITHMS COMPLETE  " + ff(elapsed) + "S TOTAL  -  ESC TO EXIT"
        : "ELAPSED  " + ff(elapsed) + "S";
    RGB tc = all_done ? RGB{0.95f, 0.90f, 0.65f} : C_TEXT;
    draw_text_c(s, ww * 0.5f, ty, tsz, tc);
}

// ─────────────────────────────────────────────────────────────────────────────
// Main
// ─────────────────────────────────────────────────────────────────────────────
int main() {
    // ── 1. Build random input array
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(5, NUM_BARS);
    std::vector<int> base(NUM_BARS);
    for (int& v : base) v = dist(rng);

    // ── 2. Pre-compute all steps
    g_steps[0] = compute_bubble(base);
    g_steps[1] = compute_quick (base);

    static const char* NAMES[NUM_PANELS] = {
        "Bubble Sort", "Quick Sort"
    };

    // ── 3. Assign natural durations (proportional to step count)
    //       Bubble Sort (most steps) anchors at BASE_SECS = 30 s.
    //       Other algorithms finish proportionally sooner.
    int max_s = 0;
    for (int i = 0; i < NUM_PANELS; ++i)
        max_s = std::max(max_s, (int)g_steps[i].size());

    for (int i = 0; i < NUM_PANELS; ++i) {
        g_panels[i].name = NAMES[i];
        g_panels[i].data = base;
        g_panels[i].srt.assign(NUM_BARS, false);
        double ratio = max_s > 0 ? (double)g_steps[i].size() / max_s : 1.0;
        g_panels[i].duration = std::max(MIN_SECS, BASE_SECS * ratio);
        printf("%-16s  %5d steps  ->  %.1fs\n",
               NAMES[i], (int)g_steps[i].size(), g_panels[i].duration);
    }

    // ── 4. Init GLFW
    glfwSetErrorCallback([](int e, const char* d) {
        fprintf(stderr, "GLFW %d: %s\n", e, d);
    });
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWwindow* win = glfwCreateWindow(
        WIN_W_INIT, WIN_H_INIT, "Bubble Sort vs Quick Sort", nullptr, nullptr);
    if (!win) { glfwTerminate(); return 1; }

    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);  // vsync
    glfwSetFramebufferSizeCallback(win, on_resize);

    // Initialise dimensions before the first frame
    glfwGetFramebufferSize(win, &g_fb_w, &g_fb_h);
    on_resize(win, g_fb_w, g_fb_h);

    // ── 5. Init GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "GLEW init failed\n");
        glfwTerminate(); return 1;
    }
    printf("GL %s | %s\n", glGetString(GL_VERSION), glGetString(GL_RENDERER));
    glClearColor(0.09f, 0.09f, 0.11f, 1.f);

    // ── 6. Main loop
    double t0 = glfwGetTime();

    while (!glfwWindowShouldClose(win)) {
        double now     = glfwGetTime();
        double elapsed = now - t0;

        // Live framebuffer dimensions (respects any resize that occurred)
        float ww = (float)g_fb_w;
        float wh = (float)g_fb_h;
        // Scale factor: design assumes 800 px tall; everything scales from that
        float sc = wh / 800.f;

        // ── Advance each panel at its own natural pace
        for (int pi = 0; pi < NUM_PANELS; ++pi) {
            Panel& p = g_panels[pi];
            if (p.done) continue;

            int ns = (int)g_steps[pi].size();
            if (!ns) { p.done = true; p.finished_s = elapsed; continue; }

            double frac = elapsed / p.duration;
            if (frac >= 1.0) {
                // Snap to fully sorted final state
                const Step& ls = g_steps[pi].back();
                p.data = ls.arr; p.srt = ls.srt;
                p.ca = -1; p.cb = -1;
                p.steps      = ns;
                p.done       = true;
                p.finished_s = elapsed;
            } else {
                int tgt = (int)(frac * ns);
                if (tgt >= ns) tgt = ns - 1;
                const Step& s = g_steps[pi][tgt];
                p.data      = s.arr;
                p.srt       = s.srt;
                p.ca        = s.a;
                p.cb        = s.b;
                p.steps     = (long long)(tgt + 1);
                p.elapsed_s = elapsed;
            }
        }

        bool all_done = true;
        for (int i = 0; i < NUM_PANELS; ++i)
            if (!g_panels[i].done) { all_done = false; break; }

        // ── Draw
        glClear(GL_COLOR_BUFFER_BIT);

        float pw = ww / (float)NUM_PANELS;
        for (int pi = 0; pi < NUM_PANELS; ++pi)
            render_panel(pi, pi * pw, pw, wh, sc, g_panels[pi]);

        // Dividers between panels
        for (int i = 1; i < NUM_PANELS; ++i)
            quad(i * pw - 1.f, 0, 2.f, wh, C_DIVIDER);

        // Topbar drawn last so it always sits on top of panel headers
        render_topbar(ww, wh, sc, elapsed, all_done);

        glfwSwapBuffers(win);
        glfwPollEvents();

        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(win, GLFW_TRUE);
    }

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
