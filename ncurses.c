#include <stdio.h>
#include <ncurses.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "main.h"

void startNcurses() {
    WINDOW* screen = initscr();
    start_color();
    nodelay(screen, 1);
    cbreak();
    noecho();
    curs_set(0);
    keypad(screen, TRUE);

    // Color pairs
    init_pair(1, COLOR_RED, COLOR_BLACK); 
    init_pair(2, COLOR_GREEN, COLOR_BLACK); 
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
}

ushort sinWaveNcurses(double amplitude, double period,
    short phase_shift, ushort color_index) {
	double ratio, y;
    double x;
    for (x=0.0; x < (COLS+phase_shift) ; x += 1.0) {

        // Swap color
        color_index += 1;
        attron(COLOR_PAIR((color_index % NUM_COLORS)+1));

        // Find Y 
        ratio = (2.0*M_PI)/LINES;
        y = sin(period*x*ratio);
        y *= amplitude; 
        y += LINES/2;

        // Print cell
        mvprintw((int)(y), (int)(x-phase_shift), " ");
    }
    return color_index;
}

