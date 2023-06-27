# alleno-ora

Alleno means I train now in italian.  
This is a TUI application written in C with the Ncurses library to help manage and see your workout progress.
Planned features:

## Installation

To install and use this program clone the repo in your desired location. There is a `config.txt`, put there the path for where you would like the `workout.txt` file to be stored, the file will hold all the workout information.  
The path should be in a single line like this: `/home/user/Documents/workouts.txt`

- Make new entries for workouts with the date and what was done on the day, this should be stored in a new text file.
- Be able to see the stats, PRs for specific lifts.
- Run some fun calculations to see the statistics for the lifts:
  - improvement for a specific lifts PR.
  - average/total amount time spent in the gym for any given time frame.
  - check for any lifts that have been stagnant.
  - others...
