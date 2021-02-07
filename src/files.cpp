/**
 * File I/O
 */
#include <Arduino.h>
#include <xxtea-lib.h>

#include "entries.h"
#include "files.h"

extern char filter[];
extern char* buffer;
extern unsigned int list_size;
extern Entry* entries;
extern Entry** filtered_entries;
extern Entry** fav_entries;
extern unsigned int fav_list_size;


int
countLines(File file, int* line_length) {
  int i = 0;
  int l = 0;
  int c;
  int k;
  int first_field = 1;
  char filter_tmp[FILTER_SIZE];

  *line_length = 0;

  memset(filter_tmp, 0, FILTER_SIZE);

  while (file.available()) {
    if ((c = file.read()) == '\n') {
      i++;
      first_field = 1;

      if (l > *line_length) {
        *line_length = l;
      }
      l = 0;
    }
    else {
      if (c == '\t') {
        first_field = 0;
      }
      if (first_field) {
        filter_tmp[tolower(c)] = 1;
      }
      l++;
    }
  }
  file.seek(0);

  if (l > *line_length) {
    *line_length = l;
  }

  memset(filter, 0, FILTER_SIZE);
  for (c = 0, k = 0; c < FILTER_SIZE; c++) {
    if (filter_tmp[c]) {
      filter[k++] = c;
    }
  }
  filter[k] = '\0';
  Serial.print("Filter: ");
  Serial.println(filter);

  return i;
}


char*
readField(File file) {
  char c;
  int pos = 0;
  while (file.available() && (c = file.read()) != '\t' && c != '\n') {
    buffer[pos++] = c;
  }
  buffer[pos] = '\0';
  return buffer;
}


void
readLine(File file, Entry* entry) {
  entry->name = strdup(readField(file));
  entry->passwd = strdup(xxtea.decrypt(readField(file)).c_str());
}


void
readFile(fs::FS& fs, const char* path) {
  int i = 0;
  int line_length;

  Serial.print("Reading file: ");
  Serial.println(path);
  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  list_size = countLines(file, &line_length);
  Serial.print("Lines: ");
  Serial.println(list_size);

  buffer = (char*)malloc(line_length + 1);
  entries = (Entry*)malloc(sizeof(Entry) * list_size);
  filtered_entries = (Entry**)malloc(sizeof(Entry*) * list_size);
  fav_entries = (Entry**)malloc(sizeof(Entry*) * list_size);

  while (file.available()) {
    readLine(file, &entries[i]);
    i++;
  }
  file.close();

  Serial.println("Done.");
}


void
writeFav(fs::FS& fs, const char* path) {
  Serial.print("Writing fav: ");
  Serial.println(path);
  File file = fs.open(path, "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  for (unsigned int i = 0; i < fav_list_size; i ++) {
    file.write(fav_entries[i]->name);
    file.write("\n");
  }
  file.close();
  Serial.println("Done.");
}


void
readFav(fs::FS& fs, const char* path) {
  char* name;
  bool found;

  Serial.print("Reading fav: ");
  Serial.println(path);
  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  while (file.available()) {
    name = readField(file);
    found = false;
    for (unsigned int i = 0; i < list_size; i++) {
      if (0 == strcmp(entries[i].name, name)) {
        fav_entries[fav_list_size++] = &entries[i];
        Serial.print("Fav ");
        Serial.print(name);
        Serial.println(" added");
        found = true;
        break;
      }
    }
    if (!found) {
      Serial.print("Fav ");
      Serial.print(name);
      Serial.println(" not found");
    }
  }
  file.close();

  Serial.print(fav_list_size);
  Serial.println(" favs read.");
}


bool
initFiles(const char* password) {
  xxtea.setKey(password);

  if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI, 4000000UL)) {
    Serial.println("Card mount failed");
    return false;
  }

  uint8_t card_type = SD.cardType();
  if (card_type == CARD_NONE) {
    Serial.println("No SD card attached");
    return false;
  }

  readFile(SD, "/crypted.txt");
  readFav(SD, "/fav.txt");

  return true;
}
