#include <ncurses.h>
#include <string.h>

#define MAX_WORKOUTS 365

void center(char *title);
void welcomeScreen();

typedef struct Workout {

    char date[10];
    char training[10];
    char time[10];

} Workout;

int main(void)
{
    Workout workout1[MAX_WORKOUTS];
    int workoutNum = 0;

    initscr();  // Initialize the ncurses library

    welcomeScreen();

    clear();
    addstr("ALLENO-ORA (TRAIN NOW)\n");
    attroff(A_BOLD);

    move(3, 0);
    addstr("Make new workout entry for date: ");
    getstr(workout1[workoutNum].date);
    printw("You have chosen %s as the date\n", workout1[workoutNum].date);

    addstr("What did you train: ");
    getstr(workout1[workoutNum].training);
    printw("You trained %s.\n", workout1[workoutNum].training);

    addstr("How long was the workout: ");
    getstr(workout1[workoutNum].time);
    printw("You trained for %s.\n", workout1[workoutNum].time);

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

void welcomeScreen()
{
    attron(A_BOLD);
    center("Hello, ready to lift some weights?");
    refresh();
    getch();
    return;
}
