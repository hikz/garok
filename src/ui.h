#pragma once
using namespace std;

// simple word-wrap
vector<string> wrap_text(const string &text, int maxw)
{
    vector<string> out;
    if (maxw <= 0)
    {
        out.push_back(text);
        return out;
    }
    istringstream iss(text);
    string word, line;
    while (iss >> word)
    {
        if ((int)line.size() + (line.empty() ? 0 : 1) + (int)word.size() <= maxw)
        {
            if (!line.empty())
                line += ' ';
            line += word;
        }
        else
        {
            if (!line.empty())
            {
                out.push_back(line);
                line.clear();
            }
            // break very long words
            while ((int)word.size() > maxw)
            {
                out.push_back(word.substr(0, maxw));
                word = word.substr(maxw);
            }
            line = word;
        }
    }
    if (!line.empty())
        out.push_back(line);
    if (out.empty())
        out.push_back("");
    return out;
}

// draw one card window at (y,x) with provided width/height and text lines
void draw_card(int y, int x, int w, int h, const vector<string> &lines)
{
    if (w < 4 || h < 3)
        return; // too small
    WINDOW *winn = newwin(h, w, y, x);

    // interior background (pair 2)
    wbkgd(winn, COLOR_PAIR(2));

    // draw colored border: enable color pair 1 then call box
    wattron(winn, COLOR_PAIR(1));
    box(winn, 0, 0);
    wattroff(winn, COLOR_PAIR(1));

    // print text with padding
    int padx = 2;
    int pady = 1;
    int inner_w = w - padx * 2;
    int inner_h = h - pady * 2;
    for (int i = 0; i < (int)lines.size() && i < inner_h; ++i)
    {
        string t = lines[i];
        if ((int)t.size() > inner_w)
            t = t.substr(0, inner_w);
        mvwprintw(winn, pady + i, padx, "%s", t.c_str());
    }

    wrefresh(winn);
    // note: not deleting winn so it remains visible until endwin()
}

// small helper to compute size and draw card given paragraphs
void make_card_and_draw(int starty, int startx, int maxw, int maxh, const vector<string> &paragraphs)
{
    int padx = 2, pady = 1;
    int usable_w = max(1, maxw - 2 * padx);

    // wrap paragraphs
    vector<string> wrapped;
    for (auto &p : paragraphs)
    {
        auto wl = wrap_text(p, usable_w);
        for (auto &s : wl)
            wrapped.push_back(s);
        wrapped.push_back(""); // blank line between paragraphs
    }
    if (!wrapped.empty())
        wrapped.pop_back(); // remove trailing blank

    // measure needed width & height
    int longest = 0;
    for (auto &s : wrapped)
        longest = max<long long>(longest, s.size());
    int need_w = max(12, longest + 2 * padx);
    int need_h = max(3, (int)wrapped.size() + 2 * pady);

    int final_w = min(need_w, maxw);
    int final_h = min(need_h, maxh);

    // rewrap if clamped horizontally
    int final_inner_w = max(1, final_w - 2 * padx);
    vector<string> final_wrapped;
    for (auto &p : paragraphs)
    {
        auto wl = wrap_text(p, final_inner_w);
        for (auto &s : wl)
            final_wrapped.push_back(s);
        final_wrapped.push_back("");
    }
    if (!final_wrapped.empty())
        final_wrapped.pop_back();

    draw_card(starty, startx, final_w, final_h, final_wrapped);
}

// fungsi helper sederhana
string trim(const string &s)
{
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos)
        return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

string toLower(const string &s)
{
    string r = s;
    for (size_t i = 0; i < r.size(); ++i)
        r[i] = (char)tolower((unsigned char)r[i]);
    return r;
}