#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <ctype.h>


int sineWave(int i, int n, float a){ //Function to generate sine wave
    float delta=(2.0*3.142)/n;
    return((int) (((sinf((float)(i*delta))) + 1.0) * 0.5 * a * 0x8000));
}

int squareWave(int i, int n, float a){//Function to generate square wave
    if(i<=(n/2)){
        return((int)(1.0 * a * 0x8000));
    }
    else{
        return((int)(0.0 * a * 0x8000));
    }
}

int sawtoothWave(int i, int n, float a){//Function to generate sawtooth wave
    return((((float) i / (float) n) * a * 0.5 * ((float) 0x8000)));
}

int triangularWave(int i, int n, float a){//Function to generate triangular wave
    if(i<=(n/2)){
        return((int)(((((float)i*2.0*((float)0x8000))/((float)n)))*((float) a)));
    }
    else{
        return((int)((((float) 0x8000)-((((float) 0x8000)*((2.0*((float) i))-((float) n)))/((float) n)))*((float) a)));
    } 
}