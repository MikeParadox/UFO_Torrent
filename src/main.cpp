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

// Added a structure to track torrents statuses.
struct TorrentStatus {
    std::string name;
    float progress = 0.0f;
    int download_rate;
    std::string state;
};

struct WindowState
{
    int selected = 0;
    bool active = false;
    std::vector<std::string> items;
};

WindowState left_win, right_win;

std::vector <TorrentStatus> active_torrents;

std::mutex updates_mutex;

std::atomic<bool> running{ true };

std::set<std::string> selectedTorrents;
std::string downDir = ".";
WINDOW* lwin;
WINDOW* rwin;
const std::vector<std::string> left_items = {"Add Torrent", "Select Download Dir", "Create example Torrent",
                                             "Exit"};

lt::session torrent_session;
const float WINDOW_SIZE_RATIO = 0.7f; 


void update_progress(lt::session& torrent_session) {
    while (running) { 
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        auto handles = torrent_session.get_torrents();
        std::vector<lt::torrent_status> statuses;
        for (const auto& handle : handles) {
            statuses.push_back(handle.status());
        }

        std::lock_guard<std::mutex> lock(updates_mutex);
        active_torrents.clear();

        for (const auto& s : statuses) {
            TorrentStatus tor;
            tor.name = s.name;
            tor.progress = s.progress * 100.0f; 
            tor.download_rate = s.download_rate / 1024;

            if ((s.flags & lt::torrent_flags::paused) != 0) {
                tor.state = "Paused";
            }
            else {
                switch (s.state)
                {
                case lt::torrent_status::state_t::downloading_metadata:
                    tor.state = "Metadata";
                    break;
                case lt::torrent_status::state_t::downloading:
                    tor.state = "Downloading";
                    break;
                case lt::torrent_status::state_t::seeding:
                    tor.state = "Seeding";
                    break;
                case lt::torrent_status::state_t::finished:
                    tor.state = "Finished";
                    break;
                default:
                    tor.state = "Unknown";
                }
            }

            active_torrents.push_back(tor);
        }
    }
}

void renderWindows(WINDOW* lwin, WINDOW* rwin)
{
    std::lock_guard<std::mutex> lock(updates_mutex);

    werase(lwin);
    werase(rwin);

    box(lwin, 0, 0);
    mvwprintw(lwin, 1, 2, "Main Menu");
    int max_width_l = getmaxx(lwin) - 4;
    for (size_t i = 0; i < left_items.size(); ++i)
    {
        if (left_win.active && i == static_cast<size_t>(left_win.selected))
            wattron(lwin, A_REVERSE);
        mvwprintw(lwin, i + 3, 2, "%.*s", max_width_l, left_items[i].c_str());
        if (left_win.active && i == static_cast<size_t>(left_win.selected))
            wattroff(lwin, A_REVERSE);
    }

    box(rwin, 0, 0);
    mvwprintw(rwin, 1, 2, "Active Torrents (%zu)", selectedTorrents.size());


    int max_width_r = getmaxx(rwin) - 30; 

    int row = 3;
    int i = 0;

    for (const auto& torrent : active_torrents)
    {
        if (row >= getmaxy(rwin) - 2) break;
        std::string name = torrent.name;
        if (right_win.active && i == right_win.selected)
            wattron(rwin, A_REVERSE);
        mvwprintw(rwin, row++, 2, "%.*s", max_width_r, name.c_str());


        int bar_width = 10;
        int pos = (torrent.progress / 100.0f) * bar_width;
        //wattron(rwin, COLOR_PAIR(1));
        wprintw(rwin, " [");
        for (int i = 0; i < bar_width; i++) {
            if (i < pos)
                waddch(rwin, ACS_BLOCK);
            else
                waddch(rwin, ' ');
        }
        wprintw(rwin, "] %d kb/s", torrent.download_rate);
        wattroff(rwin, COLOR_PAIR(1));
        //wprintw(rwin, " {%s}", torrent.state.c_str());

        if (right_win.active && i == right_win.selected)
            wattroff(rwin, A_REVERSE);
        i++;
    }

    wrefresh(lwin);
    wrefresh(rwin);
}

