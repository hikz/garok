#include "src/question.h"
#include "assets/art/ascii-art.h"
#include <iostream>
using namespace std;

int main(int argc, char** argv) {
    string filename = "assets/data/questions.txt";
    if (argc >= 2) filename = argv[1];

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
