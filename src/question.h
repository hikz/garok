#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <ncurses.h>
#include "ui.h"
#include "anim.h"
//#include "../assets/art/frames.h"
#include "music.h"

#include "ascii-art.hpp"

using namespace std; // diminta: supaya kode lebih mudah dibaca


struct Question
{
    string text;
    vector<string> answers; // jawaban yang diterima (sudah lowercase)
    bool done;
};

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

void selamat() {
    srand(time(NULL)); // biar bener bener acak
    int angka = rand() % 4 + 1; // hasil 1-4

    switch (angka) {
        case 1:
            play_music("assets/audio/chibi.wav");
            play_frames(chibi, chibi.size(), 7, 30, 120, 70, 20, true);
            break;

        case 2:
            play_music("assets/audio/arisu.wav");
            play_frames(arisu, arisu.size(), 7, 30, 120, 70, 20, true);
            break;

        case 3:
            play_music("assets/audio/doodle.wav");
            play_frames(doodle, doodle.size(), 7, 30, 120, 70, 20, true);
            break;

        case 4:
            play_music("assets/audio/dragon.wav");
            play_frames(dragon, dragon.size(), 7, 30, 120, 70, 20, true);
            break;
    }
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
            ofs <<"A: " << questions[i].answers[j];
        }
        ofs << "\n";
        ofs << "DONE:" << (questions[i].done ? "1" : "0") << "\n";
        if (i + 1 < questions.size())
            ofs << "---\n";
    }
    return true;
}

void reset(vector<Question> &questions)
{
    for (auto &q : questions) {
        q.done = false;
    }
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


void runQuizUI(vector<Question> &questions, const string &filename)
{
    // init
    initscr();
    cbreak(); 
    noecho(); // jangan tampilkan input user
    keypad(stdscr, TRUE); // enable arrow keys
    curs_set(0); // sembunyikan kursor

    if (!has_colors()) // cek dukungan warna
    {
        endwin();
        printf("Terminal does not support colors\n");
        return;
    }

    start_color();
    use_default_colors();
    // pair 1: cyan border on default bg; 
    // pair 2: white text on black background
    init_pair(1, COLOR_YELLOW, -1);
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
    if (n == 0) // keluar jika tidak ada soal
    {
    
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
            "Selamat Datang di Garok Player ;)",
            "Garok adalah game kuis interaktif berbasis terminal yang menantang Kamu untuk menjawab berbagai pertanyaan menarik.",
            "Pastikan Kamu menggunakan 100% kemampuan otakmu untuk menjawab setiap pertanyaan yang ada. :0",
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
        int row = 1;

        //  contents
        vector<string> leftTop = {
            "Soal saat ini: " + to_string(idx + 1) + " / " + to_string(n),
            "Status: " + string(questions[idx].done ? "Sudah dijawab" : "Belum dijawab / Jawaban belum benar"),
            "\n",
            questions[idx].text};
        vector<string> leftBottom = {
            "(n)ext (>) | (p)rev (<) | (a)nswer | (r)eset | (s)ave | (q)uit",
            "",
            ""};
        vector<string> rightCol = {
            "Selamat Datang di Garok Player ;)",
            "Garok adalah game kuis interaktif berbasis terminal yang menantang Kamu untuk menjawab berbagai pertanyaan menarik.",
            "Pastikan Kamu menggunakan 100% kemampuan otakmu untuk menjawab setiap pertanyaan yang ada. :0", 

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
        else if (ch == 'a')
        {
            mvprintw(left_top_h + gap + 2, 2, "Ketik jawaban: ");

            curs_set(1); // munculkan kursor
            string user = getInput(left_top_h + gap + 2, 18, 60);
            curs_set(0); // sembunyikan kursor
            bool ok = checkAnswer(questions[idx], user);
            if (ok)
            {
                clear();
                questions[idx].done = true;
                refresh();
                mvprintw(left_top_h + gap, 2, "Selamaatttt!!!");
                mvprintw(left_top_h + gap + 1, 2, "Jawabanmu Benar!  >_<");
                mvprintw(left_top_h + gap + 3, 2, "tekan space....");
                selamat();
                stop_music();
                mvprintw(left_top_h + gap + 1, 2, "Ayo Kerjakan soal yang lain!  >_< ");
                mvprintw(left_top_h + gap + 2, 2, "Tekan apapun...");

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
            
            mvprintw(left_top_h + gap + 2, 2, "Yakin ingin melakukan reset? (y/n) ");
            int c2 = getch();
            if (c2 == 'y' || c2 == 'Y')
            {
                reset(questions);
                mvprintw(left_top_h + gap + 3, 2, "Berhasil melakukan reset soal :v ");
                mvprintw(left_top_h + gap + 4, 2, "Tekan apapun...");
            }
        }
        else if (ch == 's')
        {
            saveQuestions(filename, questions);
            mvprintw(left_top_h + gap + 2, 2, "Tersimpan ke file. Tekan apapun...");
            getch();
        }
        else if (ch == 'q')
        {
            mvprintw(left_top_h + gap + 2, 2, "Simpan sebelum keluar? (y/n) ");
            int c2 = getch();
            if (c2 == 'y' || c2 == 'Y')
            {
                saveQuestions(filename, questions);
                quit = true;
            }
            
        }
    }

    endwin();
}


