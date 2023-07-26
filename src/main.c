#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <sqlite3.h>
#include <stdbool.h>

#define MAX_WORKOUTS 365
#define MAX_LIFTS 10
#define WINDOW_MARGIN 2

typedef struct Lift {

    char name[100];
    int reps;
    int sets;
    int weight;

} Lift;

typedef struct Workout {

    char date[15];
    char training[100];
    char time[15];
    int duration;
    int number_of_lifts;
    Lift lift[MAX_LIFTS];
    Lift pr;

} Workout;

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
    while (getch() != '\n') {
    }
    echo();
    wclear(menu_window);
}

void add_pr(sqlite3 *db, WINDOW *menu_window) {

    wclear(menu_window);

    char date[11];  // Buffer to store the date
    int ch;

    while(1){
        box(menu_window, 0, 0);

        wattron(menu_window, A_BOLD);
        mvwprintw(menu_window, 1, 2, "Add PR");
        wattroff(menu_window, A_BOLD);
        mvwhline(menu_window, 2, 1, ACS_HLINE, getmaxx(menu_window) - 2);

        mvwprintw(menu_window, 3, 2, "Enter the date of the PR (YYYY/MM/DD): ");
        wmove(menu_window, 3, 41); // Set the cursor position
        wrefresh(menu_window);
        mvwgetnstr(menu_window, 3, 41, date, sizeof(date));

        if (strlen(date) == 0) {
            return;
        }

        int year, month, day;
        if (sscanf(date, "%d/%d/%d", &year, &month, &day) != 3 ||
            day < 1 || day > 31 || month < 1 || month > 12 || year < 2000 || year > 9999) {
            invalid_input("Invalid date format or invalid date. Please enter a valid date.", menu_window);
        }else {
            break;
        }
    }

    char select_workout_query[100];
    sprintf(select_workout_query, "SELECT date FROM workouts WHERE date = '%s';", date);
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, select_workout_query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to execute select query: %s\n", sqlite3_errmsg(db));
        return;
    }
    if(sqlite3_step(stmt) == SQLITE_ROW){
        sqlite3_finalize(stmt);
        char pr_name[100];
        char pr_weight_str[100];

        wclear(menu_window);
        box(menu_window, 0, 0);

        wattron(menu_window, A_BOLD);
        mvwprintw(menu_window, 1, 2, "Add PR for %s", date);
        wattroff(menu_window, A_BOLD);
        mvwhline(menu_window, 2, 1, ACS_HLINE, getmaxx(menu_window) - 2);

        mvwprintw(menu_window, 3, 2, "Enter the name of the PR: ");
        wmove(menu_window, 3, 28);
        wrefresh(menu_window);
        wgetstr(menu_window, pr_name);
        if (strlen(pr_name) == 0) {
            return;
        }

        mvwprintw(menu_window, 4, 2, "Enter the weight of the PR (kg): ");
        wmove(menu_window, 4, 35);
        wrefresh(menu_window);
        wgetstr(menu_window, pr_weight_str);

        if (strlen(pr_weight_str) == 0) {
            return;
        }

        int pr_weight = atoi(pr_weight_str);
        wrefresh(menu_window);

        char insert_pr_query[200];
        sprintf(insert_pr_query, "INSERT INTO lifts (date, name, weight, is_pr) VALUES ('%s', '%s', %d, 1);", date, pr_name, pr_weight);
        rc = sqlite3_exec(db, insert_pr_query, NULL, 0, NULL);
        if (rc != SQLITE_OK) {
            printf("Failed to execute insert query: %s\n", sqlite3_errmsg(db));
            return;
        }
        wattron(menu_window, A_BOLD);
        mvwprintw(menu_window, 8, 2, "PR added successfully. Press any key to conitue...");
        wattroff(menu_window, A_BOLD);
        wrefresh(menu_window);
        getch();
        return;
    }
    else {
        sqlite3_finalize(stmt);
        wattron(menu_window, A_BOLD);
        mvwprintw(menu_window, 8,2, "No Workout found for the entered date. Press any key continue...");
        wattroff(menu_window, A_BOLD);
        wrefresh(menu_window);
        getch();
        return;
    }
    sqlite3_finalize(stmt);
    getch();

}

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


