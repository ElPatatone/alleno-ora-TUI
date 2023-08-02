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

    char date[15];
    char name[100];
    int weight;
    int sets;
    int reps;
    int is_pr;

} Lift;

typedef struct Workout {

    char date[15];
    char training[100];
    char time[15];
    int duration;
    int number_of_lifts;
    Lift lift[MAX_LIFTS];
    Lift pr;
    int rating;
    char location[10];

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

    Lift pr_lift;

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
        mvwgetnstr(menu_window, 3, 41, pr_lift.date, sizeof(pr_lift.date));

        if (strlen(pr_lift.date) == 0) {
            return;
        }

        int year, month, day;
        if (sscanf(pr_lift.date, "%d/%d/%d", &year, &month, &day) != 3 ||
            day < 1 || day > 31 || month < 1 || month > 12 || year < 2000 || year > 9999) {
            invalid_input("Invalid date format or invalid date. Please enter a valid date.", menu_window);
        }else {
            break;
        }
    }

    char select_workout_query[100];
    sprintf(select_workout_query, "SELECT date FROM workouts WHERE date = '%s';", pr_lift.date);
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, select_workout_query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to execute select query: %s\n", sqlite3_errmsg(db));
        return;
    }

    int workout_exists = (sqlite3_step(stmt) == SQLITE_ROW);
    if(workout_exists){
        sqlite3_finalize(stmt);
        char pr_weight_str[100];

        wclear(menu_window);
        box(menu_window, 0, 0);

        wattron(menu_window, A_BOLD);
        mvwprintw(menu_window, 1, 2, "Add PR for %s", pr_lift.date);
        wattroff(menu_window, A_BOLD);
        mvwhline(menu_window, 2, 1, ACS_HLINE, getmaxx(menu_window) - 2);

        mvwprintw(menu_window, 3, 2, "Enter the name of the PR: ");
        wmove(menu_window, 3, 28);
        wrefresh(menu_window);
        wgetstr(menu_window, pr_lift.name);
        if (strlen(pr_lift.name) == 0) {
            return;
        }

        mvwprintw(menu_window, 4, 2, "Enter the weight of the PR (kg): ");
        wmove(menu_window, 4, 35);
        wrefresh(menu_window);
        wgetstr(menu_window, pr_weight_str);

        if (strlen(pr_weight_str) == 0) {
            return;
        }

        pr_lift.weight = atoi(pr_weight_str);
        wrefresh(menu_window);

        char insert_pr_query[200];
        sprintf(insert_pr_query, "INSERT INTO lifts (date, name, weight, is_pr) VALUES ('%s', '%s', %d, 1);", pr_lift.date, pr_lift.name, pr_lift.weight);
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
        mvwprintw(menu_window, 3,2, "No Workout found for the entered date. You can still add a PR.");
        mvwprintw(menu_window, 4,2, "Press any key to continue...");
        wattroff(menu_window, A_BOLD);
        wrefresh(menu_window);
        getch();

        char pr_weight_str[100];

        wclear(menu_window);
        box(menu_window, 0, 0);

        wattron(menu_window, A_BOLD);
        mvwprintw(menu_window, 1, 2, "Add PR for %s", pr_lift.date);
        wattroff(menu_window, A_BOLD);
        mvwhline(menu_window, 2, 1, ACS_HLINE, getmaxx(menu_window) - 2);

        mvwprintw(menu_window, 3, 2, "Enter the name of the PR: ");
        wmove(menu_window, 3, 28);
        wrefresh(menu_window);
        wgetstr(menu_window, pr_lift.name);
        if (strlen(pr_lift.name) == 0) {
            return;
        }

        mvwprintw(menu_window, 4, 2, "Enter the weight of the PR (kg): ");
        wmove(menu_window, 4, 35);
        wrefresh(menu_window);
        wgetstr(menu_window, pr_weight_str);

        if (strlen(pr_weight_str) == 0) {
            return;
        }

        pr_lift.weight = atoi(pr_weight_str);
        wrefresh(menu_window);

        char insert_pr_query[200];
        sprintf(insert_pr_query, "INSERT INTO lifts (date, name, weight, is_pr) VALUES ('%s', '%s', %d, 1);", pr_lift.date, pr_lift.name, pr_lift.weight);
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
    sqlite3_finalize(stmt);
    getch();

}

