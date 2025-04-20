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
#include <filesystem>
#include <libtorrent/session.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_handle.hpp>

#include <ncurses.h>

namespace lt = libtorrent;
namespace fs = std::filesystem;

using namespace bencode;
using namespace Torrent;
using namespace File;
using namespace Hash;

struct WindowState
{
    int selected = 0;
    bool active = false;
    std::vector<std::string> items;
};

WindowState left_win, right_win;
std::set<std::string> selectedTorrents;
const std::vector<std::string> left_items = { "Add Torrent","Select DownDir", "Exit" };

// Global libtorrent session
//lt::session torrent_session;

void renderWindows(WINDOW* lwin, WINDOW* rwin)
{
    werase(lwin);
    werase(rwin);

    box(lwin, 0, 0);
    mvwprintw(lwin, 1, 2, "Main Menu");
    int max_width_l = getmaxx(lwin) - 4;
    for (size_t i = 0; i < left_items.size(); ++i)
    {
        if (left_win.active && i == static_cast<size_t>(left_win.selected)) wattron(lwin, A_REVERSE);
        mvwprintw(lwin, i + 3, 2, "%.*s", max_width_l, left_items[i].c_str());
        if (left_win.active && i == static_cast<size_t>(left_win.selected)) wattroff(lwin, A_REVERSE);
    }

    box(rwin, 0, 0);
    mvwprintw(rwin, 1, 2, "Active Torrents (%zu)", selectedTorrents.size());
    int max_width_r = getmaxx(rwin) - 4;
    int row = 3;
    auto it = selectedTorrents.begin();
    int i = 0;
    for (it = selectedTorrents.begin(); it != selectedTorrents.end() && row < getmaxy(rwin) - 2; ++it, ++i)
    {
        std::string name = fs::path(*it).filename().string();
        if (right_win.active && i == right_win.selected) wattron(rwin, A_REVERSE);
        mvwprintw(rwin, row++, 2, "%.*s", max_width_r, name.c_str());
        if (right_win.active && i == right_win.selected) wattroff(rwin, A_REVERSE);
    }

    wrefresh(lwin);
    wrefresh(rwin);
}

