#include<ncurses.h>

int main()
{
int amplitude = 50, frequency = 20;
int ch;

/* Curses Initialisations */
initscr();
raw();
keypad(stdscr, TRUE);
noecho();

printw("Press E to Exit\n");

while((ch = getch()) != 'E')
{
    switch(ch)
    {
    case KEY_UP:         
        amplitude++;
        break;
    case KEY_DOWN:      
        amplitude--;
        break;
    case KEY_LEFT:      
        frequency--;
        break;
    case KEY_RIGHT:     
        frequency++;
        break;
    }
    printw("\nAmplitude %d Frequency %d ",amplitude,frequency);
}

printw("\n\Exiting Now\n");
endwin();

return 0;
}