bool showTorrentPreview(WINDOW* parent, const std::string& path)
{
    try
    {
        TorrentFile file = parseTorrentFile(Decoder::decode(read(path)));

        int height = static_cast<int>(LINES * WINDOW_SIZE_RATIO);
        int width = static_cast<int>(COLS * WINDOW_SIZE_RATIO);
        int starty = (LINES - height) / 2;
        int startx = (COLS - width) / 2;

        WINDOW* preview = newwin(height, width, starty, startx);
        keypad(preview, TRUE);
        WINDOW* content = derwin(preview, height - 2, width - 2, 1, 1);

        // Disable text wrapping
        idcok(content, FALSE);
        idlok(content, FALSE);

        box(preview, 0, 0);
        mvwprintw(preview, 0, 2, " Torrent Preview ");
        wrefresh(preview);

        // Variables for scrolling
        int scroll_offset = 0;
        const int max_content_rows = height - 4; // Space for header
        bool needs_scrolling = false;

        while (true)
        {
            werase(content);

            int row = 0;
            mvwprintw(content, row++, 1, "Name: %s", file.info.name.c_str());

            if (file.info.files.empty())
            {
                mvwprintw(content, row++, 1, "Single File");
            }
            else
            {
                mvwprintw(content, row++, 1, "Files:");

                needs_scrolling = (
                    file.info.files.size() > static_cast<size_t>(
                        max_content_rows));

                std::map<std::string, std::vector<std::pair<
                             std::string, unsigned long long>>> dir_structure;
                for (const auto& fileInfo : file.info.files)
                {
                    std::string current_path;
                    for (size_t i = 0; i < fileInfo.path.size() - 1; i++)
                    {
                        current_path += (current_path.empty() ? "" : "/") +
                            fileInfo.path[i];
                        dir_structure[current_path];
                    }
                    if (!fileInfo.path.empty())
                    {
                        dir_structure[current_path].emplace_back(
                            fileInfo.path.back(), fileInfo.length);
                    }
                }
                std::function<void(const std::string&, int, int&)> display_tree
                    =
                    [&](const std::string& path, int depth, int& current_row)
                {
                    if (current_row >= max_content_rows + scroll_offset) return;

                    if (current_row >= scroll_offset)
                    {
                        std::string display_path = path.substr(
                            path.find_last_of('/') + 1);
                        mvwprintw(content, current_row - scroll_offset + 2,
                                  1 + depth * 2,
                                  "%s%s", std::string(depth * 2, ' ').c_str(),
                                  display_path.c_str());
                    }
                    current_row++;
                    for (const auto& file : dir_structure[path])
                    {
                        if (current_row >= max_content_rows + scroll_offset)
                            continue;
                        if (current_row >= scroll_offset)
                        {
                            mvwprintw(content, current_row - scroll_offset + 2,
                                      1 + (depth + 1) * 2,
                                      "%s%s (%llu)",
                                      std::string((depth + 1) * 2, ' ').c_str(),
                                      file.first.c_str(), file.second);
                        }
                        current_row++;
                    }
                    for (const auto& entry : dir_structure)
                    {
                        if (entry.first.find(path + '/') == 0 &&
                            entry.first.rfind('/') == path.length())
                        {
                            display_tree(entry.first, depth + 1, current_row);
                        }
                    }
                };

                int current_row = 0;
                for (const auto& entry : dir_structure)
                {
                    if (entry.first.find('/') == std::string::npos)
                    {
                        display_tree(entry.first, 0, current_row);
                    }
                }

            }
            mvwprintw(preview, height - 2, 1,
                      "ENTER: Accept  Q: Quit  UP/DOWN: Scroll");

            wrefresh(content);
            wrefresh(preview);
            int ch = wgetch(preview);
            switch (ch)
            {
            case KEY_UP: if (needs_scrolling && scroll_offset > 0)
                {
                    scroll_offset--;
                }
                break;
            case KEY_DOWN: if (needs_scrolling)
                {
                    scroll_offset++;
                }
                break;
            case 10: // ENTER - accept
                delwin(content);
                delwin(preview);
                touchwin(parent);
                wrefresh(parent);
                return true;
            case 'q':
            case 'Q': // Quit
                delwin(content);
                delwin(preview);
                touchwin(parent);
                wrefresh(parent);
                refresh();

                renderWindows(lwin, rwin);

                return false;
            default: continue;
            }
        }
    }
    catch (const std::exception& e)
    {
        WINDOW* err = newwin(5, COLS * 0.6, LINES / 2 - 2, COLS * 0.2);
        box(err, 0, 0);
        mvwprintw(err, 1, 1, "Error parsing torrent file:");
        mvwprintw(err, 2, 1, "%s", e.what());
        mvwprintw(err, 3, 1, "Press any key to continue...");
        wrefresh(err);
        wgetch(err);
        delwin(err);
        touchwin(parent);
        wrefresh(parent);
        return false;
    }
}

