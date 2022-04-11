#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "betterexec.h"

#define FILENAME_SIZE 64
#define MAX_LINE 256

int registerApp(char file[]) {
    FILE *list = fopen("/etc/Neptune/list", "a");
    
    fprintf(list, "%s\n", file);
    fclose(list);
}

int unregisterApp(char filename[], char delete_line[])
{
    char temp_filename[FILENAME_SIZE] = "/tmp/temppfile";
    char buffer[MAX_LINE];
    strcat(delete_line, "\n");

    FILE *file, *temp;

    // open the original file for reading and the temp file for writing
    file = fopen(filename, "r");
    temp = fopen(temp_filename, "w");

    if (file == NULL || temp == NULL)
    {
        printf("Error opening file(s).\n");
        return 1;
    }

    bool keep_reading = true;
    do {

        // stores the next line from the file into the buffer        
        fgets(buffer, MAX_LINE, file);

        // if we've reached the end of the file, stop reading from the file
        if (feof(file)) keep_reading = false;
        else if (strcmp(buffer, delete_line) != 0) {
            fputs(buffer, temp);
        }

    } while (keep_reading);

    // close our access to the files
    fclose(file);
    fclose(temp);

    // delete the original file, give the temp file the name of the original file
    remove(filename);
    sexecl("/usr/bin/mv", "/tmp/temppfile", filename, NULL);

    return 0;
}