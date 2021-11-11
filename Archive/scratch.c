#include <stdio.h>
#include <curses.h>
#include <ncurses.h>

int main()
{
int amplitude=50, period=20;
    while(1){
        getchar(); // skip the [
        switch(getchar())  // the real value
    {
    case 72:
        printf("%s","Up Key Pressed. Amplitude Increasing.\n");
        amplitude = amplitude + 0.1;
        break;
    case 80:
        printf("%s","Down Key Pressed. Amplitude Decreasing.\n");
        amplitude = amplitude - 0.1;
        break;
    case 75:
        printf("%s","Left Key Pressed. Frequency Decreasing.\n");
        period = period + 10;
        break;
    case 77:
        printf("%s","Right Key Pressed. Frequency Increasing.\n");
        period = period - 10;
        break;
    default:
        printf("%s","Invalid key input. Only use arrow keys.\n");
    }
    printf("amplitude %d period %d\n",amplitude,period);
    }
return 0;
}