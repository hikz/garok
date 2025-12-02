// question.h
#ifndef QUESTION_H
#define QUESTION_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <ncurses.h>

using namespace std; // diminta: supaya kode lebih mudah dibaca

struct Question
{
    string text;
    vector<string> answers; // jawaban yang diterima (sudah lowercase)
    bool done;
};

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

// Membaca file questions.txt dengan format:
// Q: soal
// A: jawaban1|jawaban2
// DONE:0
// ---  (pembatas)
vector<Question> loadQuestions(const string &filename)
{
    vector<Question> out;
    ifstream ifs(filename);
    if (!ifs)
        return out; // kembali kosong jika file tidak ada

    string line;
    Question cur;
    bool inBlock = false;
    while (getline(ifs, line))
    {
        string t = trim(line);
        if (t == "---")
        {
            if (inBlock)
            {
                out.push_back(cur);
                cur = Question();
                inBlock = false;
            }
            continue;
        }
        if (t.rfind("Q:", 0) == 0)
        {
            cur.text = trim(t.substr(2));
            cur.answers.clear();
            cur.done = false;
            inBlock = true;
        }
        else if (t.rfind("A:", 0) == 0)
        {
            string a = trim(t.substr(2));
            stringstream ss(a);
            string token;
            while (getline(ss, token, '|'))
            {
                token = trim(token);
                if (!token.empty())
                    cur.answers.push_back(toLower(token));
            }
        }
        else if (t.rfind("DONE:", 0) == 0)
        {
            string v = trim(t.substr(5));
            cur.done = (v == "1");
            inBlock = true;
        }
        else
        {
            // abaikan baris yang tidak dikenal
        }
    }
    if (inBlock)
        out.push_back(cur);
    return out;
}

// Menyimpan kembali file dengan format yang sama
bool saveQuestions(const string &filename, const vector<Question> &questions)
{
    ofstream ofs(filename);
    if (!ofs)
        return false;
    for (size_t i = 0; i < questions.size(); ++i)
    {
        ofs << "Q: " << questions[i].text << "\n";
        // gabungkan jawaban dengan |
        for (size_t j = 0; j < questions[i].answers.size(); ++j)
        {
            if (j)
                ofs << "|";
            ofs << questions[i].answers[j];
        }
        ofs << "\n";
        ofs << "DONE:" << (questions[i].done ? "1" : "0") << "\n";
        if (i + 1 < questions.size())
            ofs << "---\n";
    }
    return true;
}

// fungsi input sederhana (menggunakan ncurses)
string getInput(int starty, int startx, int width)
{
    echo();
    char buf[512];
    mvgetnstr(starty, startx, buf, width - 1);
    noecho();
    return string(buf);
}

bool checkAnswer(const Question &q, const string &given)
{
    string g = toLower(trim(given));
    for (size_t i = 0; i < q.answers.size(); ++i)
    {
        if (g == q.answers[i])
            return true;
    }
    return false;
}

