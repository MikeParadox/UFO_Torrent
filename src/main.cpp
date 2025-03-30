ддд, [30.03.2025 20:40]
#include "ufo_torrent.h"
#include <iostream>
#include <ncurses.h>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

#include "../includes/prettyPrinter.h"
#include "../includes/decode.h"
#include "../includes/torrentFile.h"
#include "../includes/encode.h"
#include "../includes/fileUtils.h"
#include "../includes/createHash.h"
#include <boost/locale.hpp>
#include <filesystem>
#include "ncurses_utils.h"
#include "menu.h"
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/torrent_status.hpp>

using namespace bencode;
using namespace Torrent;
using namespace File;
using namespace Hash;

namespace fs = std::filesystem;

std::set<std::string> selectedTorrents;
std::set<TorrentFile> actualyTorrent;
int right_win_selected = 0;  // Track selected item in right window

std::string fileDialog(WINDOW* win, const std::string& startDir = ".") {
    std::string currentDir = startDir;
    if (startDir == ".") {
        currentDir = fs::current_path().string();
    }

    std::vector<std::string> files;
    int selected = 0;

    const int MAX_VISIBLE_LINES = 7;
    const int PADDING = 3;

    while (true) {
        files.clear();
        files.push_back("..");

        for (const auto& entry : fs::directory_iterator(currentDir)) {
            try {
                files.push_back(entry.path().filename().string());
            }
            catch (const fs::filesystem_error& e) {
                if (e.code() == std::errc::permission_denied) {
                    continue;
                }
            }
        }

        int startIndex = 0;
        if (selected >= MAX_VISIBLE_LINES) {
            startIndex = selected - MAX_VISIBLE_LINES + 1;
        }

        int numRows = std::min((int)files.size() - startIndex, MAX_VISIBLE_LINES);
        numRows = std::max(numRows, 3);

        werase(win);

        mvwprintw(win, 0, PADDING, "Select a .torrent file ");
        mvwprintw(win, 1, PADDING, "(ENTER to select, Q to quit):");
        mvwprintw(win, 2, PADDING, "Current Directory: %s", currentDir.c_str());

        int currDirLines = (22 + currentDir.size()) / 40;

        for (int i = 0; i < numRows; ++i) {
            int fileIndex = startIndex + i;
            if (fileIndex < (int)files.size()) {
                if (fileIndex == selected) {
                    wattron(win, A_REVERSE);
                }
                mvwprintw(win, i + 3 + currDirLines, PADDING, "%s", files[fileIndex].c_str());
                if (fileIndex == selected) {
                    wattroff(win, A_REVERSE);
                }
            }
        }
        wrefresh(win);

        int key = wgetch(win);
        switch (key) {
        case KEY_UP:
            if (selected > 0) --selected;
            break;
        case KEY_DOWN:
            if ((size_t)selected < files.size() - 1) ++selected;
            break;
        case 10: // Enter key
            if (files[selected] == "..") {
                auto parentDir = fs::path(currentDir).parent_path();
                if (fs::exists(parentDir) && fs::is_directory(parentDir)) {
                    currentDir = parentDir.string();
                    selected = 0;
                }
            }
            else if (fs::is_directory(currentDir + "/" + files[selected])) {
                currentDir += "/" + files[selected];
                selected = 0;
            }
            else if (files[selected].find(".torrent") != std::string::npos) {
                return currentDir + "/" + files[selected];
            }
            break;
        case 'q':
        case 'Q':
            return "";
        case KEY_BACKSPACE:
        case 127:
            currentDir = fs::path(currentDir).parent_path().string();
            selected = 0;
            break;
        }
    }
}

void refresh_right_win(WINDOW* right_win) {
    werase(right_win);
    box(right_win, 0, 0);
    mvwprintw(right_win, 1, 2, "Active Torrents (%zu)", selectedTorrents.size());

ддд, [30.03.2025 20:40]
// Adjust selection if out of bounds
    if (!selectedTorrents.empty()) {
        right_win_selected = std::min(right_win_selected, (int)selectedTorrents.size() - 1);
    }
    else {
        right_win_selected = 0;
    }

    int row = 3;
    int index = 0;
    for (const auto& torrent : selectedTorrents) {
        std::string displayName = fs::path(torrent).filename().string();

        // Highlight selected item
        if (index == right_win_selected) {
            wattron(right_win, A_REVERSE);
        }
        mvwprintw(right_win, row++, 2, "%s", displayName.c_str());
        if (index == right_win_selected) {
            wattroff(right_win, A_REVERSE);
        }

        index++;
        if (row >= getmaxy(right_win) - 1) break;
    }
    wrefresh(right_win);
}

