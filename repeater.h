#ifndef REPEATER_H
#define REPEATER_H

#include <Arduino.h>

namespace repeater {
    // Fungsi untuk menyimpan kredensial repeater
    void connect(String ssid, String pass);
    
    // Fungsi utama untuk menjalankan mode NAPT/Repeater
    void run();
    
    // Mengecek apakah mode repeater aktif
    bool status();
    
    // Mengubah status aktif/non-aktif repeater
    void update_status(bool s);
}

#endif
