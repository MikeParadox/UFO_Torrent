#include "ufo_torrent.h"

#include <iostream>
#include <ncurses.h>
#include <string>
#include <fstream>

#include "../includes/prettyPrinter.h"
#include "../includes/decode.h"
#include "../includes/torrentFile.h"
#include "../includes/encode.h"
#include <filesystem>
#include <string.h>
#include "./bencode/decode.h"
#include "bencode/prettyPrinter.h"
#include "ncurses_utils.h"

using namespace bencode;
using namespace Torrent;
#define WIDTH 30
#define HEIGHT 10 
#include <vector>
#include <string>

int main() 
{
    // Инициализация ncurses
    initscr();
    noecho();
    cbreak();
    curs_set(0);

    // Опции меню
    std::vector<std::string> menu_items = { "Scan file", "exit" };
    size_t highlight = 0;
    size_t choice = 0;

    // Создаем окно для меню
    int menu_height = menu_items.size() + 2;
    int menu_width = 40, menu_start_y = 4, menu_start_x = 10;
    WINDOW* menu_win = newwin(menu_height, menu_width, menu_start_y, menu_start_x);
    keypad(menu_win, true);

    // Основной цикл программы
    while (true) 
    {
        // Отображаем меню
        display_menu(menu_win, menu_items, highlight);

        int ch = wgetch(menu_win);
        switch (ch) 
        {
        case KEY_UP:
            if (highlight > 0) --highlight;
            break;
        case KEY_DOWN:
            if (highlight < menu_items.size()) ++highlight;
            break;
        case '\n':  // Enter
            choice = highlight + 1;
            break;
        default:
            break;
        }
        // Обработка выбора
        if (choice != 0) 
        {
            if (choice == 1) 
            {  // Первый пункт: ввод пути к файлу
                // Создаем окно для ввода пути
                int input_height = 8, input_width = 50;
                int input_start_y = menu_start_y + menu_height + 2, input_start_x = 10;
                WINDOW* input_win = newwin(input_height, input_width, input_start_y, input_start_x);
                box(input_win, 0, 0);
                wrefresh(input_win);

    // Опции меню
    std::vector<std::string> menu_items = { "Scan file", "exit" };
    size_t highlight = 0;
    size_t choice = 0;

    // Создаем окно для меню
    int menu_height = menu_items.size() + 2;
    int menu_width = 40, menu_start_y = 4, menu_start_x = 10;
    WINDOW* menu_win = newwin(menu_height, menu_width, menu_start_y, menu_start_x);
    keypad(menu_win, true);

    // Основной цикл программы
    while (true) 
    {
        // Отображаем меню
        display_menu(menu_win, menu_items, highlight);

        int ch = wgetch(menu_win);
        switch (ch) 
        {
        case KEY_UP:
            if (highlight > 0) --highlight;
            break;
        case KEY_DOWN:
            if (highlight < menu_items.size()) ++highlight;
            break;
        case '\n':  // Enter
            choice = highlight + 1;
            break;
        default:
            break;
        }
        // Обработка выбора
        if (choice != 0) 
        {
            if (choice == 1) 
            {  // Первый пункт: ввод пути к файлу
                // Создаем окно для ввода пути
                int input_height = 8, input_width = 50;
                int input_start_y = menu_start_y + menu_height + 2, input_start_x = 10;
                WINDOW* input_win = newwin(input_height, input_width, input_start_y, input_start_x);
                box(input_win, 0, 0);
                wrefresh(input_win);

                std::string path = inputFilePath(input_win);  // Вызываем функцию ввода пути

    boost::apply_visitor(PrettyPrinter(), valueFile);
	return 0;
                werase(input_win);
                box(input_win, 0, 0);
                mvwprintw(input_win, 1, 1, "Your path:");
                mvwprintw(input_win, 2, 1, "%s", path.c_str());
                mvwprintw(input_win, 3, 1, "Press any button");
                wrefresh(input_win);
                wgetch(input_win);
                delwin(input_win);  // Удаляем окно после использования
            }
            else if (choice == 2) 
            {  // Второй пункт: выход
                break;
            }
            choice = 0;  // Сбрасываем выбор
        }
    }

    // Завершаем работу ncurses
    endwin();
    return 0;
}