void redraw_interface(WINDOW* left_win, WINDOW* right_win, MENU* left_menu) {
    refresh();
    clear();
    box(left_win, 0, 0);
    box(right_win, 0, 0);
    mvwprintw(left_win, 1, 2, "Main Menu");
    post_menu(left_menu);
    refresh_right_win(right_win);
    wrefresh(left_win);
    wrefresh(right_win);
}

int main() {
    setlocale(LC_ALL, "");
    session s;
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();

    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);

    const char* left_choices[] = {
        "Add Torrent",
        "Exit",
        nullptr
    };

    ITEM** left_items = new ITEM * [4];
    for (int i = 0; i < 4; ++i) {
        left_items[i] = new_item(left_choices[i], "");
    }
    left_items[4] = nullptr;

    MENU* left_menu = new_menu(left_items);

    WINDOW* left_win = newwin(10, 30, 2, 2);
    WINDOW* right_win = newwin(10, 30, 2, 34);
    keypad(left_win, TRUE);
    keypad(right_win, TRUE);

    set_menu_win(left_menu, left_win);
    set_menu_sub(left_menu, derwin(left_win, 6, 28, 3, 1));
    set_menu_mark(left_menu, " * ");
    set_menu_fore(left_menu, COLOR_PAIR(1));

    post_menu(left_menu);
    refresh_right_win(right_win);

    redraw_interface(left_win, right_win, left_menu);

    WINDOW* active_win = left_win;
    bool in_right_pane = false;

    int key;
    while ((key = wgetch(active_win)) != KEY_F(1)) {
        switch (key) {
        case KEY_DOWN:
            if (in_right_pane && !selectedTorrents.empty()) {
                right_win_selected = std::min(right_win_selected + 1, (int)selectedTorrents.size() - 1);
                refresh_right_win(right_win);
            }
            else if (!in_right_pane) {
                menu_driver(left_menu, REQ_DOWN_ITEM);
            }
            break;
        case KEY_UP:
            if (in_right_pane && !selectedTorrents.empty()) {
                right_win_selected = std::max(right_win_selected - 1, 0);
                refresh_right_win(right_win);
            }
            else if (!in_right_pane) {
                menu_driver(left_menu, REQ_UP_ITEM);
            }
            break;
        case KEY_LEFT:
            active_win = left_win;
            in_right_pane = false;
            break;
        case KEY_RIGHT:
            if (!selectedTorrents.empty()) {
                active_win = right_win;
                in_right_pane = true;
            }
            break;
        case 10: // Enter key
        {

            if (in_right_pane && !selectedTorrents.empty()) 
            {
                
            }
            else if (!in_right_pane) {
                ITEM* cur_item = current_item(left_menu);
                int choice = item_index(cur_item);

                switch (choice) {
                case 0: // Add Torrent
                {
                    WINDOW* fileWin = newwin(14, 50, 5, 10);
                    box(fileWin, 0, 0);
                    wrefresh(fileWin);
                    WINDOW* truefileWin = newwin(12, 40, 6, 11);
                    keypad(truefileWin, TRUE);
                    std::string selectedFile = fileDialog(truefileWin);
                    delwin(fileWin);
                    delwin(truefileWin);

ддд, [30.03.2025 20:40]
if (!selectedFile.empty()) 
                    {
                        TorrentFile file = parseTorrentFile(Decoder::decode(read(selectedFile)));
                        selectedTorrents.insert(selectedFile);
                        refresh_right_win(right_win);
                        try 
                        {
                            // Загружаем информацию о торренте
                            torrent_info info(selectedFile);

                            // Создаем параметры для добавления торрента в сессию
                            add_torrent_params p;
                            p.ti = std::make_shared<torrent_info>(info);
                            p.save_path = "./";  // Путь для сохранения скачанных файлов

                            // Добавляем торрент в сессию
                            torrent_handle h = s.add_torrent(p);

                            std::cout << "Скачивание началось..." << std::endl;

                            // Цикл обработки событий
                            while (!h.status().is_seeding) 
                            {
                                s.post_torrent_updates();
                                std::vector<alert*> alerts;
                                s.pop_alerts(&alerts);

                                // Получаем текущий прогресс
                                torrent_status status = h.status();

                                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                            }

                        }
                    }
                    redraw_interface(left_win, right_win, left_menu);
                    break;
                }
                case 1: // Exit
                    goto exit;
                }
            }
            break;
        }
        }
        wrefresh(left_win);
        wrefresh(right_win);
    }

exit:
    unpost_menu(left_menu);
    free_menu(left_menu);
    for (int i = 0; i < 4; ++i) {
        free_item(left_items[i]);
    }
    delete[] left_items;
    delwin(left_win);
    delwin(right_win);
    endwin();

    return 0;
}