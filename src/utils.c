#include <ncurses.h>
#include <string.h>

void center(char *title)
{
    int len, indent, rows, cols;

    getmaxyx(stdscr, rows, cols);
    len = strlen(title);
    indent = cols - len;
    indent /= 2;
    rows =  rows / 2 - 1;
    mvaddstr(rows, indent, title);
    refresh();
}

void welcomeScreen()
{
    attron(A_BOLD);
    center("Ready to lift some weights?");
    refresh();
    getch();
    return;
}