void welcome_screen() {
    attron(A_BOLD);
    center("Ready to lift some weights?");
    attroff(A_BOLD);
    refresh();
    getch();
    return;
}

void insert_workout(sqlite3 *db, Workout *workout) {

    char insert_query[300];
    snprintf(insert_query, sizeof(insert_query), "INSERT INTO workouts (date, time, duration, training) VALUES ('%s', '%s', %d, '%s');",
            workout->date, workout->time, workout->duration, workout->training);

    char *error_message = NULL;
    int rc = sqlite3_exec(db, insert_query, 0, 0, &error_message);
    if (rc != SQLITE_OK) {
        printf("Failed to insert workout: %s\n", error_message);
        sqlite3_free(error_message);
    }

    // Apply ORDER BY clause to retrieve data in the desired order
    char *order_query = "SELECT * FROM workouts ORDER BY date DESC;";
    rc = sqlite3_exec(db, order_query, 0, 0, &error_message);
    if (rc != SQLITE_OK) {
        printf("Failed to retrieve workouts: %s\n", error_message);
        sqlite3_free(error_message);
    }
}

void add_workout(sqlite3 *db, Workout *workout, int *workout_number, WINDOW *menu_window) {
    wclear(menu_window);
    box(menu_window, 0, 0);

    while (1) {
        box(menu_window, 0, 0);
        wattron(menu_window, A_BOLD);
        mvwprintw(menu_window, 1, 2, "Add Workout");
        wattroff(menu_window, A_BOLD);
        mvwhline(menu_window, 2, 1, ACS_HLINE, getmaxx(menu_window) - 2);

        mvwprintw(menu_window, 3, 2, "Date (YYYY/MM/DD): ");
        wrefresh(menu_window);

        wmove(menu_window, 3, 21); // Set the cursor position
        wrefresh(menu_window);
        wgetnstr(menu_window, workout[*workout_number].date, sizeof(workout[*workout_number].date));

        if (strlen(workout[*workout_number].date) == 0) {
            return;
        }

        int year, month, day;
        if (sscanf(workout[*workout_number].date, "%d/%d/%d", &year, &month, &day) != 3) {
            invalid_input("Invalid date format. Please use the format YYYY/MM/DD.", menu_window);
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
            wattron(menu_window, A_BOLD);
            mvwprintw(menu_window, 1, 2, "Add Workout");
            wattroff(menu_window, A_BOLD);
            mvwhline(menu_window, 2, 1, ACS_HLINE, getmaxx(menu_window) - 2);

            mvwprintw(menu_window, 3, 2, "Date (YYYY/MM/DD): %s", workout[*workout_number].date);
            mvwprintw(menu_window, 4, 2, "Time (HH:MM): ");
        } else {
            mvwprintw(menu_window, 4, 2, "Time (HH:MM): ");
        }

        wrefresh(menu_window);
        wgetnstr(menu_window, workout[*workout_number].time, sizeof(workout[*workout_number].time));

        if (strlen(workout[*workout_number].time) == 0) {
            return;
        }

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
            wattron(menu_window, A_BOLD);
            mvwprintw(menu_window, 1, 2, "Add Workout");
            wattroff(menu_window, A_BOLD);
            mvwhline(menu_window, 2, 1, ACS_HLINE, getmaxx(menu_window) - 2);

            mvwprintw(menu_window, 3, 2, "Date (YYYY/MM/DD): %s", workout[*workout_number].date);
            mvwprintw(menu_window, 4, 2, "Time (HH:MM): %s", workout[*workout_number].time);
            mvwprintw(menu_window, 5, 2, "Duration (minutes): ");
        } else {
            mvwprintw(menu_window, 5, 2, "Duration (minutes): ");
        }

        wrefresh(menu_window);
        char duration_string[10];
        wmove(menu_window, 5, 22); // Set the cursor position
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
            wattron(menu_window, A_BOLD);
            mvwprintw(menu_window, 1, 2, "Add Workout");
            wattroff(menu_window, A_BOLD);
            mvwhline(menu_window, 2, 1, ACS_HLINE, getmaxx(menu_window) - 2);

            mvwprintw(menu_window, 3, 2, "Date (YYYY/MM/DD): %s", workout[*workout_number].date);
            mvwprintw(menu_window, 4, 2, "Time (HH:MM): %s", workout[*workout_number].time);
            mvwprintw(menu_window, 5, 2, "Duration (minutes): %d", workout[*workout_number].duration);
            mvwprintw(menu_window, 6, 2, "Training done: ");
        } else {
            mvwprintw(menu_window, 6, 2, "Training done: ");
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

    insert_workout(db, &workout[*workout_number - 1]);
    return;
}


