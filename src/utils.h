#ifndef UTILS_H
#define UTILS_H

#define MAX_WORKOUTS 365

void center(char *title);
void welcomeScreen();

typedef struct Workout {

    char date[10];
    char training[10];
    char time[10];

} Workout;

#endif // !UTILS_H
