#ifndef DEBUG_H
#define DEBUG_H

#include "config.h"
#include <Arduino.h>

// Kita hapus extern "C" karena Serial adalah C++
// Ini bakal ngilangin error "template with C linkage" selamanya

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
