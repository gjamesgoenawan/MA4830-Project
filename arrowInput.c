#include<ncurses.h>

void readkbd(int amplitude,int frequency){
    int ch;
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
}

int main()
{
int amplitude = 50, frequency = 20;

/* Curses Initialisations */
initscr();
raw();
keypad(stdscr, TRUE);
noecho();

readkbd(amplitude,frequency);
return 0;
}