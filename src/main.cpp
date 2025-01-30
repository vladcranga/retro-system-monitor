#include <cstdlib>
#include <cstring>
#include <ncurses.h>
#include <unistd.h>
#include <iomanip>
#include <sstream>
#include "../include/system_monitor.hpp"

std::string format_memory(unsigned long kb) {
    double gb = kb / (1024.0 * 1024.0);
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << gb << " GB";
    return ss.str();
}

// Create a retro-style border with ASCII
void draw_retro_border() {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Draw corners
    mvprintw(0, 0, "+");
    mvprintw(0, max_x - 1, "+");
    mvprintw(max_y - 1, 0, "+");
    mvprintw(max_y - 1, max_x - 1, "+");

    // Draw horizontal lines
    for (int i = 1; i < max_x - 1; i++) {
        mvprintw(0, i, "=");
        mvprintw(max_y - 1, i, "=");
        mvprintw(2, i, "=");
    }

    // Draw vertical lines
    for (int i = 1; i < max_y - 1; i++) {
        mvprintw(i, 0, "|");
        mvprintw(i, max_x - 1, "|");
    }
}

// Create status bars
void draw_status_bar(int y, int x, float percentage, int width = 20, bool is_cpu = true) {
    std::string prefix = "[";
    std::string bar = "";
    int filled = (int)((percentage * width) / 100);
    
    // Add filled portion
    for (int i = 0; i < filled; i++) {
        bar += "#";
    }
    // Add empty portion
    for (int i = filled; i < width; i++) {
        bar += "-";
    }
    
    // Colour based on percentage
    // Red for danger, yellow for warning, green for safe
    if (percentage > 80) attron(COLOR_PAIR(4));
    else if (percentage > 60) attron(COLOR_PAIR(3));
    else {
        if (is_cpu) attron(COLOR_PAIR(1)); else attron(COLOR_PAIR(6));
    }

    mvprintw(y, x, "%s%s] %3.1f%%", prefix.c_str(), bar.c_str(), percentage);
    
    // Reset the colour
    if (is_cpu) attron(COLOR_PAIR(1)); else attron(COLOR_PAIR(6));
}

// Print centered text based on screen width and text length
void print_centered_text(int y, const char* format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    int screen_width = getmaxx(stdscr);
    int starting_column = (screen_width - strlen(buffer)) / 2;

    mvprintw(y, starting_column, "%s", buffer);
}

// Get RPG-style status message based on usage
std::string get_status_message(float usage) {
    if (usage > 90) return "CRITICAL!";
    if (usage > 80) return "DANGER!";
    if (usage > 60) return "WARNING!";
    if (usage > 40) return "Normal";
    if (usage > 20) return "Good";
    return "Idle";
}

int main() {
    // Setup ncurses
    initscr();            // Start ncurses mode
    system("printf '\\e[8;30;80t'"); // Set terminal size
    resize_term(30, 80);
    start_color();        // Enable colours
    cbreak();             // Disable line buffering
    noecho();            // Don't echo keypresses
    keypad(stdscr, TRUE); // Enable keypad
    curs_set(0);         // Hide cursor
    timeout(0);          // Non-blocking getch()

    // Initialise colour pairs
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_BLUE, COLOR_BLACK);

    SystemMonitor monitor;

    while (true) {
        clear();
        monitor.update();

        auto cpu_stats = monitor.getCPUStats();
        auto mem_stats = monitor.getMemoryStats();
        float cpu_temp = monitor.getCPUTemp();
        auto processes = monitor.getTopProcesses();
        auto distro_info = monitor.getDistroInfo();

        // Draw the main border
        attron(COLOR_PAIR(2));
        draw_retro_border();

        // Turn on bold text
        attron(A_BOLD);

        // Title
        print_centered_text(1, "/-[ RETRO SYSTEM MONITOR ]-\\");
        attroff(COLOR_PAIR(2));

        // Distro Info
        print_centered_text(3, "Running on %s", distro_info.name.c_str());

        // CPU Usage Percent
        attron(COLOR_PAIR(1));
        print_centered_text(5, "<<< CPU STATUS >>>");
        draw_status_bar(
            6, getmaxx(stdscr) / 2 - 10, cpu_stats.usage_percent, 20, true);

        // Temperature with color coding
        if (cpu_temp > 80) attron(COLOR_PAIR(4));
        else if (cpu_temp > 60) attron(COLOR_PAIR(3));
        print_centered_text(7, "TEMP: %.1f°C", cpu_temp);

        // CPU Status
        std::string cpu_status = get_status_message(cpu_stats.usage_percent);
        print_centered_text(8, "Status: %s", cpu_status.c_str());

        attroff(COLOR_PAIR(1));

        // RAM Status
        attron(COLOR_PAIR(6));
        print_centered_text(10, "<<< RAM STATUS >>>");
        draw_status_bar(11, getmaxx(stdscr) / 2 - 10, mem_stats.usage_percent, 20, false);

        // RAM Usage
        print_centered_text(12, "Total RAM: %s", format_memory(mem_stats.total).c_str());
        print_centered_text(13, "Used  RAM: %s", format_memory(mem_stats.used).c_str());
        print_centered_text(14, "Free  RAM: %s", format_memory(mem_stats.free).c_str());

        std::string mem_status = get_status_message(mem_stats.usage_percent);
        print_centered_text(15, "Status: %s", mem_status.c_str());

        attroff(COLOR_PAIR(6));

        // Battery Statistics
        auto battery_stats = monitor.getBatteryStats();
        if (battery_stats.present) {
            attron(COLOR_PAIR(3));
            print_centered_text(17, "<<< BATTERY STATUS >>>");
            
            // Battery bar with an appropriate colour
            std::string energy_prefix = battery_stats.charging ? "[+" : "[";
            std::string bar = "";
            int width = 20;
            int filled = (int)((battery_stats.percentage * width) / 100);
            
            // Colour based on the battery level
            // Red for low, yellow for medium, green for good
            if (battery_stats.percentage <= 20) attron(COLOR_PAIR(4));
            else attron(COLOR_PAIR(3));
            
            // Create the bar
            for (int i = 0; i < filled; i++) bar += "*";
            for (int i = filled; i < width; i++) bar += "-";
            
            print_centered_text(18, "%s%s] %3.1f%%", energy_prefix.c_str(), bar.c_str(), battery_stats.percentage);
            
            // Colour based on the battery status
            if (battery_stats.charging) {
                attron(COLOR_PAIR(1));
                print_centered_text(19, "* Charging *");
            } else if (battery_stats.percentage <= 20) {
                attron(COLOR_PAIR(4));
                beep();
                print_centered_text(19, "! Low Battery !");
            }
            attron(COLOR_PAIR(1));
        }

        // Process Section
        attron(COLOR_PAIR(5));
        print_centered_text(21, "<<< TOP PROCESSES >>>");
        for (size_t i = 0; i < processes.size(); ++i) {
            print_centered_text(22 + i, "* %s", processes[i].c_str());
        }
        attroff(COLOR_PAIR(5));

        // Footer
        attron(COLOR_PAIR(2));
        print_centered_text(getmaxy(stdscr) - 2, "[Q]uit");
        attroff(COLOR_PAIR(2));

        // Turn off bold text
        attroff(A_BOLD);

        refresh();

        // Check if the user wants to quit
        int userQuitKey = getch();
        // Check for the ASCII codes of 'q' (113) and 'Q' (81)
        if (userQuitKey == 'q' || userQuitKey == 'Q') break;

        // 1 second delay
        usleep(1000000);
    }

    endwin();
    return 0;
}