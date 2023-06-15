#include <ncurses.h>
#include <string.h>

void center(char *title);

int main(void)
{
    initscr();  // Initialize the ncurses library
    attron(A_BOLD);
    center("Hello, ready to lift some weights?");
    refresh();
    getch();

    clear();
    addstr("ALLENO-ORA (TRAIN NOW)\n");
    attroff(A_BOLD);
    addstr("Make new entry for: ");
    refresh();
    getch();
    endwin();  // End the ncurses mode
    return 0;
}

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