void to_low(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); });
}

std::string fileDialog(WINDOW* parent, const std::string& message, const std::string& startDir = ".",
                       const bool& only_dirs = false)
{
    std::string currentDir = (startDir == ".")
                                 ? fs::current_path().string()
                                 : startDir;
    std::vector<std::string> files;
    int selected = 0;
    std::string errorMsg;

    auto calculate_path_height = [](const std::string& path,
                                    const std::string& error = "")
    {
        int max_width = COLS - 4;
        int lines = 1;
        size_t pos = 0;

        while (pos < path.length())
        {
            pos += max_width;
            lines++;
        }

        if (!error.empty())
        {
            pos = 0;
            while (pos < error.length())
            {
                pos += max_width;
                lines++;
            }
        }

        return std::max(lines + 2, 3);
    };

    int path_win_height = calculate_path_height(currentDir);
    int path_win_y = LINES - path_win_height;
    WINDOW* path_win = newwin(path_win_height, COLS, path_win_y, 0);

    // windows auto-size
    int dialog_height = static_cast<int>(LINES * WINDOW_SIZE_RATIO);
    int dialog_width = static_cast<int>(COLS * WINDOW_SIZE_RATIO);
    int dialog_y = (LINES - dialog_height) / 2;
    int dialog_x = (COLS - dialog_width) / 2;

    WINDOW* dialog = newwin(dialog_height, dialog_width, dialog_y, dialog_x);
    keypad(dialog, TRUE);

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
            mvwprintw(path_win, line++, 2, "%.*s", (int)(end - pos),
                      path.c_str() + pos);
            pos = end;
        }

        if (!errorMsg.empty())
        {
            line++;
            wattron(path_win, A_BOLD);
            pos = 0;
            while (pos < errorMsg.length())
            {
                size_t end = std::min(pos + max_width, errorMsg.length());
                mvwprintw(path_win, line++, 2, "%.*s", (int)(end - pos),
                          errorMsg.c_str() + pos);
                pos = end;
            }
            wattroff(path_win, A_BOLD);
        }

        wrefresh(path_win);
    };

    while (true)
    {
        errorMsg.clear();
        files = {".."};

        try
        {
            for (const auto& entry : fs::directory_iterator(currentDir))
            {
                try
                {
                    if (entry.is_directory() || (
                            !only_dirs && entry.is_regular_file() && entry.
                            path().extension() == ".torrent"))
                    {
                        files.push_back(entry.path().filename().string());
                    }
                }
                catch (const fs::filesystem_error&)
                {
                    continue;
                }
            }
        }
        catch (const fs::filesystem_error& e)
        {
            errorMsg = "Error: " + std::string(e.what());
            if (files.size() > 1) files.resize(1);
        }

        selected = std::clamp(selected, 0, (int)files.size() - 1);
        
        //auto f = [](std::string a, std::string b) { return to_low(a) < to_low(b); }
        std::sort(files.begin(), files.end(), [](std::string a, std::string b) { to_low(a); to_low(b); return a < b; });
        int new_height = calculate_path_height(currentDir, errorMsg);
        if (new_height != path_win_height)
        {
            path_win_height = new_height;
            path_win_y = LINES - path_win_height;
            delwin(path_win);
            path_win = newwin(path_win_height, COLS, path_win_y, 0);
        }

        refresh_path_window();

        werase(dialog);
        box(dialog, 0, 0);

        int max_width = getmaxx(dialog) - 4;
        int max_height = getmaxy(dialog) - 4;
        int MAX_VISIBLE_LINES = max_height;

        int startIndex = 0;
        if (selected >= MAX_VISIBLE_LINES)
        {
            startIndex = selected - MAX_VISIBLE_LINES + 1;
        }

        int numRows = std::min((int)files.size() - startIndex,
                               MAX_VISIBLE_LINES);

        if (only_dirs)
        {
            mvwprintw(dialog, 1, 2, "%s",
                (message + " (E:move, Enter:select, Q:quit)").c_str());
        }
        else
        {
            mvwprintw(dialog, 1, 2, "%s",
                (message + " (ENTER:select, Q : quit)").c_str());
        }

        for (int i = 0; i < numRows; ++i)
        {
            int fileIndex = startIndex + i;
            if (fileIndex < (int)files.size())
            {
                if (fileIndex == selected)
                    wattron(dialog, A_REVERSE);
                mvwprintw(dialog, 2 + i, 2, "%.*s", max_width,
                          files[fileIndex].c_str());
                if (fileIndex == selected)
                    wattroff(dialog, A_REVERSE);
            }
        }

        wrefresh(dialog);

        int key = wgetch(dialog);
        switch (key)
        {
        case KEY_UP: if (selected > 0) --selected;
            break;
        case KEY_DOWN: if (selected < (int)files.size() - 1) ++selected;
            break;
        case 'e':
        case 'E': if (only_dirs)
            {
                auto path = fs::path(currentDir) / files[selected];
                try
                {
                    fs::directory_iterator test_it(path);
                    currentDir = path.string();
                    selected = 0;
                }
                catch (const fs::filesystem_error& e)
                {
                    errorMsg = "Error: " + std::string(e.what());
                }
            }
            break;
        case 10:
        {
            // Enter
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
            else if (only_dirs)
            {
                return (fs::path(currentDir) / choice).string();
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

                        bool accepted = showTorrentPreview(
                            parent, path.string());

                        //touchwin(stdscr);
                        //refresh();

                        if (accepted)
                        {
                            delwin(dialog);
                            return path.string();
                        }

                        path_win_height = calculate_path_height(currentDir);
                        path_win_y = LINES - path_win_height;
                        path_win = newwin(path_win_height, COLS, path_win_y, 0);
                        dialog = newwin(dialog_height, dialog_width, dialog_y,
                                        dialog_x);
                        keypad(dialog, TRUE);
                    }
                }
                catch (const fs::filesystem_error& e)
                {
                    errorMsg = "Error: " + std::string(e.what());
                }
            }
            break;
        }
        case 'q':
        case 'Q': werase(path_win);
            wrefresh(path_win);
            delwin(path_win);
            delwin(dialog);
            touchwin(parent);
            refresh();
            return "";
        case KEY_BACKSPACE:
        case 127:
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
    //background thread
    std::thread progress_thread(update_progress, std::ref(torrent_session));

    setlocale(LC_ALL, "");
    //setenv("TERMINFO", "/usr/share/terminfo", 1);
    initscr();
    // Initializing color pair to paint progress bar
    //start_color();
    //init_pair(1, COLOR_GREEN, COLOR_BLACK);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    printf("\033[?7l");
    fflush(stdout);

    lwin = newwin(LINES - 4, COLS / 2, 2, 1);
    rwin = newwin(LINES - 4, COLS / 2 - 1, 2, COLS / 2 + 1);
    keypad(lwin, TRUE);
    keypad(rwin, TRUE);

    left_win.active = true;
    left_win.items = left_items;
    right_win.items.assign(selectedTorrents.begin(), selectedTorrents.end());

    renderWindows(lwin, rwin);

    // Added a 100ms timeout for getch() func 
    wtimeout(lwin, 100);
    wtimeout(rwin, 100);

    
    while (running)
    {
        int ch = wgetch(left_win.active ? lwin : rwin);

        
        if (ch != ERR)
        {
            if (ch == KEY_F(1)) break;

            if (left_win.active)
            {
                switch (ch)
                {
                case KEY_DOWN: left_win.selected =
                    (left_win.selected + 1) % left_items.size();
                    break;
                case KEY_UP: left_win.selected =
                    (left_win.selected - 1 + left_items.size()) %
                    left_items.size();
                    break;
                case KEY_RIGHT: if (!selectedTorrents.empty())
                {
                    left_win.active = false;
                    right_win.active = true;
                    right_win.selected = 0;
                }
                              break;
                case 10: if (left_win.selected == 0)
                {
                    std::string path = fileDialog(stdscr,"Select a .torrent file");
                    if (!path.empty())
                    {
                        selectedTorrents.insert(path);
                        right_win.items.assign(selectedTorrents.begin(),
                            selectedTorrents.end());
                        try
                        {
                            lt::add_torrent_params atp;
                            atp.ti = std::make_shared<lt::torrent_info>(path);
                            atp.save_path = downDir;
                            torrent_session.add_torrent(atp);
                        }
                        catch (const std::exception& e)
                        {
                            mvprintw(LINES - 2, 0, "Failed to add torrent: %s",
                                e.what());
                            clrtoeol();
                            refresh();
                        }
                    }
                    renderWindows(lwin, rwin);
                }
                       else if (left_win.selected == 1)
                {
                    downDir = fileDialog(stdscr,"Select a download dir", ".", true);
                    clear();
                    renderWindows(lwin, rwin);
                    refresh();
                }
                       else if (left_win.selected == 2)
                {
                std::string path = fileDialog(stdscr,"Select a base-folder", ".", true);
                if (path.empty())
                {  
                    renderWindows(lwin, rwin);
                    continue;
                }

                clear();
                refresh();
                renderWindows(lwin, rwin);

                Torrent::TorrentFile torrent = Torrent::createTorrentFile("exemple", { {"exemple"} }, "Tester", path);

                std::string outPath = fileDialog(stdscr,"Select a output path", ".", true);
                if (outPath.empty())
                {  
                    renderWindows(lwin, rwin);
                    continue;
                }
                if (fs::is_directory(outPath))
                {
                    outPath = (fs::path(outPath) / "example.torrent").string();
                }
                std::string data = Encoder::encode(toValue(torrent));
                createFile(outPath, data);
                clear();
                refresh();
                renderWindows(lwin, rwin);
                }
                else if (left_win.selected == 3)
                {
                goto cleanup;
                    // Changing the flag value.
                    running = false;
                    goto cleanup;
                }
                       break;
                }
            }
            else if (right_win.active)
            {
                switch (ch)
                {
                case KEY_LEFT: left_win.active = true;
                    right_win.active = false;
                    break;
                case KEY_DOWN: if (!right_win.items.empty())
                {
                    right_win.selected =
                        (right_win.selected + 1) % right_win.items.size();
                }
                    break;
                case KEY_UP: if (!right_win.items.empty())
                {
                    right_win.selected =
                        (right_win.selected - 1 + right_win.items.size()) %
                        right_win.items.size();
                }
                    break;
                case 10: if (!right_win.items.empty())
                {
                    std::string selected = *std::next(
                        selectedTorrents.begin(), right_win.selected);
                    mvprintw(LINES - 1, 0, "Selected: %s", selected.c_str());
                    clrtoeol();
                    refresh();
                }
                       break;
                }
            }
        }

        renderWindows(lwin, rwin);
    }

cleanup:
    printf("\033[?7h");
    fflush(stdout);

    torrent_session.abort();

    delwin(lwin);
    delwin(rwin);
    endwin();


    running = false;
    progress_thread.join();
    torrent_session.abort();
    return 0;
}