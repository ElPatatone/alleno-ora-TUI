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

void invalid_input(const char *error_message, WINDOW *menu_window) {
    wclear(menu_window);
    box(menu_window, 0,0);

    wattron(menu_window, A_BOLD);
    mvwprintw(menu_window, 1, 1, "Error");
    mvwprintw(menu_window, 2, 1, "%s", error_message);
    mvwprintw(menu_window, 4, 1, "Press any key to continue...");
    wattroff(menu_window, A_BOLD);

    wrefresh(menu_window);
    noecho();
    getch();
    while (getch() != '\n') {
    }
    echo();
    wclear(menu_window);
}

void welcome_screen() {
    attron(A_BOLD);
    center("Ready to lift some weights?");
    attroff(A_BOLD);
    refresh();
    getch();
    return;
}

void add_workout(Workout *workout, int *workout_number, WINDOW *menu_window) {
    wclear(menu_window);
    box(menu_window, 0, 0);

    while (1) {
        box(menu_window, 0, 0);
        mvwprintw(menu_window, 1, 2, "Add Workout");
        mvwprintw(menu_window, 2, 2, "Date (DD/MM/YYYY): ");
        wrefresh(menu_window);

        wmove(menu_window, 2, 21); // Set the cursor position
        wrefresh(menu_window);
        wgetnstr(menu_window, workout[*workout_number].date, sizeof(workout[*workout_number].date));

        // Check if date is in the format "DD/MM/YYYY"
        int day, month, year;
        if (sscanf(workout[*workout_number].date, "%d/%d/%d", &day, &month, &year) != 3) {
            invalid_input("Invalid date format. Please use the format DD/MM/YYYY.", menu_window);
        } else {
            if (day < 1 || day > 31 || month < 1 || month > 12 || year < 2000 || year > 9999) {
                invalid_input("Invalid date. Please enter a valid date.", menu_window);
            } else {
                break;
            }
        }
    }
    wrefresh(menu_window);

    int iteration1 = 1;
    while (1) {
        box(menu_window, 0, 0);

        if (iteration1 > 1) {
            mvwprintw(menu_window, 1, 2, "Add Workout");
            mvwprintw(menu_window, 2, 2, "Date (DD/MM/YYYY): %s", workout[*workout_number].date);
            mvwprintw(menu_window, 3, 2, "Time (HH:MM): ");
        } else {
            mvwprintw(menu_window, 3, 2, "Time (HH:MM): ");
        }

        wrefresh(menu_window);
        wgetnstr(menu_window, workout[*workout_number].time, sizeof(workout[*workout_number].time));

        // Check if time is in the format "HH:MM"
        int hour, minute;
        if (sscanf(workout[*workout_number].time, "%d:%d", &hour, &minute) != 2) {
            invalid_input("Invalid time format. Please use the format HH:MM.", menu_window);
            iteration1++;
        } else {
            if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
                invalid_input("Invalid time. Please enter a valid time.", menu_window);
                iteration1++;
            } else {
                break;
            }
        }
    }
    wrefresh(menu_window);

    int iteration2 = 1;
    while (1) {
        box(menu_window, 0, 0);

        if (iteration2 > 1) {
            mvwprintw(menu_window, 1, 2, "Add Workout");
            mvwprintw(menu_window, 2, 2, "Date (DD/MM/YYYY): %s", workout[*workout_number].date);
            mvwprintw(menu_window, 3, 2, "Time (HH:MM): %s", workout[*workout_number].time);
            mvwprintw(menu_window, 4, 2, "Duration (minutes): ");
        } else {
            mvwprintw(menu_window, 4, 2, "Duration (minutes): ");
        }

        wrefresh(menu_window);
        char duration_string[10];
        wmove(menu_window, 4, 22); // Set the cursor position
        wrefresh(menu_window);
        wgetstr(menu_window, duration_string);
        sscanf(duration_string, "%d", &workout[*workout_number].duration);

        if (workout[*workout_number].duration <= 0) {
            invalid_input("Invalid input for duration.", menu_window);
            iteration2++;
        } else {
            break;
        }
    }
    wrefresh(menu_window);

    int iteration3 = 1;
    while (1) {
        box(menu_window, 0, 0);

        if (iteration3 > 1) {
            mvwprintw(menu_window, 1, 2, "Add Workout");
            mvwprintw(menu_window, 2, 2, "Date (DD/MM/YYYY): %s", workout[*workout_number].date);
            mvwprintw(menu_window, 3, 2, "Time (HH:MM): %s", workout[*workout_number].time);
            mvwprintw(menu_window, 4, 2, "Duration (minutes): %d", workout[*workout_number].duration);
            mvwprintw(menu_window, 5, 2, "Training done: ");
        } else {
            mvwprintw(menu_window, 5, 2, "Training done: ");
        }

        wrefresh(menu_window);
        wgetnstr(menu_window, workout[*workout_number].training, sizeof(workout[*workout_number].training));

        if (strlen(workout[*workout_number].training) == 0) {
            invalid_input("Training value cannot be empty. Please enter a valid training value.", menu_window);
            iteration3++;
        } else {
            break;
        }
    }

    // Remove the training value appended to the date value (No idea why this happens)
    strtok(workout[*workout_number].date, " \t\n");
    wrefresh(menu_window);
    (*workout_number)++;

    return;
}

