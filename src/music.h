#pragma once
#include <string>
#include <cstdlib>
#include <unistd.h> 

using namespace std;

static bool isPlaying = false;

// menjalankan vlc untuk memutar musik di latar belakang
void play_music(const string &windows_path) {
    if (isPlaying) {
        // Music already playing
        return;
    }

    string vlc_path = "C:\\\\Program Files\\\\\\VideoLAN\\\\\\VLC\\\\\\vlc.exe";

   
    string cmd =
        "powershell.exe -Command \"Start-Process -FilePath '"
        + vlc_path +
        "' -ArgumentList '"
        + windows_path +
        "','--intf','dummy','--no-video','--loop','--quiet' -WindowStyle Hidden\"";

    int r = system(cmd.c_str());
    if (r != -1) {
        isPlaying = true;
    } 
}

// Hentikan semua proses vlc (aman jika tidak ada)
void stop_music() {
    if (!isPlaying) {
        // No music is playing (flag false)
        return;
    }

    // menytop semua proses vlc
    string cmd = "powershell.exe -Command \"Stop-Process -Name 'vlc' -Force -ErrorAction SilentlyContinue\"";

    system(cmd.c_str());

    isPlaying = false; // stopped music
    
}


