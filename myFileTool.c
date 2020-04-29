#include "myFileTool.h"
#include <string.h>

char* readFile(char* filename, int* bytes) 
{
    char* buffer;
    FILE* infile;
    int numbytes;
    /* open an existing file for reading */
    infile = fopen(filename, "rb");

    if (infile == NULL) {
        char* notExistMsg = "File not exist.\n";
        buffer = notExistMsg;
    }
    else {
        /* Get the number of bytes */
        fseek(infile, 0L, SEEK_END);
        numbytes = ftell(infile);

        /* reset the file position indicator to the beginning of the file */
        fseek(infile, 0L, SEEK_SET);	
        
        /* grab sufficient memory for the buffer to hold the text */
        buffer = (char*)calloc(numbytes, sizeof(char));	
        
        /* memory error */
        if(buffer == NULL) exit(EXIT_FAILURE);
        
        /* copy all the text into the buffer */
        fread(buffer, sizeof(char), numbytes, infile);
        fclose(infile);
    }
    /* copy numbytes */
    *bytes = numbytes;
    return buffer;
}

char* getFileList() {
    char* buffer;
    int numbytes = 0;
    /* do ls command to output file */
    system("ls storage -l > output.txt 2>&1");
    /* read output result to buffer */
    buffer = readFile("output.txt", &numbytes);
    /* delete output file */
    system("rm output.txt");
    return buffer;
}

char* deleteFile(char* filename) {
    char* buffer;
    int numbytes = 0;
    /* process command string */
    char command[DEFAULT_SIZE] = "rm storage/";
    strcat(command, filename); 
    strcat(command, " > output.txt 2>&1");
    /* do rm command to output file */
    system(command);
    /* read output result to buffer */
    buffer = readFile("output.txt", &numbytes);
    /* delete output file */
    system("rm output.txt");
    /* if rm success then return success message */
    if (!strlen(buffer)) {
        free(buffer);
        char* successMsg = "File deleted successfully.\n";
        buffer = successMsg;
    }
    return buffer;
}

void writeFile(char* buffer, char* filename, int bytes) {
    FILE* infile;
    /* open file */
    infile = fopen(filename, "ab");
    if (infile == NULL) {
        perror("write file error");
        exit(EXIT_FAILURE);
    }
    else {
        /* copy all the text into the buffer */
        fwrite(buffer, 1, bytes, infile);
        fclose(infile);
    }
}