void display_workouts(WINDOW *menu_window, const char *directory) {
    wclear(menu_window);
    box(menu_window, 0, 0);

    FILE *file;
    char file_path[100];
    sprintf(file_path, "%s", directory);

    file = fopen(file_path, "r");
    if (file == NULL) {
        mvwprintw(menu_window, 2, 2, "No workouts found. Please add workouts first.");
        wrefresh(menu_window);
        getch();
        return;
    }

    int max_rows, max_columns;
    getmaxyx(menu_window, max_rows, max_columns);

    int top_index = 0;  // Index of the topmost visible workout
    int visible_rows = max_rows - 5;  // Adjust the number of visible rows as needed

    char line[256];
    int workout_numberber = 0;

    wattron(menu_window, A_BOLD);
    mvwprintw(menu_window, 1, 2, "Displaying all workouts");
    wattroff(menu_window, A_BOLD);
    mvwhline(menu_window, 2, 1, ACS_HLINE, max_columns - 2);  // Top border

    while (fgets(line, sizeof(line), file) != NULL) {
        workout_numberber++;
        if (workout_numberber <= top_index)
            continue;

        // Parse the workout information from the line
        char date[15], time[15], training[100];
        int duration;
        sscanf(line, "%*d Date: %[^,], Time: %[^,], Duration: %d, Training: %[^\n]",
               date, time, &duration, training);

        mvwprintw(menu_window, workout_numberber - top_index + 2, 2, "%d - Date: %s", workout_numberber, date);
        mvwprintw(menu_window, workout_numberber - top_index + 2, 25, "Time: %s", time);
        mvwprintw(menu_window, workout_numberber - top_index + 2, 40, "Duration: %d", duration);
        mvwprintw(menu_window, workout_numberber - top_index + 2, 55, "Training: %s", training);

        if (workout_numberber >= top_index + visible_rows)
            break;
    }

    fclose(file);

    if (workout_numberber > visible_rows) {
        mvwprintw(menu_window, visible_rows + 3, 2, "Use UP/DOWN arrow keys to scroll");
    } else {
        mvwprintw(menu_window, visible_rows + 3, 2, "Press any key to continue...");
    }

    wrefresh(menu_window);
    getch();
}

void remove_workouts(Workout *workout, int *workout_number, WINDOW *menu_window, const char *directory){
    wclear(menu_window);
    box(menu_window, 0, 0);

    wattron(menu_window, A_BOLD);
    mvwprintw(menu_window, 1, 2, "Remove a workout");
    wattroff(menu_window, A_BOLD);
    wrefresh(menu_window);
    getch();
}

