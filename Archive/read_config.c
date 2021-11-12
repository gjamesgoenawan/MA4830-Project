#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char waveType[20];
    int amplitude;
    int frequency; 
} params;

char* getconfig(char* file,char* config){ //function to the value of a specific parameter e.g. frequency from the parsed config file
    char *line;
    line = strstr(file,config); //go to the line containing the required config
    line = strstr(line,"="); // store only the string after =
    line = strtok(line,"\n"); //split the string when a break line is found
    return(&line[1]); //return only the value
}

int main()
{
FILE *config = fopen("config.txt","r+"); //open file and assign it to a handle
char *buffer = NULL;
char buffer2[1000],buffer3[1000];
int buffer_size, read_size;
params param; //initialize a struct to hold metronome parameters
params *paramsptr;
paramsptr = &param;

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

strcpy(paramsptr->waveType,getconfig(buffer,"waveType"));//store the waveType in a struct
paramsptr->frequency = atoi(getconfig(buffer2,"frequency"));//convert frequency and amplitude to int
paramsptr->amplitude = atoi(getconfig(buffer3,"amplitude"));// then store in a struct


printf("%s %d %d\n",paramsptr->waveType,paramsptr->frequency,paramsptr->amplitude);

free(buffer); //free the buffer
return 0;
}