void display_pr(sqlite3 *db, WINDOW *menu_window) {
    wclear(menu_window);
    box(menu_window, 0, 0);

    char *select_query = "SELECT * FROM lifts ORDER BY date DESC;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, select_query, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("Failed to execute select query: %s\n", sqlite3_errmsg(db));
        return;
    }

    int max_rows, max_columns;
    getmaxyx(menu_window, max_rows, max_columns);

    int top_index = 0;  // Index of the topmost visible workout
    int visible_rows = max_rows - 7;  // Adjust the number of visible rows as needed

    char line[256];
    int workout_number = 0;

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
        mvwprintw(menu_window, 1, 2, "Displaying all PRs");
        wattroff(menu_window, A_BOLD);
        mvwhline(menu_window, 2, 1, ACS_HLINE, max_columns - 2);  // Top border

        wattron(menu_window, A_BOLD);
        mvwprintw(menu_window, 3, 7, "Date");
        mvwprintw(menu_window, 3, 24, "Name");
        mvwprintw(menu_window, 3, 37, "Weight");
        wattroff(menu_window, A_BOLD);
        mvwhline(menu_window, 4, 1, ACS_HLINE, max_columns - 2);
        wrefresh(menu_window);

        mvwaddch(menu_window, 3, 17, ACS_VLINE);
        mvwaddch(menu_window, 3, 35, ACS_VLINE);
        mvwaddch(menu_window, 3, 45, ACS_VLINE);
        wrefresh(menu_window);

        workout_number = 0;
        sqlite3_reset(stmt);  // Reset statement to re-execute the query

        int row = 5;

        while (sqlite3_step(stmt) == SQLITE_ROW && workout_number < top_index + visible_rows) {
            workout_number++;
            if (workout_number <= top_index)
                continue;

            int id = sqlite3_column_int(stmt, 0);
            const unsigned char *date = sqlite3_column_text(stmt, 1);
            const unsigned char *name = sqlite3_column_text(stmt, 2);
            int weight = sqlite3_column_int(stmt, 3);

            mvwprintw(menu_window, row, 4, "%s", date);
            mvwprintw(menu_window, row, 20, "%s", name);
            mvwprintw(menu_window, row, 39, "%d", weight);

            row++;

            if (workout_number >= top_index + visible_rows)
                break;
        }

        if (workout_number > visible_rows) {
            mvwprintw(menu_window, visible_rows + 5, 2, "Use UP/DOWN arrow keys to scroll");
        } else {
            mvwprintw(menu_window, visible_rows + 5, 2, "Press Escape key to continue...");
        }

        // // Draw vertical borders
        for (int i = 0; i < visible_rows; i++) {
            mvwaddch(menu_window, 5 + i, 17, ACS_VLINE);
            mvwaddch(menu_window, 5 + i, 35, ACS_VLINE);
            mvwaddch(menu_window, 5 + i, 45, ACS_VLINE);
        }

        wrefresh(menu_window);
    } while ((ch = getch()) != 27);  // Exit on Escape key press (27 is the Escape key code)

    sqlite3_finalize(stmt);
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

