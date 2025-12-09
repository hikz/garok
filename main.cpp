#include "src/question.h"
#include "src/ui.h"
#include "src/ascii-art.hpp"
#include <iostream>
using namespace std;

int main() {

    string filename = "assets/data/questions.txt"; // lokasi file soal
    
    vector<Question> questions = loadQuestions(filename);
    runQuizUI(questions, filename);

    // Simpan lagi saat keluar (best-effort)
    if (!saveQuestions(filename, questions)) {
        cerr << "Gagal menyimpan ke " << filename << "\n";
    } else {
        cout << "Perubahan disimpan ke " << filename << "\n";
    }
    return 0;
}
