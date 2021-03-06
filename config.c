#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    unsigned int waveType;
    float amplitude;
    float frequency; 
} params;

char* getconfig(char* file,char* config){ //function to the value of a specific parameter e.g. frequency from the parsed config file
    char *line;
    line = strstr(file,config); //go to the line containing the required config
    line = strstr(line,"="); // store only the string after =
    line = strtok(line,"\n"); //split the string when a break line is found
    return(&line[1]); //return only the value
}

void read_config(char *fileName, params *paramptr){
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
    paramptr->frequency = atof(getconfig(buffer2,"frequency"));//convert frequency and amplitude to float
    paramptr->amplitude = atof(getconfig(buffer3,"amplitude"));// then store in a struct

}

void save_config(char* fileName,params *paramsptr){
    /* Function to save inputted parameters into a config file
   Input : Config file name and path(if necessary), Structure to hold the parameters*/ 
    FILE *config = fopen(fileName,"w+"); //open file and assign it to a handle
    fseek(config,0,SEEK_SET);//set the cursor to the start of the file
    /*write the updated parameters to the config file*/
    fprintf(config,"waveType=%d\n",paramsptr->waveType);
    fprintf(config,"frequency=%f\n",paramsptr->frequency);
    fprintf(config,"amplitude=%f\n",paramsptr->amplitude);
    fclose(config);
}



int main()
{
params param;
// read_config("config.txt",&param);
// printf("%d %f %f\n",param.waveType,param.frequency,param.amplitude);
param.waveType=2;
param.frequency=4;
param.amplitude=3;
save_config("config.txt",&param);
return 0;
}