int display_menu(int window_height, int window_width, WINDOW *menu_window){

    // Text to be centered
    const char* text1 = "1. Add Workout";
    const char* text2 = "2. Remove Workout";
    const char* text3 = "3. Display Workouts";
    const char* text4 = "4. Exit";
    const char* text5 = "5. help";
    const char* text6 = "Enter your choice: ";

    int numLines = 6; // Total number of lines of text
    int text_length_1 = strlen(text1);
    int text_length_2 = strlen(text2);
    int text_length_3 = strlen(text3);
    int text_length_4 = strlen(text4);
    int text_length_5 = strlen(text5);
    int text_length_6 = strlen(text6);

    int centerX = (window_width - text_length_1) / 2; // Calculate the center position
    int centerY = (window_height - numLines) / 2; // Calculate the center position

    mvwprintw(menu_window, centerY, centerX, "%s", text1);
    mvwprintw(menu_window, centerY + 1, centerX, "%s", text2);
    mvwprintw(menu_window, centerY + 2, centerX, "%s", text3);
    mvwprintw(menu_window, centerY + 3, centerX, "%s", text4);
    mvwprintw(menu_window, centerY + 4, centerX, "%s", text5);
    mvwprintw(menu_window, centerY + 5, centerX, "%s", text6);
    wrefresh(menu_window);

    // resetting the cursor to be in the menu window
    wmove(menu_window, centerY + 5, centerX + text_length_6);
    wrefresh(menu_window);

    char choice_string[10];
    wgetstr(menu_window, choice_string);
    int choice = atoi(choice_string);

    wrefresh(menu_window);
    return choice;
}

int compare_date(const void *a, const void *b) {
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

void save_workouts_to_file(Workout *workouts, int length, const char *directory) {
    FILE *file;
    char file_path[100];
    sprintf(file_path, "%s", directory);

    // Read existing_ workouts from the file
    file = fopen(file_path, "r");
    Workout existing_workouts[MAX_WORKOUTS];
    char line[256];
    int existing_length = 0;
    if (file != NULL) {
        while (fgets(line, sizeof(line), file) != NULL && existing_length < MAX_WORKOUTS) {
            sscanf(line, "%*d Date: %[^,], Time: %[^,], Duration: %d, Training: %[^\n]",
                   existing_workouts[existing_length].date, existing_workouts[existing_length].time,
                   &existing_workouts[existing_length].duration, existing_workouts[existing_length].training);
            existing_length++;
        }
        fclose(file);
    }

    existing_workouts[existing_length] = workouts[length - 1];
    existing_length++;

    qsort(existing_workouts, existing_length, sizeof(Workout), compare_date);

    file = fopen(file_path, "w");
    if (file == NULL) {
        printf("Failed to open file for writing.\n");
        return;
    }

    for (int i = 0; i < existing_length; i++) {
        int workout_numberber = i + 1;
        fprintf(file, "%d Date: %s, Time: %s, Duration: %d, Training: %s\n",
                workout_numberber, existing_workouts[i].date, existing_workouts[i].time,
                existing_workouts[i].duration, existing_workouts[i].training);
    }

    fclose(file);
    printf("Workouts saved to file.\n");
}

int main(void) {
    Workout workout1[MAX_WORKOUTS];
    int workout_number = 0;

    initscr();
    keypad(stdscr, TRUE);

    welcome_screen();

    FILE *config_file;
    char directory[100] = "";
    config_file = fopen("config.txt", "r");
    if (config_file != NULL) {
        fgets(directory, sizeof(directory), config_file);
        // Remove newline character if present
        if (directory[strlen(directory) - 1] == '\n') {
            directory[strlen(directory) - 1] = '\0';
        }
        fclose(config_file);
    }

    int choice;
    while (1) {
        clear();
        mvprintw(0, 0, "ALLENO-ORA (TRAIN NOW)");

        int terminal_width, terminal_height;
        getmaxyx(stdscr, terminal_height, terminal_width);

        int window_height = terminal_height - WINDOW_MARGIN;
        int window_width = terminal_width - WINDOW_MARGIN;

        WINDOW *menu_window = newwin(window_height, window_width, WINDOW_MARGIN, WINDOW_MARGIN);
        box(menu_window, 0, 0);
        refresh();
        wrefresh(menu_window);

        choice = display_menu(window_height, window_width, menu_window);

        switch (choice) {
            case 1:
                add_workout(workout1, &workout_number, menu_window);
                save_workouts_to_file(workout1, workout_number, directory);
                break;
            case 2:
                remove_workouts(workout1, &workout_number, menu_window, directory);
                break;
            case 3:
                display_workouts(menu_window, directory);
                break;
            case 4:
                endwin();
                return 0;
            default:
                invalid_input("Invalid choice, please try again.", menu_window);
                break;
        }
    }
    return 0;
}
