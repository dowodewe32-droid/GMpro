#ifndef DEBUG_H
#define DEBUG_H

// 1. WAJIB DI LUAR: Semua include harus di atas, sebelum logika apa pun
#include "config.h"
#include <Arduino.h>

// 2. JANGAN pakai extern "C" karena Serial adalah fitur C++
// Ini yang bikin error "template with C linkage" di log lo.

#ifdef ENABLE_DEBUG
  #define debug(...) DEBUG_PORT.print(__VA_ARGS__)
  #define debugln(...) DEBUG_PORT.println(__VA_ARGS__)
  #define debugF(...) DEBUG_PORT.print(__VA_ARGS__)
  #define debuglnF(...) DEBUG_PORT.println(__VA_ARGS__)
#else
  #define debug(...)
  #define debugln(...)
  #define debugF(...)
  #define debuglnF(...)
#endif

#endif
