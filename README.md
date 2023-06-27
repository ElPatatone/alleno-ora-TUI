# alleno-ora

> > This is in active developement so expect the program to change a lot very quickly.

Alleno ora means I train now in italian.  
This is a TUI application written in C with the Ncurses library to help manage and see your workout progress.

## Installation

To install and use this program clone the repo in your desired location. In the `src` folder make a `config.txt`, in it put the path where you would like the `workout.txt` file to be stored, the file will hold all the workout information.  
The path should be in a single line like this: `/home/user/Documents/workouts.txt`  
Make sure to not use the `~` to specify the root directory as this will not be parsed correctly in the program, you can find the full path of where you would like to store the file using the command `realpath workouts.txt`

I made a `build.sh` file that will compile the program, you can then run the executable `./main` to run the program. However, I reccomend creating an alias in your
shell config, this would let you run the program from anywhere without needing to be in the project directory.  
Just specify the path where you have cloned the repo and replace the path example path below while keeping the rest of the code the same.
In `zsh` it is done like this:

```shell
alias alleno="cd ~/path/alleno-ora/src && ./main && cd -"
```

I have plans in the future to package this program into an app available to download.

## Usage

The program doesn't have a help section yet but you can use this for now.  
Right now the program has this options:

1. Add workout  
   This lets you enter a new workout with the following fields:
   - Date - **keep format to DD/MM/YYYY**
   - Time - **keep format to HH:MM**
   - Duration - **keep format to integers**
   - Training - **can be both strings and numbers**
2. Display workouts  
   This will display the workouts after reading them from the file (data persistency through multiple sessions)  
   They will be ordered with the most recent dates at the top
3. Exit
4. Help - _Not implemented yet_

The program is still in developement and I have a lot of ideas I want to add, expect the options to grow with time.

## Demo

## TODO

- [ ] Improve the UI as a lot of space is wasted.
- [ ] Be able to store information on lifts done for a given workout.
- [ ] Be able to see the stats, PRs for specific lifts.
- [ ] calculate the improvement for a specific lifts PR.
- [ ] calculate average/total amount time spent in the gym for any given time frame.
- [ ] check for any lifts that have been stagnant.
- [ ] Sort the list by most or least time spent, and by the earliest or latest workout done time wise.