void display_workouts(sqlite3 *db, WINDOW *menu_window) {
    wclear(menu_window);
    box(menu_window, 0, 0);

    char *select_query = "SELECT * FROM workouts ORDER BY date DESC;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, select_query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to execute select query: %s\n", sqlite3_errmsg(db));
        return;
    }

    int max_rows, max_columns;
    getmaxyx(menu_window, max_rows, max_columns);

    int top_index = 0;  // Index of the topmost visible workout
    int visible_rows = max_rows - 5;  // Adjust the number of visible rows as needed

    char line[256];
    int workout_number = 0;

    wattron(menu_window, A_BOLD);
    mvwprintw(menu_window, 1, 2, "Display Workouts");
    wattroff(menu_window, A_BOLD);
    mvwhline(menu_window, 2, 1, ACS_HLINE, max_columns - 2);  // Top border

    int ch;
    do {
        wclear(menu_window);
        box(menu_window, 0, 0);

        switch (ch) {
            case KEY_UP:
                top_index = (top_index > 0) ? top_index - 1 : 0;
                break;
            case KEY_DOWN:
                if (workout_number >= visible_rows)
                    top_index++;
                break;
            default:
                break;
        }

        wattron(menu_window, A_BOLD);
        mvwprintw(menu_window, 1, 2, "Displaying all workouts");
        wattroff(menu_window, A_BOLD);
        mvwhline(menu_window, 2, 1, ACS_HLINE, max_columns - 2);  // Top border

        workout_number = 0;
        sqlite3_reset(stmt);  // Reset statement to re-execute the query

        while (sqlite3_step(stmt) == SQLITE_ROW && workout_number < top_index + visible_rows) {
            workout_number++;
            if (workout_number <= top_index)
                continue;

            int id = sqlite3_column_int(stmt, 0);
            const unsigned char *date = sqlite3_column_text(stmt, 1);
            const unsigned char *time = sqlite3_column_text(stmt, 2);
            int duration = sqlite3_column_int(stmt, 3);
            const unsigned char *training = sqlite3_column_text(stmt, 4);

            mvwprintw(menu_window, workout_number - top_index + 2, 2, "- Date: %s", date);
            mvwprintw(menu_window, workout_number - top_index + 2, 25, "Time: %s", time);
            mvwprintw(menu_window, workout_number - top_index + 2, 40, "Duration: %d", duration);
            mvwprintw(menu_window, workout_number - top_index + 2, 55, "Training: %s", training);
            if (workout_number >= top_index + visible_rows)
                break;
        }

        if (workout_number > visible_rows) {
            mvwprintw(menu_window, visible_rows + 3, 2, "Use UP/DOWN arrow keys to scroll");
        } else {
            mvwprintw(menu_window, visible_rows + 3, 2, "Press Escape key to continue...");
        }

        wrefresh(menu_window);
    } while ((ch = getch()) != 27);  // Exit on Escape key press (27 is the Escape key code)

    sqlite3_finalize(stmt);
}

