#include "ufo_torrent.h"

#include <iostream>
#include <ncurses.h>
#include <string>
#include <vector>

#include "../includes/prettyPrinter.h"
#include "../includes/decode.h"
#include "../includes/torrentFile.h"
#include "../includes/encode.h"
#include "../includes/fileUtils.h"
#include "../includes/createHash.h"
#include <boost/locale.hpp>
#include <filesystem>
#include "ncurses_utils.h"
#include <menu.h>

using namespace bencode;
using namespace Torrent;
using namespace File;
using namespace Hash;

#include <filesystem>
#include <menu.h>

namespace fs = std::filesystem;

#include <cstring>

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

        // all files
        for (const auto& entry : fs::directory_iterator(currentDir)) {
            files.push_back(entry.path().filename().string());
        }

        // maybe will be deleted
        int startIndex = 0;
        if (selected >= MAX_VISIBLE_LINES) {
            startIndex = selected - MAX_VISIBLE_LINES + 1;
        }

        // meybe will be deleted
        int numRows = std::min((int)files.size() - startIndex, MAX_VISIBLE_LINES);
        numRows = std::max(numRows, 3);

        werase(win);


        // help
        mvwprintw(win, 0, PADDING, "Select a .torrent file ");
        mvwprintw(win, 1, PADDING, "(ENTER to select, Q to quit):");

        // current path
        mvwprintw(win, 2, PADDING, "Current Directory: %s", currentDir.c_str());

        int currDirLines = (22 + currentDir.size()) / 40;

        // render files
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

        // hangle loop
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





int main() {
    // init
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();

    // highlite color
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);

    // left options
    const char* left_choices[] = {
        "Add Torrent",
        "Verify Hash",
        "Options",
        "Exit",
        nullptr
    };

    //right options
    const char* right_choices[] = {
        "Start Download",
        "Pause Download",
        "Remove Torrent",
        "Settings",
        nullptr
    };

    // left items
    ITEM** left_items = new ITEM * [4];
    for (int i = 0; i < 4; ++i) {
        left_items[i] = new_item(left_choices[i], "");
    }
    left_items[4] = nullptr;

    // right items
    ITEM** right_items = new ITEM * [4];
    for (int i = 0; i < 4; ++i) {
        right_items[i] = new_item(right_choices[i], "");
    }
    right_items[4] = nullptr;

    MENU* left_menu = new_menu(left_items);
    MENU* right_menu = new_menu(right_items);

    // menu windowses
    WINDOW* left_win = newwin(10, 30, 2, 2);
    WINDOW* right_win = newwin(10, 30, 2, 34);
    keypad(left_win, TRUE);
    keypad(right_win, TRUE);

    // sizes of menus
    set_menu_win(left_menu, left_win);
    set_menu_sub(left_menu, derwin(left_win, 6, 28, 3, 1));
    set_menu_win(right_menu, right_win);
    set_menu_sub(right_menu, derwin(right_win, 6, 28, 3, 1));

    // menu options
    set_menu_mark(left_menu, " * ");
    set_menu_mark(right_menu, " * ");

    // Post
    post_menu(left_menu);
    post_menu(right_menu);

    // borders
    box(left_win, 0, 0);
    box(right_win, 0, 0);

    // Print titles
    mvwprintw(left_win, 1, 2, "Left Menu");
    mvwprintw(right_win, 1, 2, "Right Menu");
    wrefresh(left_win);
    wrefresh(right_win);

    // tracking
    auto active_win = left_win;
    auto active_menu = left_menu;

    // active menu
    set_menu_fore(left_menu, COLOR_PAIR(1));
    set_menu_fore(right_menu, COLOR_PAIR(2));

    // Menu navigation
    int key;
    while ((key = wgetch(active_win)) != KEY_F(1)) {
        switch (key) {
        case KEY_DOWN:
            menu_driver(active_menu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(active_menu, REQ_UP_ITEM);
            break;
        case KEY_LEFT:
            active_menu = left_menu;
            set_menu_fore(left_menu, COLOR_PAIR(1));
            set_menu_fore(right_menu, COLOR_PAIR(2));
            break;
        case KEY_RIGHT:
            active_menu = right_menu;
            set_menu_fore(left_menu, COLOR_PAIR(2));
            set_menu_fore(right_menu, COLOR_PAIR(1));
            break;
        case 10: // Enter key
        {
            ITEM* cur_item = current_item(active_menu);
            int choice = item_index(cur_item);

            // Handle choice
            if (active_menu == left_menu) {
                if (choice == 0) { // Add Torrent
                    
                    WINDOW* fileWin = newwin(14, 50, 5, 10);
                    box(fileWin, 0, 0);
                    wrefresh(fileWin);
                    WINDOW* truefileWin = newwin(12, 45, 6, 11);
                    keypad(truefileWin, TRUE);
                    std::string selectedFile = fileDialog(truefileWin);
                   
                    delwin(fileWin);
                    delwin(truefileWin);
                    refresh();
                    redrawwin(left_win);
                    redrawwin(right_win);
                    //next draft
                    if (!selectedFile.empty()) {
                        mvprintw(12, 2, "Selected file: %s", selectedFile.c_str());
                        
                    }
                }
                else if (choice == 3) { // Exit
                    goto exit;
                }
            }
        }
        break;
        }
        wrefresh(left_win);
        wrefresh(right_win);
    }

    //delete all sht
exit:
    unpost_menu(left_menu);
    unpost_menu(right_menu);
    free_menu(left_menu);
    free_menu(right_menu);
    for (int i = 0; i < 4; ++i) {
        free_item(left_items[i]);
        free_item(right_items[i]);
    }
    delete[] left_items;
    delete[] right_items;
    delwin(left_win);
    delwin(right_win);
    endwin();

    return 0;
}
