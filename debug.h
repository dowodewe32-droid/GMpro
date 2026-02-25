#ifndef DEBUG_H
#define DEBUG_H

// 1. Load config dan Arduino di LUAR blok extern "C"
#include "config.h"
#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif
    // Kosongkan atau isi hanya dengan fungsi C murni
#ifdef __cplusplus
}
#endif

// 2. Definisikan macro debug dengan benar agar dikenali di .cpp
#ifdef ENABLE_DEBUG
    #define debugF(...) DEBUG_PORT.print(__VA_ARGS__)
    #define debuglnF(...) DEBUG_PORT.println(__VA_ARGS__)
    #define debug(...) DEBUG_PORT.print(__VA_ARGS__)
    #define debugln(...) DEBUG_PORT.println(__VA_ARGS__)
#else
    #define debugF(...)
    #define debuglnF(...)
    #define debug(...)
    #define debugln(...)
#endif

#endif
