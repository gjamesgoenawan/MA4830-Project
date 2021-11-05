#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    FILE *config = fopen("config.txt","r+"); //open file and assign it to a handle
    char * line = NULL;
    size_t len = 0;
    char params[2][10];

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
    puts(params[0]);

    fclose(config);
return 0;
}