# 🧠 GAROK - Game Reaksi Otak

> Sebuah permainan berbasis terminal yang menguji kemampuan berpikir dan reaksi Anda. Diberikan konteks, dapatkah Anda menebak kata yang tepat?

---

## 📖 Deskripsi

**GAROK** (Game Reaksi Otak) adalah permainan edukatif berbasis terminal yang dibangun menggunakan **ncurses**. Dalam permainan ini, pemain akan:
- Menerima berbagai **konteks atau deskripsi**
- **Menebak kata** yang paling sesuai dengan konteks tersebut
- Menguji **kecepatan berpikir** dan **pengetahuan** mereka

Permainan ini dirancang untuk melatih kognitif dan memberikan hiburan yang menyenangkan!

---

## ✨ Fitur

- ✅ Antarmuka berbasis terminal yang responsif (menggunakan ncurses)
- ✅ Sistem pertanyaan dinamis dengan berbagai tingkat kesulitan
- ✅ ASCII art yang menarik untuk pengalaman visual
- ✅ Efek audio untuk meningkatkan pengalaman bermain 
- ✅ Gameplay yang cepat dan adiktif

---

## 🚀 Cara Memainkan

1. **Jalankan Program**
   ```bash
   ./garok
   ```

2. **Baca Konteks**
   Perhatikan konteks atau deskripsi yang diberikan di layar

3. **Ketik Jawaban**
   Ketik kata yang Anda pikir adalah jawaban yang paling sesuai

4. **Lihat Skor**
   Dapatkan poin jika jawaban Anda benar!

5. **Ulangi**
   Mainkan terus untuk meningkatkan skor Anda

---

## 📋 Struktur Proyek

```
garok/
├── main.cpp                 # File utama program
├── garok                    # Executable
├── README.md               # File dokumentasi ini
└── assets/
    ├── ascii-art/          # Koleksi ASCII art
    │   ├── arisu.txt
    │   ├── chibi.txt
    │   ├── doodle.txt
    │   ├── dragon.txt
    │   └── garok.txt
    ├── audio/              # File audio (opsional)
    └── data/
        └── questions.txt   # Database pertanyaan
└── src/
    ├── anim.h             # Header animasi
    ├── ascii-art.hpp      # Header ASCII art
    ├── music.h            # Header musik
    ├── question.h         # Header sistem pertanyaan
    └── ui.h               # Header antarmuka pengguna
```

---

## 🛠️ Requirement

- **Compiler**: C++ (g++, clang++, atau sejenisnya)
- **Library**: ncurses
- **OS**: Linux/Unix (WSL juga didukung)

### Instalasi Dependency (Linux/Ubuntu)

```bash
sudo apt-get install libncurses-dev
```

---

## 🎮 Kontrol

- **Ketik Jawaban**: Masukkan teks jawaban Anda
- **Enter**: Konfirmasi jawaban
- **Ctrl+C**: Keluar dari permainan (di beberapa sistem)

---

## 🎨 Fitur Visual

Permainan ini dilengkapi dengan ASCII art yang menarik:
- Logo GAROK yang keren
- Karakter pendamping (Arisu, Chibi, Doodle, Dragon)
- Efek animasi untuk meningkatkan pengalaman pengguna

---

## 🔧 Cara Kompilasi

```bash
g++ -o garok main.cpp -lncurses
```

Atau gunakan Makefile jika tersedia:

```bash
make
```

---

## 📝 Format Pertanyaan

Pertanyaan disimpan dalam file `assets/data/questions.txt` dengan format:

```
Konteks/Deskripsi | Jawaban yang Benar
```

Contoh:
```
Hewan yang paling cepat di darat | Cheetah
Ibukota Indonesia | Jakarta
Bintang terbesar di tata surya kami | Matahari
```

---

## 🎯 Tips Bermain

1. **Fokus pada Konteks**: Baca dengan teliti apa yang diminta
2. **Pikirkan Cepat**: Reaksi cepat adalah kunci keberhasilan
3. **Ketahui Fakta**: Pengetahuan umum sangat membantu
4. **Jangan Ragu**: Percaya diri adalah awal kesuksesan
5. **Bermain Terus-Menerus**: Latihan membuat sempurna!

---

## 📜 Lisensi

Proyek ini dibuat untuk keperluan pembelajaran (UAS - Universitas).

---

## 👥 Developer

*Penulis*: hiroba, tety febian
*Repositori*: [Termul di GitHub](https://github.com/hikz/garok)

---

## 💡 Catatan

Untuk memainkan game ini, Anda harus menggunakan **100% otak Anda** untuk mencapai skor tertinggi! 🧠💪


Selamat bermain dan semoga bersenang-senang! 🎮✨
