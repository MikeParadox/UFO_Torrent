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

using namespace bencode;
using namespace Torrent;
using namespace File;
using namespace Hash;

namespace fs = std::filesystem;

std::set<std::string> selectedTorrents;
std::set<TorrentFile> actualyTorrent;
int right_win_selected = 0;  // Track selected item in right window
int left_win_selected = 0;   // Track selected item in left window
const char* left_choices[] = { "Add Torrent", "Exit" };

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

void refresh_left_win(WINDOW* left_win) {
    werase(left_win);
    box(left_win, 0, 0);
    mvwprintw(left_win, 1, 2, "Main Menu");

    for (int i = 0; i < 2; ++i) {
        if (i == left_win_selected) {
            wattron(left_win, A_REVERSE);
        }
        mvwprintw(left_win, i + 3, 2, "%s", left_choices[i]);
        if (i == left_win_selected) {
            wattroff(left_win, A_REVERSE);
        }
    }
    wrefresh(left_win);
}

void redraw_interface(WINDOW* left_win, WINDOW* right_win) {
    refresh();
    clear();
    refresh_left_win(left_win);
    refresh_right_win(right_win);
}

int main() {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    WINDOW* left_win = newwin(10, 30, 2, 2);
    WINDOW* right_win = newwin(10, 30, 2, 34);
    keypad(left_win, TRUE);
    keypad(right_win, TRUE);

    refresh_left_win(left_win);
    refresh_right_win(right_win);

    redraw_interface(left_win, right_win);

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
                left_win_selected = std::min(left_win_selected + 1, 1);
                refresh_left_win(left_win);
            }
            break;
        case KEY_UP:
            if (in_right_pane && !selectedTorrents.empty()) {
                right_win_selected = std::max(right_win_selected - 1, 0);
                refresh_right_win(right_win);
            }
            else if (!in_right_pane) {
                left_win_selected = std::max(left_win_selected - 1, 0);
                refresh_left_win(left_win);
            }
            break;
        case KEY_LEFT:
            active_win = left_win;
            in_right_pane = false;
            refresh_left_win(left_win);
            break;
        case KEY_RIGHT:
            if (!selectedTorrents.empty()) {
                active_win = right_win;
                in_right_pane = true;
                refresh_right_win(right_win);
            }
            break;
        case 10: // Enter key
        {
            if (in_right_pane && !selectedTorrents.empty())
            {
                // Handle right pane selection
            }
            else if (!in_right_pane) {
                switch (left_win_selected) {
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

                    if (!selectedFile.empty()) {
                        TorrentFile file = parseTorrentFile(Decoder::decode(read(selectedFile)));
                        selectedTorrents.insert(selectedFile);
                        refresh_right_win(right_win);
                    }
                    redraw_interface(left_win, right_win);
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
    delwin(left_win);
    delwin(right_win);
    endwin();

    return 0;
}