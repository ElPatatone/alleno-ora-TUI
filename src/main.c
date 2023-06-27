#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

#define MAX_WORKOUTS 365
#define WINDOW_MARGIN 2

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

void invalidInput(const char *errorMessage, WINDOW *menuWin) {
    wclear(menuWin);
    box(menuWin, 0,0);

    wattron(menuWin, A_BOLD);
    mvwprintw(menuWin, 1, 1, "Error");
    mvwprintw(menuWin, 2, 1, errorMessage);
    mvwprintw(menuWin, 4, 1, "Press any key to continue...");
    wattroff(menuWin, A_BOLD);

    wrefresh(menuWin);
    noecho();
    getch();
    while (getch() != '\n') {
    }
    echo();
    wclear(menuWin);
}

void welcomeScreen() {
    attron(A_BOLD);
    center("Ready to lift some weights?");
    attroff(A_BOLD);
    refresh();
    getch();
    return;
}

void addWorkout(Workout *workout, int *workoutNum, WINDOW *menuWin) {
    wclear(menuWin);
    box(menuWin, 0, 0);

    while (1) {
        box(menuWin, 0, 0);
        mvwprintw(menuWin, 1, 2, "Add Workout");
        mvwprintw(menuWin, 2, 2, "Date (DD/MM/YYYY): ");
        wrefresh(menuWin);

        wmove(menuWin, 2, 21); // Set the cursor position
        wrefresh(menuWin);
        wgetnstr(menuWin, workout[*workoutNum].date, sizeof(workout[*workoutNum].date));

        // Check if date is in the format "DD/MM/YYYY"
        int day, month, year;
        if (sscanf(workout[*workoutNum].date, "%d/%d/%d", &day, &month, &year) != 3) {
            invalidInput("Invalid date format. Please use the format DD/MM/YYYY.", menuWin);
        } else {
            if (day < 1 || day > 31 || month < 1 || month > 12 || year < 2000 || year > 9999) {
                invalidInput("Invalid date. Please enter a valid date.", menuWin);
            } else {
                break;
            }
        }
    }
    wrefresh(menuWin);

    int iteration1 = 1;
    while (1) {
        box(menuWin, 0, 0);

        if (iteration1 > 1) {
            mvwprintw(menuWin, 1, 2, "Add Workout");
            mvwprintw(menuWin, 2, 2, "Date (DD/MM/YYYY): %s", workout[*workoutNum].date);
            mvwprintw(menuWin, 3, 2, "Time (HH:MM): ");
        } else {
            mvwprintw(menuWin, 3, 2, "Time (HH:MM): ");
        }

        wrefresh(menuWin);
        wgetnstr(menuWin, workout[*workoutNum].time, sizeof(workout[*workoutNum].time));

        // Check if time is in the format "HH:MM"
        int hour, minute;
        if (sscanf(workout[*workoutNum].time, "%d:%d", &hour, &minute) != 2) {
            invalidInput("Invalid time format. Please use the format HH:MM.", menuWin);
            iteration1++;
        } else {
            if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
                invalidInput("Invalid time. Please enter a valid time.", menuWin);
                iteration1++;
            } else {
                break;
            }
        }
    }
    wrefresh(menuWin);

    int iteration2 = 1;
    while (1) {
        box(menuWin, 0, 0);

        if (iteration2 > 1) {
            mvwprintw(menuWin, 1, 2, "Add Workout");
            mvwprintw(menuWin, 2, 2, "Date (DD/MM/YYYY): %s", workout[*workoutNum].date);
            mvwprintw(menuWin, 3, 2, "Time (HH:MM): %s", workout[*workoutNum].time);
            mvwprintw(menuWin, 4, 2, "Duration (minutes): ");
        } else {
            mvwprintw(menuWin, 4, 2, "Duration (minutes): ");
        }

        wrefresh(menuWin);
        char durationString[10];
        wmove(menuWin, 4, 22); // Set the cursor position
        wrefresh(menuWin);
        wgetstr(menuWin, durationString);
        sscanf(durationString, "%d", &workout[*workoutNum].duration);

        if (workout[*workoutNum].duration <= 0) {
            invalidInput("Invalid input for duration.", menuWin);
            iteration2++;
        } else {
            break;
        }
    }
    wrefresh(menuWin);

    int iteration3 = 1;
    while (1) {
        box(menuWin, 0, 0);

        if (iteration3 > 1) {
            mvwprintw(menuWin, 1, 2, "Add Workout");
            mvwprintw(menuWin, 2, 2, "Date (DD/MM/YYYY): %s", workout[*workoutNum].date);
            mvwprintw(menuWin, 3, 2, "Time (HH:MM): %s", workout[*workoutNum].time);
            mvwprintw(menuWin, 4, 2, "Duration (minutes): %d", workout[*workoutNum].duration);
            mvwprintw(menuWin, 5, 2, "Training done: ");
        } else {
            mvwprintw(menuWin, 5, 2, "Training done: ");
        }

        wrefresh(menuWin);
        wgetnstr(menuWin, workout[*workoutNum].training, sizeof(workout[*workoutNum].training));

        if (strlen(workout[*workoutNum].training) == 0) {
            invalidInput("Training value cannot be empty. Please enter a valid training value.", menuWin);
            iteration3++;
        } else {
            break;
        }
    }

    // Remove the training value appended to the date value (No idea why this happens)
    strtok(workout[*workoutNum].date, " \t\n");
    wrefresh(menuWin);
    (*workoutNum)++;
    return;
}

