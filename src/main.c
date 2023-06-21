#include <ncurses.h>
#include <string.h>

#define MAX_WORKOUTS 365

typedef struct Workout {
    char date[10];
    char training[10];
    char time[10];
} Workout;

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
    attroff(A_BOLD);
    refresh();
    getch();
    return;
}

void addWorkout(Workout *workout, int *workoutNum)
{
    clear();
    addstr("Make new workout entry for date: ");
    refresh();
    getstr(workout[*workoutNum].date);
    printw("You have chosen %s as the date\n", workout[*workoutNum].date);

    addstr("What did you train: ");
    refresh();
    getstr(workout[*workoutNum].training);
    printw("You trained %s.\n", workout[*workoutNum].training);

    addstr("How long was the workout: ");
    refresh();
    getstr(workout[*workoutNum].time);
    printw("You trained for %s.\n", workout[*workoutNum].time);

    refresh();
    getch();
    (*workoutNum)++;

    return;
}

void displayWorkouts(Workout *workout, int workoutNum)
{
    clear();
    mvprintw(0, 0, "Displaying all workouts");
    for (int i = 0; i < workoutNum; i++)
    {
        mvprintw(i + 2, 0, "Date: %s", workout[i].date);
        mvprintw(i + 2, 15, "Time: %s", workout[i].time);
        mvprintw(i + 2, 40, "Training: %s", workout[i].training);
    }
    mvprintw(workoutNum + 3, 0, "Press any key to continue...");
    refresh();
    getch();
}

void invalidInput(const char *errorMessage)
{
    clear();
    attron(A_BOLD);
    mvprintw(0, 0, "Error");
    mvprintw(2, 0, errorMessage);
    mvprintw(4, 0, "Press any key to continue...");
    attroff(A_BOLD);
    refresh();
    getch();
}

int main(void)
{
    Workout workout1[MAX_WORKOUTS];
    int workoutNum = 0;

    initscr();  // Initialize the ncurses library
    keypad(stdscr, TRUE);

    welcomeScreen();

    int choice;
    while (1) {
        clear();
        mvprintw(0, 0, "ALLENO-ORA (TRAIN NOW)");
        mvprintw(2, 0, "1. Add Workout");
        mvprintw(3, 0, "2. Display Workouts");
        mvprintw(4, 0, "3. Exit");
        mvprintw(6, 0, "Enter your choice: ");
        refresh();
        scanw("%d", &choice);

        switch (choice) {
            case 1:
                addWorkout(workout1, &workoutNum);
                break;
            case 2:
                displayWorkouts(workout1, workoutNum);
                break;
            case 3:
                endwin();
                return 0;
            default:
                invalidInput("Invalid choice, please try again.");
                break;
        }
    }
    return 0;
}

