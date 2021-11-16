#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"


// check for correct user input
int checkUserInput(char* parameter, int type) {
    // type 0 = frequency;
    // type 2 = amplitude
    if(atof(parameter)<=0.0 || (type == 0 && atof(parameter)>= 50.0) || (type == 1 && atof(parameter)>= 20.0)){
        return 0;
    }
    else{
        return 1;
    }
}

char* getconfig(char* file,char* config){ //function to the value of a specific parameter e.g. frequency from the parsed config file
    char *line;
    line = strstr(file,config); //go to the line containing the required config
    line = strstr(line,"="); // store only the string after =
    line = strtok(line,"\n"); //split the string when a break line is found
    return(&line[1]); //return only the value
}

void read_config(char *fileName, input *paramptr){
/* Function to read settings value from config file and store it into a struct
Input : Config file name and path(if necessary), Structure to hold the parameters
Return : Parsed config value will be stored in the struct*/

    FILE *config = fopen(fileName,"r+"); //open file and assign it to a handle
    char *buffer = NULL;
    char buffer2[1000],buffer3[1000];
    int buffer_size, read_size;

    if(!config) { //error handling if cannot open config file
        perror("Cannot open config file!");
        exit(1);
    }

    fseek(config, 0, SEEK_END); //go to end of file
    buffer_size = ftell(config); //determine file length
    rewind(config); //go to start of the file

    //dynamically allocate enough memory for a string to hold all file content
    if((buffer = (char*) calloc((buffer_size + 1),sizeof(char))) == NULL) {
        printf("Not enough memory!"); //error handling if not enough memory
        exit(1);
    }

    //reads everything and assign to a buffer
    read_size = fread(buffer,sizeof(char),buffer_size,config);

    //check if everything is read properly
    if (buffer_size != read_size){
        printf("File not read properly!!!"); //error handling and free up buffer
        free(buffer);
        buffer = NULL;
    }
    fclose(config); //close the config
    strcpy(buffer2,buffer); //copy buffer to avoid segmentation error while executing the functions
    strcpy(buffer3,buffer);
    
    paramptr->waveType = atoi(getconfig(buffer,"waveType"));//store the waveType in a struct
    paramptr->freq = atof(getconfig(buffer2,"frequency"));//convert frequency and amplitude to float
    paramptr->amp = atof(getconfig(buffer3,"amplitude"));// then store in a struct

}

void save_config(char* fileName,input *paramsptr){
    /* Function to save inputted parameters into a config file
   Input : Config file name and path(if necessary), Structure to hold the parameters*/ 
    FILE *config = fopen(fileName,"w+"); //open file and assign it to a handle
    fseek(config,0,SEEK_SET);//set the cursor to the start of the file
    /*write the updated parameters to the config file*/
    fprintf(config,"waveType=%d\n",paramsptr->waveType);
    fprintf(config,"frequency=%.2f\n",paramsptr->freq);
    fprintf(config,"amplitude=%.2f\n",paramsptr->amp);
    fclose(config);
}