void displayWorkouts(WINDOW *menuWin) {
    wclear(menuWin);
    box(menuWin, 0, 0);
    mvwprintw(menuWin, 1, 2, "Displaying all workouts");

    FILE *file = fopen("workouts.txt", "r");
    if (file == NULL) {
        mvwprintw(menuWin, 2, 2, "No workouts found. Please add workouts first.");
        wrefresh(menuWin);
        getch();
        return;
    }

    int maxRows, maxCols;
    getmaxyx(menuWin, maxRows, maxCols);

    int topIndex = 0;  // Index of the topmost visible workout
    int numVisibleRows = maxRows - 5;  // Adjust the number of visible rows as needed

    char line[256];
    int workoutNumber = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        workoutNumber++;
        if (workoutNumber <= topIndex)
            continue;

        // Parse the workout information from the line
        char date[15], time[15], training[100];
        int duration;
        sscanf(line, "%*d Date: %[^,], Time: %[^,], Duration: %d, Training: %[^\n]",
               date, time, &duration, training);

        mvwprintw(menuWin, workoutNumber - topIndex + 1, 2, "%d - Date: %s", workoutNumber, date);
        mvwprintw(menuWin, workoutNumber - topIndex + 1, 25, "Time: %s", time);
        mvwprintw(menuWin, workoutNumber - topIndex + 1, 40, "Duration: %d", duration);
        mvwprintw(menuWin, workoutNumber - topIndex + 1, 55, "Training: %s", training);

        if (workoutNumber >= topIndex + numVisibleRows)
            break;
    }

    fclose(file);

    if (workoutNumber > numVisibleRows) {
        mvwprintw(menuWin, numVisibleRows + 3, 2, "Use UP/DOWN arrow keys to scroll");
    } else {
        mvwprintw(menuWin, numVisibleRows + 3, 2, "Press any key to continue...");
    }

    wrefresh(menuWin);
    getch();
}

int displayMenu(int windowHeight, int windowWidth, WINDOW *menuWin){

    // Text to be centered
    const char* text1 = "1. Add Workout";
    const char* text2 = "2. Display Workouts";
    const char* text3 = "3. Exit";
    const char* text4 = "4. help";
    const char* text5 = "Enter your choice: ";

    int numLines = 5; // Total number of lines of text
    int textLength1 = strlen(text1);
    int textLength2 = strlen(text2);
    int textLength3 = strlen(text3);
    int textLength4 = strlen(text4);
    int textLength5 = strlen(text5);

    int centerX = (windowWidth - textLength1) / 2; // Calculate the center position
    int centerY = (windowHeight - numLines) / 2; // Calculate the center position

    mvwprintw(menuWin, centerY, centerX, text1);
    mvwprintw(menuWin, centerY + 1, centerX, text2);
    mvwprintw(menuWin, centerY + 2, centerX, text3);
    mvwprintw(menuWin, centerY + 3, centerX, text4);
    mvwprintw(menuWin, centerY + 4, centerX, text5);
    wrefresh(menuWin);

    // resetting the cursor to be in the menu window
    wmove(menuWin, centerY + 4, centerX + textLength5);
    wrefresh(menuWin);

    char choiceString[10];
    wgetstr(menuWin, choiceString);
    int choice = atoi(choiceString);

    wrefresh(menuWin);
    return choice;
}

