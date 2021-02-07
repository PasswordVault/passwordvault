/**
 * File I/O
 */
#include <Seeed_FS.h>
#include <SD/Seeed_SD.h>

#define FILTER_SIZE 256

bool initFiles(const char* password);
void writeFav(fs::FS& fs, const char* path);
void readFav(fs::FS& fs, const char* path);
void writeFile(fs::FS& fs, const char* path, Entry newent);