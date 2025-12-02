// ui.h
#pragma once
#include <ncurses.h>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

// Wrap a single long string into multiple lines not exceeding max_width
inline vector<string> wrap_text(const string &text, int max_width) {
    vector<string> lines;
    if (max_width <= 0) return {text};

    istringstream iss(text);
    string word;
    string line;
    while (iss >> word) {
        if ((int)line.size() + (line.empty() ? 0 : 1) + (int)word.size() <= max_width) {
            if (!line.empty()) line += ' ';
            line += word;
        } else {
            if (!line.empty()) {
                lines.push_back(line);
                line.clear();
            }
            // if single word is longer than max_width, break the word
            while ((int)word.size() > max_width) {
                lines.push_back(word.substr(0, max_width));
                word = word.substr(max_width);
            }
            line = word;
        }
    }
    if (!line.empty()) lines.push_back(line);
    if (lines.empty()) lines.push_back("");
    return lines;
}

// Draw a card (border + text centered with padding). The card size is specified.
inline void draw_card(int starty, int startx, int width, int height, const vector<string> &text_lines) {
    WINDOW *w = newwin(height, width, starty, startx);

    // Color pair 1 assumed to be red-on-black for border, 2 white-on-black for text
    wattron(w, COLOR_PAIR(1)); // for border
    // draw border with colored attribute
    box(w, 0, 0);
    wattroff(w, COLOR_PAIR(1));

    // fill inside with background (black)
    wbkgd(w, COLOR_PAIR(2));

    // print text with left padding = 2, top padding = 1
    int padx = 2;
    int pady = 1;
    int max_inner_w = width - 2 * padx;
    int max_inner_h = height - 2 * pady;

    // Only print as many lines as fit
    int lines_to_print = min((int)text_lines.size(), max_inner_h);
    for (int i = 0; i < lines_to_print; ++i) {
        // truncate if needed
        string t = text_lines[i];
        if ((int)t.size() > max_inner_w) t = t.substr(0, max_inner_w);
        wattron(w, COLOR_PAIR(2));
        mvwprintw(w, pady + i, padx, "%s", t.c_str());
        wattroff(w, COLOR_PAIR(2));
    }

    wrefresh(w);
    // keep a reference to the window alive until end of program (simple example)
    // If you want to delete: delwin(w);
}

// Given a block of raw paragraphs (vector<string>), compute the card width & height needed.
// max_width and max_height are the maximum allowed (e.g. column width / available area).
inline void compute_card_size_and_draw(int starty, int startx, int max_width, int max_height,
                                      const vector<string> &paragraphs) {
    // measure the longest wrapped line and total wrapped lines
    int padx = 2; // left/right padding inside card
    int pady = 1; // top/bottom padding inside card

    int usable_text_w = max_width - 2*padx;
    if (usable_text_w < 10) usable_text_w = max(1, max_width - 2*padx); // minimal fallback

    vector<string> wrapped;
    for (auto &p : paragraphs) {
        auto lines = wrap_text(p, usable_text_w);
        for (auto &l : lines) wrapped.push_back(l);
        // add an empty line after each paragraph (optional)
        wrapped.push_back("");
    }
    if (!wrapped.empty() && wrapped.back().empty()) wrapped.pop_back(); // remove last blank

    // compute needed width (longest line)
    int longest = 0;
    for (auto &l : wrapped) longest = max<long long>(longest, l.size());
    int needed_width = longest + 2*padx;
    int needed_height = (int)wrapped.size() + 2*pady;

    // enforce at least some reasonable size
    needed_width = max(needed_width, 12);
    needed_height = max(needed_height, 3);

    // clamp to max_width/max_height
    int final_w = min(needed_width, max_width);
    int final_h = min(needed_height, max_height);

    // If wrapping changed because we clamped width, re-wrap with final inner width
    int final_inner_w = final_w - 2*padx;
    vector<string> final_wrapped;
    for (auto &p : paragraphs) {
        auto lines = wrap_text(p, final_inner_w);
        for (auto &l : lines) final_wrapped.push_back(l);
        final_wrapped.push_back("");
    }
    if (!final_wrapped.empty() && final_wrapped.back().empty()) final_wrapped.pop_back();

    draw_card(starty, startx, final_w, final_h, final_wrapped);
}