int compareByDate(const void *a, const void *b) {
    const Workout *workoutA = (const Workout *)a;
    const Workout *workoutB = (const Workout *)b;

    // Parse the date strings
    int dayA, monthA, yearA;
    int dayB, monthB, yearB;
    sscanf(workoutA->date, "%d/%d/%d", &dayA, &monthA, &yearA);
    sscanf(workoutB->date, "%d/%d/%d", &dayB, &monthB, &yearB);

    // Compare year first
    if (yearA > yearB) {
        return -1;
    } else if (yearA < yearB) {
        return 1;
    }

    // Compare month next (reversed)
    if (monthA > monthB) {
        return -1;
    } else if (monthA < monthB) {
        return 1;
    }

    // Compare day last (reversed)
    if (dayA > dayB) {
        return -1;
    } else if (dayA < dayB) {
        return 1;
    }
    // Dates are equal
    return 0;
}

void saveWorkoutsToFile(Workout *workouts, int length) {
    FILE *file = fopen("workouts.txt", "a+");
    if (file == NULL) {
        printf("Failed to open file for writing.\n");
        return;
    }

    // Write the sorted workouts to the file
    for (int i = 0; i < length; i++) {
        int workoutNumber = i + 1;
        fprintf(file, "%d Date: %s, Time: %s, Duration: %d, Training: %s\n",
                workoutNumber, workouts[i].date, workouts[i].time,
                workouts[i].duration, workouts[i].training);
    }

    fclose(file);
    printf("Workouts saved to file.\n");
}

void orderFileByDate() {
    FILE *file = fopen("workouts.txt", "r+");
    if (file == NULL) {
        printf("Failed to open file for reading and writing.\n");
        return;
    }

    char line[256];
    Workout workouts[100];  // Assuming a maximum of 100 workouts

    int i = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        sscanf(line, "%*d Date: %[^,], Time: %[^,], Duration: %d, Training: %[^\n]",
               workouts[i].date, workouts[i].time, &workouts[i].duration, workouts[i].training);
        i++;
    }

    int numWorkouts = i;
    qsort(workouts, numWorkouts, sizeof(Workout), compareByDate);

    fseek(file, 0, SEEK_SET);

    for (i = 0; i < numWorkouts; i++) {
        int workoutNumber = i + 1;
        fprintf(file, "%d Date: %s, Time: %s, Duration: %d, Training: %s\n",
                workoutNumber, workouts[i].date, workouts[i].time,
                workouts[i].duration, workouts[i].training);
    }

    int truncateResult = ftruncate(fileno(file), ftell(file));
    if (truncateResult != 0) {
        printf("Failed to truncate the file.\n");
    }

    fclose(file);
    printf("Workouts sorted and saved to workouts.txt.\n");
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
        // making a window for the menu
        int terminalWidth, terminalHeight;
        getmaxyx(stdscr, terminalHeight, terminalWidth); // Get terminal dimensions

        int windowHeight = terminalHeight - WINDOW_MARGIN;
        int windowWidth = terminalWidth - WINDOW_MARGIN;

        // Create a new window with the calculated dimensions and position
        WINDOW *menuWin = newwin(windowHeight, windowWidth, WINDOW_MARGIN, WINDOW_MARGIN);
        box(menuWin, 0, 0); // Draw solid borders around the window
        refresh();
        wrefresh(menuWin);

        choice = displayMenu(windowHeight, windowWidth, menuWin);

        switch (choice) {
            case 1:
                addWorkout(workout1, &workoutNum, menuWin);
                saveWorkoutsToFile(workout1, workoutNum);
                orderFileByDate();
                break;
            case 2:
                displayWorkouts(menuWin);
                break;
            case 3:
                endwin();
                return 0;
            default:
                invalidInput("Invalid choice, please try again.", menuWin);
                break;
        }
    }
    return 0;
}

