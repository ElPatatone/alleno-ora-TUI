#include <ncurses.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORKOUTS 365

typedef struct Workout {

    char date[15];
    char training[100];
    char time[15];
    int duration;

} Workout;

void center(char *title) {
    int len, indent, rows, cols;

    getmaxyx(stdscr, rows, cols);
    len = strlen(title);
    indent = cols - len;
    indent /= 2;
    rows =  rows / 2 - 1;
    mvaddstr(rows, indent, title);
    refresh();
}

void invalidInput(const char *errorMessage) {
    clear();
    attron(A_BOLD);
    mvprintw(0, 0, "Error");
    mvprintw(2, 0, errorMessage);
    mvprintw(4, 0, "Press any key to continue...");
    attroff(A_BOLD);
    refresh();
    getch();
    clear();
}

void welcomeScreen() {
    attron(A_BOLD);
    center("Ready to lift some weights?");
    attroff(A_BOLD);
    refresh();
    getch();
    return;
}

void addWorkout(Workout *workout, int *workoutNum) {
    clear();

    int iteration1 = 1;
    while (1) {
        // if (iteration1 > 1) {
        //     printw("Training done: %s\n", workout[*workoutNum].training);
        // }
        addstr("Date (DD/MM/YYYY): ");
        refresh();
        getnstr(workout[*workoutNum].date, sizeof(workout[*workoutNum].date));

        // Check if date is in the format "DD/MM/YYYY"
        int day, month, year;
        if (sscanf(workout[*workoutNum].date, "%d/%d/%d", &day, &month, &year) != 3) {
            invalidInput("Invalid date format. Please use the format DD/MM/YYYY.");
            refresh();
            // iteration1++;
        } else {
            if (day < 1 || day > 31 || month < 1 || month > 12 || year < 2000 || year > 9999) {
                invalidInput("Invalid date. Please enter a valid date.");
                refresh();
                // iteration1++;
            } else {
                break;
            }
        }
    }
    printw("You have chosen %s as the date\n", workout[*workoutNum].date);

    int iteration2 = 1;
    while (1) {
        // if (iteration2 > 1) {
        //     printw("Training done: %s\n", workout[*workoutNum].training);
        //     printw("Date (DD/MM/YYYY): %s\n", workout[*workoutNum].date);
        // }
        addstr("Time (HH:MM): ");
        refresh();
        getnstr(workout[*workoutNum].time, sizeof(workout[*workoutNum].time));

        // Check if date is in the format "DD/MM/YYYY"
        int hour, minute;
        if (sscanf(workout[*workoutNum].time, "%d:%d", &hour, &minute) != 2) {
            invalidInput("Invalid time format. Please use the format HH:MM.");
            refresh();
            // iteration2++;
        } else {
            if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
                invalidInput("Invalid time. Please enter a valid time.");
                refresh();
                // iteration2++;
            } else {
                break;
            }
        }
    }
    printw("You trained at %s.\n", workout[*workoutNum].time);

    int iteration3 = 1;
    while (1){
        // if (iteration3> 1) {
        //     printw("Training done: %s\n", workout[*workoutNum].training);
        //     printw("Date (DD/MM/YYYY) %s\n", workout[*workoutNum].date);
        //     printw("Time (HH:MM): %s\n", workout[*workoutNum].time);
        // }
        addstr("Duration (minutes): ");
        refresh();

        if (scanw("%d", &workout[*workoutNum].duration) != 1 || workout[*workoutNum].duration <= 0) {
            invalidInput("Invalid input for duration.");
            // iteration3++;
        } else {
            break;
        }
    }
    printw("You trained for %d.\n", workout[*workoutNum].duration);

    //getting training value
    addstr("Training done: ");
    refresh();
    getnstr(workout[*workoutNum].training, sizeof(workout[*workoutNum].training));
    // remove the training value appended to the date value (No idea why this happens)
    strtok(workout[*workoutNum].date, " \t\n");
    printw("You trained %s.\n", workout[*workoutNum].training);

    refresh();
    getch();

    (*workoutNum)++;
    return;
}

void displayWorkouts(Workout *workout, int length) { 
    clear();
    mvprintw(0, 0, "Displaying all workouts");


    for (int i = 0; i < length; i++) {
        mvprintw(i + 2, 0, "Date: %s", workout[i].date);
        mvprintw(i + 2, 20, "Time: %s", workout[i].time);
        mvprintw(i + 2, 40, "Duration: %d", workout[i].duration);
        mvprintw(i + 2, 55, "Training: %s", workout[i].training);
    }

    mvprintw(length + 3, 0, "Press any key to continue...");
    refresh();
    getch();
}


int main(void) {
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

