# alleno-ora

> > This is in active developement so expect the program to change a lot very quickly.

Alleno ora means I train now in italian.  
This is a TUI application written in C with the Ncurses library to help manage and see your workout progress.

## Installation

To install and use this program clone the repo in your desired location. In the `src` folder make a `config.txt`, in it put the path where you would like the `workout.txt` file to be stored, the file will hold all the workout information.  
The path should be in a single line like this: `/home/user/Documents/workouts.txt`  
Make sure to not use the `~` to specify the root directory as this will not be parsed correctly in the program, you can find the full path of where you would like to store the file using the command `realpath workouts.txt`

The project uses Cmake to manage the build, you can run the `build.sh` file in the build directory and it will run cmake and make to build the executatble, you can then run the executable `./alleno` to run the program. However, I reccomend creating an alias in your
shell config, this would let you run the program from anywhere without needing to be in the project directory.  
Just specify the path where you have cloned the repo and replace the path example path below while keeping the rest of the code the same.
In `zsh` it is done like this:

```shell
alias alleno="cd ~/path/alleno-ora/build && ./alleno && cd -"
```

I have plans in the future to make a binary you can add to your path to run it globally more easily.

## Usage

Right now the program has this options:

1. Add workout  
   This lets you enter a new workout with the following fields:
   - Date - **keep format to YYYY/MM/DD**
   - Time - **keep format to HH:MM**
   - Duration - **keep format to integers**
   - Training - **can be both strings and numbers**
2. Remove Workouts  
   This allows the user to remove a workout entry by entering the date for that workout.
3. Display workouts  
   This will fetch the workout data from the local database and display it ordered by date in a descending order.
4. Add PR
   This allows you to enter information for a PR, you can enter the following fields:
   - Date - **keep format to YYYY/MM/DD**
   - Name - **keep format to strings**
   - Weight - **keep format to integers**
5. Display PR
   This will display the PR information ordered by date in a descending order.
6. Exit
7. Help - A more in-depth help section

## TODO

- [ ] add ablity to remove workouts directly from the file.
- [ ] Improve the UI as a lot of space is wasted.
- [ ] Be able to store information on lifts done for a given workout.
- [ ] Be able to see the stats, PRs for specific lifts.
- [ ] calculate the improvement for a specific lifts PR.
- [ ] calculate average/total amount time spent in the gym for any given time frame.
- [ ] check for any lifts that have been stagnant.
- [ ] Sort the list by most or least time spent, and by the earliest or latest workout done time wise.
