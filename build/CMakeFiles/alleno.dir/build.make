# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/elpatatone/Documents/old-alleno-ora/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/elpatatone/Documents/old-alleno-ora/build

# Include any dependencies generated for this target.
include CMakeFiles/alleno.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/alleno.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/alleno.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/alleno.dir/flags.make

CMakeFiles/alleno.dir/main.c.o: CMakeFiles/alleno.dir/flags.make
CMakeFiles/alleno.dir/main.c.o: /home/elpatatone/Documents/old-alleno-ora/src/main.c
CMakeFiles/alleno.dir/main.c.o: CMakeFiles/alleno.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/elpatatone/Documents/old-alleno-ora/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/alleno.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/alleno.dir/main.c.o -MF CMakeFiles/alleno.dir/main.c.o.d -o CMakeFiles/alleno.dir/main.c.o -c /home/elpatatone/Documents/old-alleno-ora/src/main.c

CMakeFiles/alleno.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/alleno.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/elpatatone/Documents/old-alleno-ora/src/main.c > CMakeFiles/alleno.dir/main.c.i

CMakeFiles/alleno.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/alleno.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/elpatatone/Documents/old-alleno-ora/src/main.c -o CMakeFiles/alleno.dir/main.c.s

# Object files for target alleno
alleno_OBJECTS = \
"CMakeFiles/alleno.dir/main.c.o"

# External object files for target alleno
alleno_EXTERNAL_OBJECTS =

alleno: CMakeFiles/alleno.dir/main.c.o
alleno: CMakeFiles/alleno.dir/build.make
alleno: /usr/lib/x86_64-linux-gnu/libcurses.so
alleno: /usr/lib/x86_64-linux-gnu/libform.so
alleno: /usr/lib/x86_64-linux-gnu/libsqlite3.so
alleno: CMakeFiles/alleno.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/elpatatone/Documents/old-alleno-ora/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable alleno"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/alleno.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/alleno.dir/build: alleno
.PHONY : CMakeFiles/alleno.dir/build

CMakeFiles/alleno.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/alleno.dir/cmake_clean.cmake
.PHONY : CMakeFiles/alleno.dir/clean

CMakeFiles/alleno.dir/depend:
	cd /home/elpatatone/Documents/old-alleno-ora/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/elpatatone/Documents/old-alleno-ora/src /home/elpatatone/Documents/old-alleno-ora/src /home/elpatatone/Documents/old-alleno-ora/build /home/elpatatone/Documents/old-alleno-ora/build /home/elpatatone/Documents/old-alleno-ora/build/CMakeFiles/alleno.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/alleno.dir/depend