std::string fileDialog(WINDOW* win, const std::string& startDir = ".")
{
    std::string currentDir = (startDir == ".") ? fs::current_path().string() : startDir;
    std::vector<std::string> files;
    int selected = 0;
    std::string errorMsg;

    auto calculate_path_height = [](const std::string& path, const std::string& error = "")
        {
            int max_width = COLS - 4;
            int lines = 1;
            size_t pos = 0;

            while (pos < path.length())
            {
                pos += max_width;
                lines++;
            }

            return std::max(lines + 2, 3); // +2 for borders
        };

    int path_win_height = calculate_path_height(currentDir);
    int path_win_y = LINES - path_win_height;
    WINDOW* path_win = newwin(path_win_height, COLS, path_win_y, 0);
    keypad(win, TRUE);

    auto refresh_path_window = [&]()
        {
            werase(path_win);
            box(path_win, 0, 0);

            std::string path = "Current Path: " + currentDir;
            int max_width = COLS - 4;
            int line = 1;
            size_t pos = 0;

            while (pos < path.length())
            {
                size_t end = std::min(pos + max_width, path.length());
                mvwprintw(path_win, line++, 2, "%.*s", (int)(end - pos), path.c_str() + pos);
                pos = end;
            }

            if (!errorMsg.empty())
            {
                line++; 
                wattron(path_win, COLOR_PAIR(1) | A_BOLD);
                pos = 0;
                while (pos < errorMsg.length())
                {
                    size_t end = std::min(pos + max_width, errorMsg.length());
                    mvwprintw(path_win, line++, 2, "%.*s", (int)(end - pos), errorMsg.c_str() + pos);
                    pos = end;
                }
                wattroff(path_win, COLOR_PAIR(1) | A_BOLD);
            }

            wrefresh(path_win);
        };


    while (true)
    {
        errorMsg.clear();
        files = { ".." };
            for (const auto& entry : fs::directory_iterator(currentDir))
            {
                try
                {
                    if (entry.is_directory() || (entry.is_regular_file() && entry.path().extension() == ".torrent"))
                    {
                        files.push_back(entry.path().filename().string());
                    }
                }
                catch (const fs::filesystem_error& e)
                {
                    continue;
                }
            }


        selected = std::clamp(selected, 0, (int)files.size() - 1);

        int new_height = calculate_path_height(currentDir, errorMsg);
        if (new_height != path_win_height)
        {
            path_win_height = new_height;
            path_win_y = LINES - path_win_height;
            delwin(path_win);
            path_win = newwin(path_win_height, COLS, path_win_y, 0);
        }

        refresh_path_window();

        werase(win);
        box(win, 0, 0);

        int max_width = getmaxx(win) - 4;
        int max_height = getmaxy(win) - 4;
        int MAX_VISIBLE_LINES = max_height;

        int startIndex = 0;
        if (selected >= MAX_VISIBLE_LINES)
        {
            startIndex = selected - MAX_VISIBLE_LINES + 1;
        }

        int numRows = std::min((int)files.size() - startIndex, MAX_VISIBLE_LINES);

        mvwprintw(win, 1, 2, "Select a .torrent file (ENTER to select, Q to quit)");

        for (int i = 0; i < numRows; ++i)
        {
            int fileIndex = startIndex + i;
            if (fileIndex < (int)files.size())
            {
                if (fileIndex == selected) wattron(win, A_REVERSE);
                mvwprintw(win, 2 + i, 2, "%.*s", max_width, files[fileIndex].c_str());
                if (fileIndex == selected) wattroff(win, A_REVERSE);
            }
        }

        wrefresh(win);

        int key = wgetch(win);
        switch (key)
        {
        case KEY_UP:
            if (selected > 0) --selected;
            break;
        case KEY_DOWN:
            if (selected < (int)files.size() - 1) ++selected;
            break;
        case 10: // Enter
        {
            const std::string& choice = files[selected];
            if (choice == "..")
            {
                auto parent = fs::path(currentDir).parent_path();
                if (!parent.empty())
                {
                    try
                    {
                        if (fs::exists(parent) && fs::is_directory(parent))
                        {
                            currentDir = parent.string();
                            selected = 0;
                        }
                    }
                    catch (const fs::filesystem_error& e)
                    {
                        errorMsg = "Error: " + std::string(e.what());
                    }
                }
            }
            else
            {
                auto path = fs::path(currentDir) / choice;
                try
                {
                    if (fs::is_directory(path))
                    {
                        fs::directory_iterator test_it(path);
                        currentDir = path.string();
                        selected = 0;
                    }
                    else if (path.extension() == ".torrent")
                    {
                        werase(path_win);
                        wrefresh(path_win);
                        delwin(path_win);
                        touchwin(stdscr);
                        refresh();
                        return path.string();
                    }
                }
                catch (const fs::filesystem_error& e)
                {
                    errorMsg = "Error: " + std::string(e.what());
                }
            }
            break;
        }
        case 'q': case 'Q':
            werase(path_win);
            wrefresh(path_win);
            delwin(path_win);
            touchwin(stdscr);
            refresh();
            return "";
        case KEY_BACKSPACE: case 127:
        {
            auto parent = fs::path(currentDir).parent_path();
            if (!parent.empty())
            {
                try
                {
                    if (fs::exists(parent) && fs::is_directory(parent))
                    {
                        currentDir = parent.string();
                        selected = 0;
                    }
                }
                catch (const fs::filesystem_error& e)
                {
                    errorMsg = "Error: " + std::string(e.what());
                }
            }
            break;
        }
        }
    }
}

