#include <ncurses.h>
#include <string.h>

void center(char *title);

int main(void)
{
    char date[10];

    initscr();  // Initialize the ncurses library
    attron(A_BOLD);
    center("Hello, ready to lift some weights?");
    refresh();
    getch();

    clear();
    addstr("ALLENO-ORA (TRAIN NOW)\n");
    attroff(A_BOLD);

    move(3, 0);
    addstr("Make new workout entry for date: ");
    getstr(date);
    printw("You have chosen %s as the date", date);
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

