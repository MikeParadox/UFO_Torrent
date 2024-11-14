#include "ufo_torrent.h"

#include <iostream>
#include <ncurses.h>
#include <string>
#include <fstream>

#include "../includes/prettyPrinter.h"
#include "../includes/decode.h"
#include "../includes/torrentFile.h"
#include "../includes/encode.h"
#include <filesystem>

using namespace bencode;
using namespace Torrent;

int main()
{
	int ch;

	initscr();			/* Start curses mode 		*/
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */

	printw("Type any character to see it in bold\n");
	ch = getch();			/* If raw() hadn't been called
					 * we have to press enter before it
					 * gets to the program 		*/
	if (ch == KEY_F(1))		/* Without keypad enabled this will */
		printw("F1 Key pressed");/*  not get to us either	*/
	/* Without noecho() some ugly escape
	 * charachters might have been printed
	 * on screen			*/
	else
	{
		printw("The pressed key is ");
		attron(A_BOLD);
		printw("%c", ch);
		attroff(A_BOLD);
	}
	refresh();			/* Print it on to the real screen */
	getch();			/* Wait for user input */
	endwin();			/* End curses mode		  */
    TorrentFile file = parseTorrentFile(Decoder::decode(readFile("exemple.torrent")));
    Value valueFile = toValue(file);

    boost::apply_visitor(PrettyPrinter(), valueFile);
	return 0;
}