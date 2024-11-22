#ifndef NCURSES_UTILS_H
#define NCURSES_UTILS_H
#include <ncurses.h>
#include <vector>
#include <string>

void display_menu(WINDOW* menu_win, const std::vector<std::string>& menu_items, size_t highlight);

std::string inputFilePath(WINDOW* input_win);

#endif