int main()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    printf("\033[?7l");
    fflush(stdout);

    WINDOW* lwin = newwin(LINES - 4, COLS / 2, 2, 1);
    WINDOW* rwin = newwin(LINES - 4, COLS / 2 - 1, 2, COLS / 2 + 1);
    keypad(lwin, TRUE);
    keypad(rwin, TRUE);

    left_win.active = true;
    left_win.items = left_items;
    right_win.items.assign(selectedTorrents.begin(), selectedTorrents.end());

    renderWindows(lwin, rwin);

    int ch;
    while ((ch = wgetch(left_win.active ? lwin : rwin)))
    {
        if (ch == KEY_F(1)) break;

        if (left_win.active)
        {
            switch (ch)
            {
            case KEY_DOWN: left_win.selected = (left_win.selected + 1) % left_items.size(); break;
            case KEY_UP: left_win.selected = (left_win.selected - 1 + left_items.size()) % left_items.size(); break;
            case KEY_RIGHT:
                if (!selectedTorrents.empty())
                {
                    left_win.active = false;
                    right_win.active = true;
                    right_win.selected = 0;
                }
                break;
            case 10:
                if (left_win.selected == 0)
                {
                    //WINDOW* fwin = newwin(LINES * 3 / 4, COLS * 3 / 4, LINES / 8, COLS / 8);
                    //box(fwin, 0, 0);
                    //wrefresh(fwin);

                    WINDOW* tfwin = newwin((LINES * 3 / 4) - 2, (COLS * 3 / 4) - 2, (LINES / 8) + 1, (COLS / 8) + 1);
                    keypad(tfwin, TRUE);
                    std::string path = fileDialog(tfwin);
                    delwin(tfwin);
                    //delwin(fwin);

                    if (!path.empty())
                    {
                        selectedTorrents.insert(path);
                        right_win.items.assign(selectedTorrents.begin(), selectedTorrents.end());
                        //try
                        //{
                        //    lt::add_torrent_params atp;
                        //    atp.ti = std::make_shared<lt::torrent_info>(path);
                        //    atp.save_path = ".";
                        //    torrent_session.add_torrent(atp);
                        //}
                        //catch (const std::exception& e)
                        //{
                        //    mvprintw(LINES - 2, 0, "Failed to add torrent: %s", e.what());
                        //    clrtoeol();
                        //    refresh();
                        //}
                    }
                    renderWindows(lwin, rwin);
                }
                else if (left_win.selected == 1)
                {
                    goto cleanup;
                }
                break;
            }
        }
        else if (right_win.active)
        {
            switch (ch)
            {
            case KEY_LEFT:
                left_win.active = true;
                right_win.active = false;
                break;
            case KEY_DOWN:
                if (!right_win.items.empty())
                    right_win.selected = (right_win.selected + 1) % right_win.items.size();
                break;
            case KEY_UP:
                if (!right_win.items.empty())
                    right_win.selected = (right_win.selected - 1 + right_win.items.size()) % right_win.items.size();
                break;
            case 10:
                if (!right_win.items.empty())
                {
                    std::string selected = *std::next(selectedTorrents.begin(), right_win.selected);
                    mvprintw(LINES - 1, 0, "Selected: %s", selected.c_str());
                    clrtoeol();
                    refresh();
                }
                break;
            case 'r':
                if (!right_win.items.empty())
                {
                    auto it = std::next(selectedTorrents.begin(), right_win.selected);
                    selectedTorrents.erase(it);
                    right_win.items.assign(selectedTorrents.begin(), selectedTorrents.end());
                    right_win.selected = std::min(right_win.selected, (int)right_win.items.size() - 1);
                }
                break;
            }
        }

        renderWindows(lwin, rwin);
    }

cleanup:
    printf("\033[?7h");
    fflush(stdout);

    delwin(lwin);
    delwin(rwin);
    endwin();
    return 0;
}