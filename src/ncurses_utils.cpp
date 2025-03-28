#include "ncurses_utils.h"
#include <ncurses.h>
#include <vector>
#include <string>
#include <torrentFile.h>

void display_menu(WINDOW* menu_win, const std::vector<std::string>& menu_items, size_t highlight)
{
    box(menu_win, 0, 0);  // Рисуем рамку вокруг окна
    for (size_t i = 0; i < menu_items.size(); ++i)
    {
        if (highlight == i)
        {
            wattron(menu_win, A_REVERSE);  // Выделяем текущий пункт меню
            mvwprintw(menu_win, i + 1, 2, "%s", menu_items[i].c_str());
            wattroff(menu_win, A_REVERSE);
        }
        else
        {
            mvwprintw(menu_win, i + 1, 2, "%s", menu_items[i].c_str());
        }
    }
    wrefresh(menu_win);
}

std::string inputFilePath(WINDOW* input_win,std::string message) 
{
    char input[256] = { 0 };  // Буфер для ввода текста
    int ch;
    int pos = 0;
    // Отображаем приглашение к вводу
    mvwprintw(input_win, 1, 1, message.c_str());
    wrefresh(input_win);
    while (true) 
    {
        ch = wgetch(input_win);// Получаем символ от пользователя
        if (ch == '\n') 
        {  // Enter завершает ввод
            input[pos] = '\0';
            break;
        }
        else if (ch == KEY_BACKSPACE || ch == 127) 
        {  // Backspace
            if (pos > 0) 
            {
                --pos;
                input[pos] = '\0';
                mvwprintw(input_win, 2, 1, "%-255s", input);  // Очищаем строку
                wmove(input_win, 2, 1 + pos);  // Перемещаем курсор
                box(input_win, 0, 0);
                wrefresh(input_win);
            }
        }
        else if (pos < 255) 
        {  // Добавляем символ в буфер
            input[pos++] = ch;
            mvwaddch(input_win, 2, pos, ch);  // Отображаем введенный символ
            wrefresh(input_win);
        }
    }
    return std::string(input);  // Возвращаем введенный путь
}

int countLinesForOutput(Torrent::TorrentFile file)
{
    int res = 7;//one for announce, one for "internal structure:", one for "press any button", "file content", +2 for 2 spaces
    //one for main folder
    if (file.createdBy.has_value()) ++res;
    if (file.creationDate.has_value()) ++res;
    for (size_t i = 0; i < file.info.files.size(); i++)
    {
        for (size_t j = 0; j < file.info.files[i].path.size(); j++)
        {
            ++res;
        }
    }
    return res;
}

bool strSizeComp(const std::string& str1, const std::string& str2)
{
    return str1.size() < str2.size();
}