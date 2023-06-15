# alleno-ora

Alleno means I train now in italian.  
This is a TUI application written in C with the Ncurses library to help manage and see your workout progress.
Planned features:

- Make new entries for workouts with the date and what was done on the day, this should be stored in a new text file.
- Be able to see the stats, PRs for specific lifts.
- Run some fun calculations to see the statistics for the lifts:
  - improvement for a specific lifts PR.
  - average/total amount time spent in the gym for any given time frame.
  - check for any lifts that have been stagnant.
  - others...

Maybe I can make the app take in other health metrics
such as sleep time, heart rate and many others.  
All the data will be stored in a text file (current idea) and over time it will accumulate a large amount of data.
The data will be stored locally on the text file so maybe I could use the data for some future NN model application trained on my data just for fun.
