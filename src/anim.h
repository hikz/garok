#include <ncurses.h>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iostream>


using namespace std;

// --- helper: split multiline frame into lines ---
static vector<string> split_lines(const string &s) {
    vector<string> out;
    string cur;
    for (char c : s) {
        if (c == '\r') continue;
        if (c == '\n') {
            out.push_back(cur);
            cur.clear();
        } else cur.push_back(c);
    }
    out.push_back(cur);
    return out;
}


void play_frames(vector<string> frames, int frame_count,
                 int starty, int startx, int maxw, int maxh,
                 int fps = 12, bool loop = true)
{
    if (frame_count <= 0) return;
    if (fps <= 0) fps = 12;
    int delay_ms = 1000 / fps;

    // non-blocking input so user dapat menekan 'q' untuk stop
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);

    bool running = true;
    while (running) {
        for (int i = 0; i < frame_count; ++i) {
            // cek input untuk stop
            int ch = getch();
            if (ch == 32) { // ada input
                running = false;
                break;
            }
            // determine target size/pos
            int rows, cols;
            getmaxyx(stdscr, rows, cols);

            int w = maxw;
            int h = maxh;
            if (w <= 0) w = cols;
            if (h <= 0) h = rows;

            int y = starty;
            int x = startx;
            if (y == -1 && x == -1) {
                // center
                y = max(0, (rows - h) / 2);
                x = max(0, (cols - w) / 2);
            } else {
                // clamp so tidak keluar layar
                if (y < 0) y = 0;
                if (x < 0) x = 0;
                if (y + h > rows) { if (h > rows) { h = rows; y = 0; } else y = rows - h; }
                if (x + w > cols) { if (w > cols) { w = cols; x = 0; } else x = cols - w; }
            }

            // split frame into lines
            vector<string> lines = split_lines(frames[i]);

            // create a temporary window for the frame (so we can box it, etc.)
            // if you prefer to draw directly on stdscr, you can skip creating window
            WINDOW *fw = newwin(h, w, y, x);
            if (!fw) {
                // fallback: draw directly on stdscr
                werase(stdscr);
                int padx = 1, pady = 0;
                int inner_w = w - 2*padx;
                int inner_h = h - 2*pady;
                for (int r = 0; r < (int)lines.size() && r < inner_h; ++r) {
                    string ln = lines[r];
                    if ((int)ln.size() > inner_w) ln = ln.substr(0, inner_w);
                    mvprintw(y + pady + r, x + padx, "%s", ln.c_str());
                }
                refresh();
            } else {
                // optional: fill background & border
                // interior color (pair 2) assumed initialized by caller
                wbkgd(fw, COLOR_PAIR(2));
                // draw border (color pair 1 assumed)
                wattron(fw, COLOR_PAIR(1));
                box(fw, 0, 0);
                wattroff(fw, COLOR_PAIR(1));

                // compute inner area (respect a small padding)
                int padx = 1;
                int pady = 0;
                int inner_w = w - 2*padx;
                int inner_h = h - 2*pady;

                // print lines, clipping if needed
                for (int r = 0; r < (int)lines.size() && r < inner_h; ++r) {
                    string ln = lines[r];
                    if ((int)ln.size() > inner_w) ln = ln.substr(0, inner_w);
                    mvwprintw(fw, pady + r, padx, "%s", ln.c_str());
                }

                wrefresh(fw);
                // destroy the window so next frame can redraw (keeps memory clean)
                delwin(fw);
            }

            // sleep for the frame duration, but still respond to keypresses
            int slept = 0;
            while (slept < delay_ms) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                slept += 10;
                int c2 = getch();
                if (c2 == 32) { 
                    running = false; 
                    clear();
                    refresh();
                    break; 
                }
            }
            if (!running) break;
        } // end frames loop

        
        if (!loop) break;
    } // end running loop

    // restore blocking input
    nodelay(stdscr, FALSE);
}


