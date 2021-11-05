#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    FILE *config = fopen("config.txt","r+"); //open file and assign it to a handle
    char *buffer = NULL;
    int buffer_size, read_size;
    char *pos;

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

    pos = strstr(buffer,"frequency");

    printf("%s",pos);


    // //check if everything is read properly
    // if (buffer_size != read_size){
    //     printf("File not read properly!!!"); //error handling and free up buffer
    //     free(buffer);
    //     buffer = NULL;
    // }

    free(buffer);
    fclose(config);
return 0;
}