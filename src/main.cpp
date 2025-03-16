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

using namespace bencode;
using namespace Torrent;
using namespace File;
using namespace Hash;

void print_menu(WINDOW* win, int highlight, const std::vector<std::string>& menu_items) {
    int y = 1;
    for (size_t i = 0; i < menu_items.size(); ++i) {
        if (i == (size_t)highlight) {
            wattron(win, A_REVERSE);
        }
        mvwprintw(win, y++, 1, "%s", menu_items[i].c_str());
        wattroff(win, A_REVERSE);
    }
    wrefresh(win);
}

int main() {
    // Initialize ncurses

    initscr();
    cbreak();
    noecho();
    curs_set(0);

    // Define menus
    std::vector<std::string> menu_obj = {
        "add Torrent",
        "Verify hash",
        "options?",
        "Exit"
    };

    std::vector<std::string> sec_obj = {
        "Torrent1",
        "Torrent2",
        "Torrent3",
    };

    // Create windows for both menus
    int rows = 10, cols = 20; // Dimensions for each window
    WINDOW* main_win = newwin(menu_obj.size()+2, cols, 0, 0); // Main menu on the left
    WINDOW* tor_win = newwin(rows, cols, 0, cols); // Secondary menu on the right

    box(main_win, 0, 0); // Draw borders
    box(tor_win, 0, 0);

    wrefresh(main_win);
    wrefresh(tor_win);

    // Variables for menu selection
    int current_menu = 0; // 0: Main menu, 1: Secondary menu
    int main_highlight = 0; // Highlighted item in the main menu
    int secondary_highlight = 0; // Highlighted item in the secondary menu

    int key;
    while (true) {
        if (current_menu == 0) { // Main menu is active
            print_menu(main_win, main_highlight, menu_obj);
            print_menu(tor_win, secondary_highlight, sec_obj);

            wrefresh(tor_win);

            key = wgetch(main_win);
        }
        else { // Secondary menu is active
            print_menu(main_win, main_highlight, menu_obj);
            print_menu(tor_win, secondary_highlight, sec_obj);

            key = wgetch(tor_win);
        }

        // Handle user input
        switch (key) {
        case KEY_UP: // Move up
            if (current_menu == 0 && main_highlight > 0) {
                main_highlight--;
            }
            else if (current_menu == 1 && secondary_highlight > 0) {
                secondary_highlight--;
            }
            break;

        case KEY_DOWN: // Move down
            if (current_menu == 0 && main_highlight < static_cast<int>(menu_obj.size()) - 1) {
                main_highlight++;
            }
            else if (current_menu == 1 && secondary_highlight < static_cast<int>(sec_obj.size()) - 1) {
                secondary_highlight++;
            }
            break;

        case KEY_LEFT: // Switch to main menu
            if (current_menu == 1) {
                current_menu = 0;
            }
            break;

        case KEY_RIGHT: // Switch to secondary menu
            if (current_menu == 0) {
                current_menu = 1;
            }
            break;

        case 10: // Enter key
            if (current_menu == 0 && main_highlight == static_cast<int>(menu_obj.size()) - 1) {
                // Exit if "Exit" is selected in the main menu
                break;
            }
            else if (current_menu == 1 && secondary_highlight == static_cast<int>(sec_obj.size()) - 1) {
                // Go back to the main menu
                current_menu = 0;
            }
            break;
        }

        // Exit the loop if "Exit" is selected
        if (current_menu == 0 && main_highlight == static_cast<int>(menu_obj.size()) - 1) {
            break;
        }
    }

    // Clean up
    delwin(main_win);
    delwin(tor_win);
    endwin();

    return 0;
}