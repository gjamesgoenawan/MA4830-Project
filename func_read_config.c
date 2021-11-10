#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char waveType[20];
    int amplitude;
    int frequency; 
} params;


char* getconfig(char* setting){ //function to the value of a specific parameter e.g. frequency from the parsed config file
    FILE *config = fopen("config.txt","r+"); //open file and assign it to a handle
    char *buffer = NULL;
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

    char* line;
    line = strstr(buffer,setting); //go to the line containing the required config
    line = strstr(line,"="); // store only the string after =
    line = strtok(line,"\n"); //split the string when a break line is found
    return(&line[1]); //return only the value

    free(buffer);
    fclose(config);
}

int main()
{
params param; //initialize a struct to hold metronome parameters
params *paramsptr;

paramsptr = &param;

strcpy(paramsptr->waveType,getconfig("waveType")); //store the waveType in a struct
paramsptr->frequency = atoi(getconfig("frequency")); //convert frequency and amplitude to int
paramsptr->amplitude = atoi(getconfig("amplitude")); // then store in a struct

printf("%s %d %d\n",paramsptr->waveType,paramsptr->frequency,paramsptr->amplitude);

return 0;
}