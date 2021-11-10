#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define num_of_params 3

int main()
{
    FILE *config = fopen("config.txt","r+"); //open file and assign it to a handle
    char * line = NULL;
    size_t len = 0;
    char params[num_of_params][10];
    char labels[num_of_params][20]={"waveType","frequency","amplitude"};

    if(!config) { //error handling if cannot open config file
        perror("Cannot open config file!");
        exit(1);
    }

    int i=0;
    while ((getline(&line, &len, config)) != -1) { //reads line by line in the config file
        line = strtok(line,"\n"); //remove break lines
        line = strstr(line,"="); //only look at the value after = sign
        strcpy(params[i],&line[1]); //store the value into an array
        i++;
    }

    fseek(config,0,SEEK_SET);//set the cursor to the start of the file
    for(int i=0;i<num_of_params;i++){ //write the updated parameters to the config file
        fprintf(config,"%s=%s\n",labels[i],params[i]);
    }

    fclose(config); //close the handle
return 0;
}