cmake_minimum_required(VERSION 3.12)
project(ALLENO)

set(CMAKE_C_STANDARD 11)

# Find ncurses library
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Find SQLite library
find_package(SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})

add_executable(alleno main.c)

target_link_libraries(alleno ${CURSES_LIBRARIES} ${SQLite3_LIBRARIES})


