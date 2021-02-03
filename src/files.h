/**
 * File I/O
 */
#include <Seeed_FS.h>
#include <SD/Seeed_SD.h>

#define FILTER_SIZE 256

typedef struct {
  char* passwd;
  char* name;
} Entry;

void initFiles(const char* password);
int countLines(File file, int* line_length);
char* readField(File file);
void readLine(File file, Entry* entry);
void readFile(fs::FS& fs, const char* path);
void writeFav(fs::FS& fs, const char* path);
void readFav(fs::FS& fs, const char* path);
