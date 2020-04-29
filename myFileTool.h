#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#define DEFAULT_SIZE 1024

char* readFile(char* filename, int* bytes);
char* getFileList();
char* deleteFile(char* filename);
void writeFile(char* buffer, char* filename, int bytes);