void remove_workouts(sqlite3 *db, WINDOW *menu_window) {
    wclear(menu_window);

    char date[11];  // Buffer to store the date
    int ch;

    while(1){
        box(menu_window, 0, 0);

        wattron(menu_window, A_BOLD);
        mvwprintw(menu_window, 1, 2, "Remove a workout");
        wattroff(menu_window, A_BOLD);
        mvwhline(menu_window, 2, 1, ACS_HLINE, getmaxx(menu_window) - 2);

        mvwprintw(menu_window, 3, 2, "Enter the date of the workout to remove: ");
        wmove(menu_window, 3, 43); // Set the cursor position
        wrefresh(menu_window);
        mvwgetnstr(menu_window, 3, 43, date, sizeof(date));

        if (strlen(date) == 0) {
            return;
        }

        int year, month, day;
        if (sscanf(date, "%d/%d/%d", &year, &month, &day) != 3 ||
            day < 1 || day > 31 || month < 1 || month > 12 || year < 2000 || year > 9999) {
            invalid_input("Invalid date format or invalid date. Please enter a valid date.", menu_window);
        }else {
            break;
        }
    }

    // Prepare the DELETE statement
    sqlite3_stmt *stmt;
    char *delete_query = "DELETE FROM workouts WHERE date = ?";
    int rc = sqlite3_prepare_v2(db, delete_query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to prepare DELETE statement: %s\n", sqlite3_errmsg(db));
        curs_set(0);  // Hide the cursor
        noecho();  // Disable input echoing
        return;
    }

    // Bind the date parameter
    rc = sqlite3_bind_text(stmt, 1, date, -1, SQLITE_STATIC);
    if (rc != SQLITE_OK) {
        printf("Failed to bind date parameter: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        curs_set(0);  // Hide the cursor
        noecho();  // Disable input echoing
        return;
    }

    // Execute the DELETE statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        printf("Failed to execute DELETE statement: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        curs_set(0);  // Hide the cursor
        noecho();  // Disable input echoing
        return;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
    wrefresh(menu_window);
}

int display_menu(int window_height, int window_width, WINDOW *menu_window){

    // Text to be centered
    const char* text1 = "1. Add Workout";
    const char* text2 = "2. Remove Workout";
    const char* text3 = "3. Display Workouts";
    const char* text4 = "4. Add PR";
    const char* text5 = "5. Exit";
    const char* text6 = "6. help";
    const char* text7 = "Enter your choice: ";

    int numLines = 7; // Total number of lines of text
    int text_length_1 = strlen(text1);
    int text_length_2 = strlen(text2);
    int text_length_3 = strlen(text3);
    int text_length_4 = strlen(text4);
    int text_length_5 = strlen(text5);
    int text_length_6 = strlen(text6);
    int text_length_7 = strlen(text7);

    int centerX = (window_width - text_length_1) / 2; // Calculate the center position
    int centerY = (window_height - numLines) / 2; // Calculate the center position

    mvwprintw(menu_window, centerY, centerX, "%s", text1);
    mvwprintw(menu_window, centerY + 1, centerX, "%s", text2);
    mvwprintw(menu_window, centerY + 2, centerX, "%s", text3);
    mvwprintw(menu_window, centerY + 3, centerX, "%s", text4);
    mvwprintw(menu_window, centerY + 4, centerX, "%s", text5);
    mvwprintw(menu_window, centerY + 5, centerX, "%s", text6);
    mvwprintw(menu_window, centerY + 6, centerX, "%s", text7);
    wrefresh(menu_window);

    // resetting the cursor to be in the menu window
    wmove(menu_window, centerY + 6, centerX + text_length_7);
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

void help_menu(WINDOW *menu_window) {
    wclear(menu_window);
    box(menu_window, 0, 0);

    int total_lines = 22; // Total number of lines in the help menu
    int max_lines = LINES - 6; // Maximum lines that can be displayed in the window (excluding borders)

    int start_line = 0;
    int end_line = max_lines - 1;

    const char *help_text[] = {
        "Add Workout",
        "This option allows you to insert a new workout entry. Currently it requires you to input", "the day, time, duration, and the training done for each workout entered.",
        "Please input the fields in the following formats:",
        "Date - YYYY/MM/DD - e.g. 25/07/2023",
        "Time - HH:MM - e.g. 13:15",
        "Duration - integer for minutes - e.g. 120",
        "Training - string for training - e.g. pull",
        "Disclaimer - This currently has no exit functionality",
        " ",
        "Remove Workout",
        "This option allows you to remove a workout by entering the date it was done.",
        "Please enter the date in the following format:",
        "Date - YYYY/MM/DD - e.g. 25/07/2023",
        "Disclaimer - Currently there is no exit functionality. You can just enter a wrong", "input, and the program will return to the option menu.",
        " ",
        "Display Workouts",
        "This option allows you to see all the workouts you have registered in the program by", "reading the data from the workouts.db saved in the database folder."
    };

    int num_lines = sizeof(help_text) / sizeof(help_text[0]);
    int ch;
    do {
        wclear(menu_window);
        box(menu_window, 0, 0);

        for (int i = 1; i < getmaxx(menu_window) - 1; ++i) {
            mvwaddch(menu_window, 2, i, ACS_HLINE);
        }

        // Bold the first line (title) and print it one line above
        wattron(menu_window, A_BOLD);
        mvwprintw(menu_window, 1, 2, "This is a manual for how to use this program.");
        wattroff(menu_window, A_BOLD);

        for (int i = start_line; i < start_line + max_lines && i < num_lines; ++i) {
            mvwprintw(menu_window, i - start_line + 3, 2, "%s", help_text[i]);
        }

        wrefresh(menu_window);
        ch = getch();

        // Handle scrolling
        switch (ch) {
            case KEY_UP:
                if (start_line > 0) {
                    start_line--;
                }
                break;
            case KEY_DOWN:
                if (end_line < num_lines - 1) {
                    start_line++;
                }
                break;
            default:
                break;
        }

        end_line = start_line + max_lines - 1;
        if (end_line >= num_lines) {
            end_line = num_lines - 1;
        }
    } while (ch != 27); // Exit loop on pressing the escape key
}

bool database_exists(const char *path) {
    FILE *file = fopen(path, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

int initialize_database(sqlite3 **db) {
    const char *database_path = "/home/elpatatone/Documents/alleno-ora/database/workouts.db";
    bool db_exists = database_exists(database_path);
    int rc = sqlite3_open(database_path, db);

    if (rc != SQLITE_OK) {
        printf("Cannot open database: %s\n", sqlite3_errmsg(*db));
        return rc;
    }

    // Create the table for workouts if the database doesn't exist
    if (!db_exists) {
        char *create_table_query = "CREATE TABLE workouts ("
                                   "id INTEGER PRIMARY KEY,"
                                   "date TEXT,"
                                   "time TEXT,"
                                   "duration INTEGER,"
                                   "training TEXT"
                                   ");";
        rc = sqlite3_exec(*db, create_table_query, NULL, 0, NULL);
        if (rc != SQLITE_OK) {
            printf("Failed to create table: %s\n", sqlite3_errmsg(*db));
            sqlite3_close(*db);
            return rc;
        }

        // Create the table for lifts
        char *create_lifts_table_query = "CREATE TABLE lifts ("
                                         "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                         "workout_id INTEGER,"
                                         "name TEXT,"
                                         "weight INTEGER,"
                                         "sets INTEGER,"
                                         "reps INTEGER,"
                                         "is_pr INTEGER,"
                                         "FOREIGN KEY(workout_id) REFERENCES workouts(id)"
                                         ");";
        rc = sqlite3_exec(*db, create_lifts_table_query, NULL, 0, NULL);
        if (rc != SQLITE_OK) {
            printf("Failed to create lifts table: %s\n", sqlite3_errmsg(*db));
            sqlite3_close(*db);
            return rc;
        }
    }

    return SQLITE_OK;
}

int main(void) {

    sqlite3 *db;
    int rc = initialize_database(&db);

    if (rc != SQLITE_OK) {
        return rc;
    }

    Workout workout[MAX_WORKOUTS];
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
                add_workout(db, workout, &workout_number, menu_window);
                save_workouts_to_file(workout, workout_number, directory);
                break;
            case 2:
                remove_workouts(db, menu_window);
                break;
            case 3:
                display_workouts(db, menu_window);
                break;
            case 4:
                add_pr(db, menu_window);
                break;
            case 5:
                endwin();
                return 0;
            case 6:
                help_menu(menu_window);
                break;
            default:
                invalid_input("Invalid choice, try again.", menu_window);
                break;
        }
    }
    return 0;
}
