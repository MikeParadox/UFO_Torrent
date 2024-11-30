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


int main() 
{
    initscr();
    noecho();
    cbreak();
    curs_set(0);

    std::vector<std::string> menu_items = { "Scan file", "Create example torrent", "exit"};
    
    size_t highlight = 0;
    size_t choice = 0;

    int menu_height = menu_items.size()+2;
    int menu_width = (*std::max_element(menu_items.begin(),menu_items.end(),strSizeComp)).size()+4;
    int menu_start_y = 1;
    int menu_start_x = 1;
    WINDOW* menu_win = newwin(menu_height, menu_width, menu_start_y, menu_start_x);
    keypad(menu_win, true);

    while (true)
    {
        
        display_menu(menu_win, menu_items, highlight);

        int ch = wgetch(menu_win);
        switch (ch)
        {
        case KEY_UP:
            if (highlight > 0) --highlight;
            break;
        case KEY_DOWN:
            if (highlight < menu_items.size()-1) ++highlight;
            break;
        case '\n':  // Enter
            choice = highlight + 1;
            break;
        default:
            break;
        }
        if (choice != 0)
        {
            if (choice == 1)
            {  
                int input_height = 8, input_width = 50;
                int input_start_y = menu_start_y;
                int input_start_x = menu_start_x+menu_width;
                WINDOW* input_win = newwin(input_height, input_width, input_start_y, input_start_x);
                box(input_win, 0, 0);
                wrefresh(input_win);
                try 
                {
                    TorrentFile file = parseTorrentFile(Decoder::decode(read(inputFilePath(input_win, "Put a path to file here:"))));
                    //TorrentFile file = parseTorrentFile(Decoder::decode(readFile("../../exemple.torrent")));
                    int y_output = countLinesForOutput(file);
                    WINDOW* output_win = newwin(y_output, 50, input_start_y, input_start_x);
                    werase(input_win);

                    box(output_win, 0, 0);
                    int curr_y = 1;
                    mvwprintw(output_win, curr_y++, 1, "Your file content: ");
                    mvwprintw(output_win, curr_y++, 1, "Announce: %s", file.announce.c_str());
                    if (file.createdBy.has_value()) mvwprintw(output_win, curr_y++, 1, "CreatedBy: %s", file.createdBy.value().c_str());
                    if (file.creationDate.has_value()) mvwprintw(output_win, curr_y++, 1, "CreationDate: %s", std::to_string(file.creationDate.value()).c_str());
                    mvwprintw(output_win, curr_y++, 1, "Internal structure: ");
                    mvwprintw(output_win, curr_y++, 1, "%s", file.info.name.c_str());
                    for (size_t i = 0; i < file.info.files.size(); i++)
                    {
                        for (size_t j = 0; j < file.info.files[i].path.size(); j++)
                        {
                            mvwprintw(output_win, curr_y++, 1, "  %s", file.info.files[i].path[j].c_str());
                        }
                    }
                    mvwprintw(output_win, curr_y++, 1, "Press any button");
                    wrefresh(output_win);
                    wgetch(output_win);
                    wclear(output_win);
                }
                catch (std::runtime_error& e)
                {
                    werase(input_win);
                    box(input_win, 0, 0);
                    mvwprintw(input_win, 1, 1, "No such file!");
                    mvwprintw(input_win, 2, 1, "Press any button");
                    wrefresh(input_win);
                    wgetch(input_win);
                    delwin(input_win);
                }
                catch (std::invalid_argument& e)
                {
                    werase(input_win);
                    box(input_win, 0, 0);
                    mvwprintw(input_win, 1, 1, "Wrong format file!");
                    mvwprintw(input_win, 2, 1, "Press any button");
                    wrefresh(input_win);
                    wgetch(input_win);
                    delwin(input_win);
                }
                clear();
                refresh();
            }
            else if (choice == 2)
            {
                int input_height = 8, input_width = 50;
                int input_start_y = menu_start_y;
                int input_start_x = menu_start_x + menu_width;
                WINDOW* input_win = newwin(input_height, input_width, input_start_y, input_start_x);
                box(input_win, 0, 0);
                wrefresh(input_win);
                try
                {
                //#todo
                    std::string folderPath = inputFilePath(input_win, "Put a path to folder here: ");
                    werase(input_win);
                    box(input_win, 0, 0);
                    wrefresh(input_win);

                    std::string userName = inputFilePath(input_win, "Put your name here: ");
                    werase(input_win);
                    box(input_win, 0, 0);
                    wrefresh(input_win);

                    Torrent::TorrentFile torrent = Torrent::createTorrentFile("exemple", { {"exemple"} }, userName, folderPath);
                    std::string filePath = inputFilePath(input_win, "Put a path to place for file (with fileName): ");
                    werase(input_win);
                    box(input_win, 0, 0);
                    wrefresh(input_win);

                    std::string data = Encoder::encode(toValue(torrent));
                    createFile(filePath, data);

                    

                    mvwprintw(input_win, 1, 1, "Press any button");
                    wrefresh(input_win);
                    wgetch(input_win);
                    wclear(input_win);
                }
                catch (std::runtime_error& e)
                {
                    werase(input_win);
                    box(input_win, 0, 0);
                    mvwprintw(input_win, 1, 1, "Operations with file failed");
                    mvwprintw(input_win, 2, 1, "Press any button");
                    wrefresh(input_win);
                    wgetch(input_win);
                    delwin(input_win);
                }
                clear();
                refresh();
            }
            else if (choice == menu_items.size())
            {  
                break;
            }
            choice = 0;  
        }
    }

    endwin();
    return 0;
}