#include <ncurses.h>
#include <unistd.h>

int main() {
    // Setup ncurses
    initscr();            // Start ncurses mode
    start_color();        // Enable colours
    cbreak();             // Disable line buffering
    noecho();            // Don't echo keypresses
    keypad(stdscr, TRUE); // Enable keypad
    curs_set(0);         // Hide cursor
    timeout(0);          // Non-blocking getch()

    // Initialise colour pairs
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);

    while (true) {
        clear();

        // Set colour, draw a border
        attron(COLOR_PAIR(1));
        box(stdscr, 0, 0);
        
        // Display title
        mvprintw(1, 2, "8Bit System Monitor");
        attroff(COLOR_PAIR(1));

        refresh();

        if (getch() == 'q') break;
        
        // Prevent high CPU usage
        usleep(100000); // 100ms
    }

    endwin();
    return 0;
}