#include <stdio.h>
#include <stdlib.h>

int main()
{
int amplitude = 50, frequency = 20;
int ch;

while(1){
    //when an arrow is pressed, getch returns 2 values 0 and unique arrow key value
    getch(); // clear the 0 first
    switch(getch()) { // get the real value
        case 72: //ARROW UP is PRESSED
           amplitude++;
            break;
        case 80: //ARROW DOWN is PRESSED
            amplitude--;
            break;
        case 'C': //ARROW RIGHT is PRESSED
            frequency++;
            break;
        case 'D': //ARROW LEFT is PRESSED
            frequency--;
            break;
    }
}
    printf("amplitude = %d \t frequency = %d",amplitude,frequency); 

   

return 0;
}