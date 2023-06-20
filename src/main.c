#include <ncurses.h>
#include <string.h>
#include "utils.h"

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

