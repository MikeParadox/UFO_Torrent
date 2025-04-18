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
const std::vector<std::string> left_items = { "Add Torrent","Select DownDir", "Exit"};

// Global libtorrent session
lt::session torrent_session;

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
    std::string currentDir = startDir;
    if (startDir == ".")
    {
        currentDir = fs::current_path().string();
    }

    std::vector<std::string> files;
    int selected = 0;

    const int MAX_VISIBLE_LINES = 7;
    const int PADDING = 3;

    while (true)
    {
        files.clear();
        files.push_back("..");

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
                if (e.code() == std::errc::permission_denied)
                {
                    continue;
                }
            }
        }

        int startIndex = 0;
        if (selected >= MAX_VISIBLE_LINES)
        {
            startIndex = selected - MAX_VISIBLE_LINES + 1;
        }

        int numRows = std::min((int)files.size() - startIndex, MAX_VISIBLE_LINES);
        numRows = std::max(numRows, 3);

        werase(win);

        int max_width = getmaxx(win) - 2 * PADDING;

        mvwprintw(win, 0, PADDING, "Select a .torrent file ");
        mvwprintw(win, 1, PADDING, "(ENTER to select, Q to quit):");
        mvwprintw(win, 2, PADDING, "Current Directory: %.*s", max_width, currentDir.c_str());

        int currDirLines = (22 + currentDir.size()) / 40;

        for (int i = 0; i < numRows; ++i)
        {
            int fileIndex = startIndex + i;
            if (fileIndex < (int)files.size())
            {
                if (fileIndex == selected)
                {
                    wattron(win, A_REVERSE);
                }
                mvwprintw(win, i + 3 + currDirLines, PADDING, "%.*s", max_width, files[fileIndex].c_str());
                if (fileIndex == selected)
                {
                    wattroff(win, A_REVERSE);
                }
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
            if ((size_t)selected < files.size() - 1) ++selected;
            break;
        case 10:
            if (files[selected] == "..")
            {
                auto parentDir = fs::path(currentDir).parent_path();
                if (fs::exists(parentDir) && fs::is_directory(parentDir))
                {
                    currentDir = parentDir.string();
                    selected = 0;
                }
            }
            else if (fs::is_directory(currentDir + "/" + files[selected]))
            {
                currentDir += "/" + files[selected];
                selected = 0;
            }
            else if (files[selected].find(".torrent") != std::string::npos)
            {
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
                    WINDOW* fwin = newwin(LINES / 2, COLS / 2, LINES / 4, COLS / 4);
                    box(fwin, 0, 0); // Draw the border
                    wrefresh(fwin); // Refresh to show the border

                    WINDOW* tfwin = newwin((LINES / 2) - 2, (COLS / 2) - 2, (LINES / 4) + 1, (COLS / 4) + 1);
                    keypad(tfwin, TRUE);
                    std::string path = fileDialog(tfwin);
                    delwin(tfwin);
                    delwin(fwin);

                    if (!path.empty())
                    {
                        selectedTorrents.insert(path);
                        right_win.items.assign(selectedTorrents.begin(), selectedTorrents.end());

                        // Add torrent to libtorrent session
                        try
                        {
                            lt::add_torrent_params atp;
                            atp.ti = std::make_shared<lt::torrent_info>(path);
                            atp.save_path = "."; // Change to desired download folder
                            torrent_session.add_torrent(atp);
                        }
                        catch (const std::exception& e)
                        {
                            mvprintw(LINES - 2, 0, "Failed to add torrent: %s", e.what());
                            clrtoeol();
                            refresh();
                        }
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