void add_workout_to_db(sqlite3 *db, Workout *workout) {

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

void add_workout(sqlite3 *db, WINDOW *menu_window) {
    wclear(menu_window);
    box(menu_window, 0, 0);

    Workout workout;

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
        wgetnstr(menu_window, workout.date, sizeof(workout.date));

        if (strlen(workout.date) == 0) {
            return;
        }

        int year, month, day;
        if (sscanf(workout.date, "%d/%d/%d", &year, &month, &day) != 3) {
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

            mvwprintw(menu_window, 3, 2, "Date (YYYY/MM/DD): %s", workout.date);
            mvwprintw(menu_window, 4, 2, "Time (HH:MM): ");
        } else {
            mvwprintw(menu_window, 4, 2, "Time (HH:MM): ");
        }

        wrefresh(menu_window);
        wgetnstr(menu_window, workout.time, sizeof(workout.time));

        if (strlen(workout.time) == 0) {
            return;
        }

        // Check if time is in the format "HH:MM"
        int hour, minute;
        if (sscanf(workout.time, "%d:%d", &hour, &minute) != 2) {
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

            mvwprintw(menu_window, 3, 2, "Date (YYYY/MM/DD): %s", workout.date);
            mvwprintw(menu_window, 4, 2, "Time (HH:MM): %s", workout.time);
            mvwprintw(menu_window, 5, 2, "Duration (minutes): ");
        } else {
            mvwprintw(menu_window, 5, 2, "Duration (minutes): ");
        }

        wrefresh(menu_window);
        char duration_string[10];
        wmove(menu_window, 5, 22); // Set the cursor position
        wrefresh(menu_window);
        wgetstr(menu_window, duration_string);
        sscanf(duration_string, "%d", &workout.duration);

        if (workout.duration <= 0) {
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

            mvwprintw(menu_window, 3, 2, "Date (YYYY/MM/DD): %s", workout.date);
            mvwprintw(menu_window, 4, 2, "Time (HH:MM): %s", workout.time);
            mvwprintw(menu_window, 5, 2, "Duration (minutes): %d", workout.duration);
            mvwprintw(menu_window, 6, 2, "Training done: ");
        } else {
            mvwprintw(menu_window, 6, 2, "Training done: ");
        }

        wrefresh(menu_window);
        wgetnstr(menu_window, workout.training, sizeof(workout.training));

        if (strlen(workout.training) == 0) {
            invalid_input("Training value cannot be empty. Please enter a valid training value.", menu_window);
            iteration3++;
        } else {
            break;
        }
    }

    // Remove the training value appended to the date value (No idea why this happens)
    strtok(workout.date, " \t\n");
    wrefresh(menu_window);

    add_workout_to_db(db, &workout);
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
    int visible_rows = max_rows - 7;  // Adjust the number of visible rows as needed

    char line[256];
    int workout_number = 0;


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

        wattron(menu_window, A_BOLD);
        mvwprintw(menu_window, 3, 7, "Date");
        mvwprintw(menu_window, 3, 19, "Time");
        mvwprintw(menu_window, 3, 30, "Duration");
        mvwprintw(menu_window, 3, 44, "Training");
        mvwprintw(menu_window, 3, 60, "Location");
        mvwprintw(menu_window, 3, 74, "Rating");
        wattroff(menu_window, A_BOLD);
        wrefresh(menu_window);
        mvwhline(menu_window, 4, 1, ACS_HLINE, max_columns - 2);

        mvwaddch(menu_window, 3, 17, ACS_VLINE);
        mvwaddch(menu_window, 3, 27, ACS_VLINE);
        mvwaddch(menu_window, 3, 40, ACS_VLINE);
        mvwaddch(menu_window, 3, 56, ACS_VLINE);
        mvwaddch(menu_window, 3, 71, ACS_VLINE);
        mvwaddch(menu_window, 3, 83, ACS_VLINE);

        workout_number = 0;
        sqlite3_reset(stmt);  // Reset statement to re-execute the query

        int row = 5;

        while (sqlite3_step(stmt) == SQLITE_ROW && workout_number < top_index + visible_rows) {
            workout_number++;
            if (workout_number <= top_index)
                continue;

            const unsigned char *date = sqlite3_column_text(stmt, 1);
            const unsigned char *time = sqlite3_column_text(stmt, 2);
            int duration = sqlite3_column_int(stmt, 3);
            const unsigned char *training = sqlite3_column_text(stmt, 4);

            mvwprintw(menu_window, row, 4, "%s", date);
            mvwprintw(menu_window, row, 20, "%s", time);
            mvwprintw(menu_window, row, 32, "%d", duration);
            mvwprintw(menu_window, row, 44, "%s", training);

            row++;

            if (workout_number >= top_index + visible_rows)
                break;
        }

        if (workout_number > visible_rows) {
            mvwprintw(menu_window, visible_rows + 5, 2, "Use UP/DOWN arrow keys to scroll");
        } else {
            mvwprintw(menu_window, visible_rows + 5, 2, "Press Escape key to continue...");
        }
        // Draw vertical borders
        for (int i = 0; i < visible_rows; i++) {
            mvwaddch(menu_window, 5 + i, 17, ACS_VLINE);
            mvwaddch(menu_window, 5 + i, 27, ACS_VLINE);
            mvwaddch(menu_window, 5 + i, 40, ACS_VLINE);
            mvwaddch(menu_window, 5 + i, 56, ACS_VLINE);
            mvwaddch(menu_window, 5 + i, 71, ACS_VLINE);
            mvwaddch(menu_window, 5 + i, 83, ACS_VLINE);
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
    const char* text5 = "5. Display PR";
    const char* text6 = "6. Exit";
    const char* text7 = "7. help";
    const char* text8 = "Enter your choice: ";

    int numLines = 8; // Total number of lines of text
    int text_length_1 = strlen(text1);
    int text_length_2 = strlen(text2);
    int text_length_3 = strlen(text3);
    int text_length_4 = strlen(text4);
    int text_length_5 = strlen(text5);
    int text_length_6 = strlen(text6);
    int text_length_7 = strlen(text7);
    int text_length_8 = strlen(text8);

    int centerX = (window_width - text_length_1) / 2; // Calculate the center position
    int centerY = (window_height - numLines) / 2; // Calculate the center position

    mvwprintw(menu_window, centerY, centerX, "%s", text1);
    mvwprintw(menu_window, centerY + 1, centerX, "%s", text2);
    mvwprintw(menu_window, centerY + 2, centerX, "%s", text3);
    mvwprintw(menu_window, centerY + 3, centerX, "%s", text4);
    mvwprintw(menu_window, centerY + 4, centerX, "%s", text5);
    mvwprintw(menu_window, centerY + 5, centerX, "%s", text6);
    mvwprintw(menu_window, centerY + 6, centerX, "%s", text7);
    mvwprintw(menu_window, centerY + 7, centerX, "%s", text8);
    wrefresh(menu_window);

    // resetting the cursor to be in the menu window
    wmove(menu_window, centerY + 7, centerX + text_length_8);
    wrefresh(menu_window);

    char choice_string[10];
    wgetstr(menu_window, choice_string);
    int choice = atoi(choice_string);

    wrefresh(menu_window);
    return choice;
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
        "Date - YYYY/MM/DD - e.g. 2023/07/25",
        "Time - HH:MM - e.g. 13:15",
        "Duration - integer for minutes - e.g. 120",
        "Training - string for training - e.g. pull",
        "Disclaimer - This currently has no exit functionality",
        " ",
        "Remove Workout",
        "This option allows you to remove a workout by entering the date it was done.",
        "Please enter the date in the following format:",
        "Date - YYYY/MM/DD - e.g. 2023/07/25",
        "Disclaimer - Currently there is no exit functionality. You can just enter a wrong", "input, and the program will return to the option menu.",
        " ",
        "Display Workouts",
        "This option allows you to see all the workouts you have registered in the program by", "reading the data from the workouts.db saved in the database folder.",
        " ",
        "Add PR",
        "This option allows the user to enter details for a PR.",
        "If there is no workout already saved for the date it will still allow you to just enter", "the PR.",
        "Please enter the date in the following format:",
        "Date - YYYY/MM/DD - e.g. 25/07/2023",
        "Name - string for the name of the lift - e.g. deadlift",
        "Weight - integer for the weight of the lift (kg) - e.g. 140",
        " ",
        "Disaply PR",
        "This option will allow the user to see all the PRs entries made ordered by date in a", "descending order.",

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

bool check_for_database(const char *path) {
    FILE *file = fopen(path, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

int initialize_database(sqlite3 **db) {
    const char *database_path = "/home/elpatatone/Documents/alleno-ora/database/workouts.db";
    bool db_exists = check_for_database(database_path);
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
                                    "training TEXT,"
                                    "rating INTEGER,"
                                    "location TEXT"
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
                                         "date TEXT,"
                                         "name TEXT,"
                                         "weight INTEGER,"
                                         "sets INTEGER,"
                                         "reps INTEGER,"
                                         "is_pr INTEGER,"
                                         "FOREIGN KEY(date) REFERENCES workouts(date)"
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

    initscr();
    keypad(stdscr, TRUE);

    welcome_screen();

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
                add_workout(db, menu_window);
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
                display_pr(db, menu_window);
                break;
            case 6:
                endwin();
                return 0;
            case 7:
                help_menu(menu_window);
                break;
            default:
                invalid_input("Invalid choice, try again.", menu_window);
                break;
        }
    }
    return 0;
}
