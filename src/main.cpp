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

    // Maximum visible lines
    const int MAX_VISIBLE_LINES = 7;      
    const int PADDING = 3;          // Padding from the left border

    while (true) {
        files.clear();
        // Always add ".." to go up to the parent directory
        files.push_back("..");

        // List files and directories
        for (const auto& entry : fs::directory_iterator(currentDir)) {
            files.push_back(entry.path().filename().string());
        }

        // Calculate the start index for scrolling
        int startIndex = 0;
        if (selected >= MAX_VISIBLE_LINES) {
            startIndex = selected - MAX_VISIBLE_LINES + 1;
        }

        // Calculate the number of rows needed
        int numRows = std::min((int)files.size() - startIndex, MAX_VISIBLE_LINES);
        numRows = std::max(numRows, 3); // Ensure at least 3 rows are displayed

        werase(win);


        // Print title and current directory with padding
        mvwprintw(win, 0, PADDING, "Select a .torrent file ");
        mvwprintw(win, 1, PADDING, "(ENTER to select, Q to quit):");

        // Print current directory
        mvwprintw(win, 2, PADDING, "Current Directory: %s", currentDir.c_str());

        int currDirLines = (22 + currentDir.size()) / 40;

        // Display files and directories
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

        // Handle input
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
    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();

    // Define color pairs
    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);

    // Define menu items for the left menu
    const char* left_choices[] = {
        "Add Torrent",
        "Verify Hash",
        "Options",
        "Exit",
        nullptr
    };

    // Define menu items for the right menu
    const char* right_choices[] = {
        "Start Download",
        "Pause Download",
        "Remove Torrent",
        "Settings",
        nullptr
    };

    // Create ITEMs for the left menu
    ITEM** left_items = new ITEM * [4];
    for (int i = 0; i < 4; ++i) {
        left_items[i] = new_item(left_choices[i], "");
    }
    left_items[4] = nullptr;

    // Create ITEMs for the right menu
    ITEM** right_items = new ITEM * [4];
    for (int i = 0; i < 4; ++i) {
        right_items[i] = new_item(right_choices[i], "");
    }
    right_items[4] = nullptr;

    // Create the menus
    MENU* left_menu = new_menu(left_items);
    MENU* right_menu = new_menu(right_items);

    // Create windows for the menus
    WINDOW* left_win = newwin(10, 30, 2, 2);
    WINDOW* right_win = newwin(10, 30, 2, 34);
    keypad(left_win, TRUE);
    keypad(right_win, TRUE);

    // Set the menu windows and subwindows
    set_menu_win(left_menu, left_win);
    set_menu_sub(left_menu, derwin(left_win, 6, 28, 3, 1));
    set_menu_win(right_menu, right_win);
    set_menu_sub(right_menu, derwin(right_win, 6, 28, 3, 1));

    // Set menu options
    set_menu_mark(left_menu, " * ");
    set_menu_mark(right_menu, " * ");

    // Post both menus
    post_menu(left_menu);
    post_menu(right_menu);

    // Draw boxes around the windows
    box(left_win, 0, 0);
    box(right_win, 0, 0);

    // Print titles
    mvwprintw(left_win, 1, 2, "Left Menu");
    mvwprintw(right_win, 1, 2, "Right Menu");
    wrefresh(left_win);
    wrefresh(right_win);

    // Track the active menu (0 = left, 1 = right)
    auto active_win = left_win;
    auto active_menu = left_menu;

    // Highlight the active menu initially
    set_menu_fore(left_menu, COLOR_PAIR(1));
    set_menu_fore(right_menu, COLOR_PAIR(2));

    // Menu interaction loop
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

            // Handle the selected choice
            if (active_menu == left_menu) {
                if (choice == 0) { // Add Torrent
                    // Create a new window for the file dialog
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
                    if (!selectedFile.empty()) {
                        mvprintw(12, 2, "Selected file: %s", selectedFile.c_str());
                        // Here you can add code to handle the selected .torrent file
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
