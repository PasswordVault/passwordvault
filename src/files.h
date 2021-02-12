/**
 * File I/O
 */
#include <Seeed_FS.h>
#include <SD/Seeed_SD.h>

#define FILTER_SIZE 256

bool initFiles(const char* password);
void writeFav(const char* path);
void readFav(const char* path);
void writeFile(const char* path, Entry* newent);