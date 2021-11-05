#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    FILE *config = fopen("config.txt","r+"); //open file and assign it to a handle
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char *params[10];
    int i=0;

    if(!config) { //error handling if cannot open config file
        perror("Cannot open config file!");
        exit(1);
    }

    while ((read = getline(&line, &len, config)) != -1) {
        line = strstr(line,"=");
        params[i] = &line[1];
        printf("%s\n",params[i]);
        i++;
    }
    printf("%s\n",params[0]);
    fclose(config);
return 0;
}