// UI utama berbasis ncurses
// Navigasi: n (next), p (prev), a (answer), m (toggle mark), s (save), q (quit)
void runQuizUI(vector<Question> &questions, const string &filename)
{
    // init
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    if (!has_colors())
    {
        endwin();
        printf("Terminal does not support colors\n");
        return;
    }
    start_color();
    use_default_colors();
    // pair 1: red border on default bg; pair 2: white text on black background
    init_pair(1, COLOR_RED, -1);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);

    // clear stdscr background black so cards contrast
    bkgd(COLOR_PAIR(2));
    clear();
    refresh();

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    // layout params
    // margin 0 agar betul-betul fullscreen
    int margin = 0;
    int gap = 1;

    // bagi dua kolom
    int left_w = cols * 2 / 3;
    int right_w = cols - left_w - gap;

    // tinggi kiri: 3/4 dan 1/4
    int left_top_h = rows * 3 / 4;
    int left_bottom_h = rows - left_top_h - gap;

    // kanan pakai full tinggi
    int right_h = rows;

    int right_top_h = rows * 3 / 4;
    int right_bottom_h = rows - right_top_h - gap;

    int n = (int)questions.size();
    if (n == 0)
    {
        // mvprintw(1, 2, "Tidak ada soal. Pastikan file soal ada dan format benar.");
        // mvprintw(3, 2, "Tekan tombol apapun untuk keluar.");
        // getch();
        // endwin();
        // return;

        //  contents
        vector<string> leftTop = {
            "Tidak ada yang bisa ditampilkan",
            "Kemungkinan file soal tidak ditemukan atau kosong.",
            "Pastikan file soal ada di: ./assets/data/questions.txt",
        };
        vector<string> leftBottom = {
            "Tekan tombol apa saja untuk keluar dari game",
            "Terima kasih telah bermain game ini"};
        vector<string> rightCol = {
            "lorem ipsum dolor sit amet, consectetur adipiscing elit.",
            "Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.",
        };

        // ---- gambar kartu ----
        make_card_and_draw(0, 0, left_w, left_top_h, leftTop);
        make_card_and_draw(left_top_h + gap, 0, left_w, left_bottom_h, leftBottom);
        make_card_and_draw(0, left_w + gap, right_w, right_h, rightCol);

        refresh();

        getch();
        endwin();
        return;
    }

    int idx = 0;
    bool quit = false;
    while (!quit)
    {
        // clear();
        int row = 1;
        // mvprintw(row++, 2, "Soal %d / %d", idx + 1, n);
        // mvprintw(row++, 2, "Status: %s", questions[idx].done ? "Sudah dijawab" : "Belum dijawab");
        // row++;
        // mvprintw(row++, 2, "%s", questions[idx].text.c_str());
        // row++;
        // mvprintw(row++, 2, "Perintah: (n)ext  (p)rev  (a)nswer  (m)ark toggle  (s)ave  (q)uit");
        // row++;
        // mvprintw(row++, 2, "Masukkan pilihan: ");

        //  contents
        vector<string> leftTop = {
            "Soal saat ini: " + to_string(idx + 1) + " / " + to_string(n),
            "Status: " + string(questions[idx].done ? "Sudah dijawab" : "Belum dijawab / Jawaban belum benar"),
            "\n",
            questions[idx].text};
        vector<string> leftBottom = {
            "(n)ext (>) | (p)rev (<) | (a)nswer | (j)ump | (r)eset | (s)ave | (q)uit",
            "",
            ""};
        vector<string> rightCol = {
            "lorem ipsum dolor sit amet, consectetur adipiscing elit.",
            "Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.",
        };

        // ---- gambar kartu ----
        make_card_and_draw(0, 0, left_w, left_top_h, leftTop);
        make_card_and_draw(left_top_h + gap, 0, left_w, left_bottom_h, leftBottom);
        make_card_and_draw(0, left_w + gap, right_w, right_h, rightCol);

        refresh();
        int ch = getch();
        if (ch == 'n' || ch == KEY_RIGHT)
        {
            idx = (idx + 1) % n;

            leftTop = {
                "Soal saat ini: " + to_string(idx + 1) + " / " + to_string(n),
                "Status: " + string(questions[idx].done ? "Sudah dijawab" : "Belum dijawab / Jawaban belum benar"),
                "\n",
                questions[idx].text};
            clear();
            refresh();
        }
        else if (ch == 'p' || ch == KEY_LEFT)
        {
            idx = (idx - 1 + n) % n;
            
            leftTop = {
                "Soal saat ini: " + to_string(idx + 1) + " / " + to_string(n),
                "Status: " + string(questions[idx].done ? "Sudah dijawab" : "Belum dijawab / Jawaban belum benar"),
                "\n",
                questions[idx].text};
            clear();
            refresh();
        }
        else if (ch == 'j' || ch == KEY_LEFT)
        {
           // loncat ke soal tertentu
        }
        else if (ch == 'a')
        {
            mvprintw(left_top_h + gap + 2, 2, "Ketik jawaban: ");

            curs_set(1);
            string user = getInput(left_top_h + gap + 2, 18, 60);
            curs_set(0);
            bool ok = checkAnswer(questions[idx], user);
            if (ok)
            {   
                clear();
                questions[idx].done = true;
                refresh();
                mvprintw(left_top_h + gap + 3, 2, "Benar! (tekan apapun)");
            }
            else
            {
                mvprintw(left_top_h + gap + 3, 2, "Jawaban Anda SALAH, coba lagi :) ");

                mvprintw(left_top_h + gap + 4, 2, "Tekan apapun...");
            }
            getch();
        }
        else if (ch == 'r')
        {
            // reset data
        }
        else if (ch == 's')
        {
            saveQuestions(filename, questions);
            mvprintw(row++, 2, "Tersimpan ke file. Tekan apapun...");
            getch();
        }
        else if (ch == 'q')
        {
            mvprintw(row++, 2, "Simpan sebelum keluar? (y/n) ");
            int c2 = getch();
            if (c2 == 'y' || c2 == 'Y')
            {
                saveQuestions(filename, questions);
            }
            quit = true;
        }
    }

    endwin();
}

#endif // QUESTION_H
