#pragma once
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

vector<string> loadAsciiArts(
    const string &filename,
    int requiredBlankLines = 3,
    bool treatWhitespaceAsBlank = true,
    const string &separator = ""
) {
    ifstream file(filename, ios::in);
    vector<string> frames;
    if (!file.is_open()) {
        cerr << "Gagal membuka file: " << filename << "\n";
        return frames;
    }

    string line;
    stringstream block;
    int blankCount = 0;

    auto is_blank_line = [&](const string &s)->bool {
        if (s.empty()) return true;
        if (!treatWhitespaceAsBlank) return false;
        for (char c : s) if (!isspace(static_cast<unsigned char>(c))) return false;
        return true;
    };

    while (getline(file, line)) {
        // Hapus carriage return jika ada (CRLF -> LF)
        if (!line.empty() && line.back() == '\r') line.pop_back();

        // Jika ada separator yang ditentukan, prioritaskan itu
        if (!separator.empty()) {
            // Jika line sama persis dengan separator -> finalize frame
            if (line == separator) {
                string art = block.str();
                if (!art.empty()) {
                    // hapus newline terakhir jika mau (opsional). Saat ini tetap dipertahankan.
                    frames.push_back(art);
                }
                block.str("");
                block.clear();
                blankCount = 0;
                continue;
            }
            // bukan separator -> tambahkan ke block
            block << line << "\n";
            continue;
        }

        // Tanpa separator: gunakan pola baris kosong berturut-turut
        if (is_blank_line(line)) {
            blankCount++;
        } else {
            blankCount = 0;
        }

        if (blankCount >= requiredBlankLines) {
            string art = block.str();
            if (!art.empty()) {
                frames.push_back(art);
            }
            block.str("");
            block.clear();
            blankCount = 0;
            continue;
        }

        // normal non-separator line (atau blank lines yang belum mencapai threshold)
        block << line << "\n";
    }

    // simpan frame terakhir bila ada
    string last = block.str();
    if (!last.empty()) {
        frames.push_back(last);
    }

    return frames;
}


inline vector<string> doodle = loadAsciiArts("assets/ascii-art/doodle.txt");
inline vector<string> dragon = loadAsciiArts("assets/ascii-art/dragon.txt");
inline vector<string> arisu = loadAsciiArts("assets/ascii-art/arisu.txt");
inline vector<string> chibi = loadAsciiArts("assets/ascii-art/chibi.txt", 3, false);

inline vector<string> garok = loadAsciiArts("assets/ascii-art/garok.txt");