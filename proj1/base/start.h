#ifndef start_H
#define start_H
#define LOG(argument) std::cout << argument << '\n'

#include "raylib.h"
#include <stdio.h>

enum AppStatus { TERMINATED, RUNNING };

Color ColorFromHex(const char *hex);

#endif // start_H