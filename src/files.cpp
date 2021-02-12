/**
 * File I/O
 */
#include <Arduino.h>
#include <xxtea-lib.h>

#include "entries.h"
#include "files.h"

extern char filter[];
extern char* buffer;
extern unsigned int line_length;
extern unsigned int list_size;
extern Entry* entries;
extern Entry** filtered_entries;
extern Entry** fav_entries;
extern unsigned int fav_list_size;

char* strdup(const char*);

int
countLines(File file, unsigned int* line_length) {
  unsigned int i = 0;
  unsigned int l = 0;
  unsigned int c;
  unsigned int k;
  bool first_field = true;
  char filter_tmp[FILTER_SIZE];

  *line_length = 0;

  memset(filter_tmp, 0, FILTER_SIZE);

  while (file.available()) {
    if ((c = file.read()) == '\n') {
      i++;
      first_field = true;

      if (l > *line_length) {
        *line_length = l;
      }
      l = 0;
    }
    else {
      if (c == '\t') {
        first_field = false;
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
  Serial.print(filter);
  Serial.print(", line_length: ");
  Serial.println(*line_length);

  return i;
}


char*
readField(File file) {
  char c;
  int pos = 0;
  while (file.available() && (c = file.read()) != '\t' && c != '\n') {
    Serial.println(pos);
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
readFile(const char* path) {
  int i = 0;

  Serial.print("Reading file: ");
  Serial.println(path);
  File file = SD.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  list_size = countLines(file, &line_length);
  Serial.print("Lines: ");
  Serial.println(list_size);

  if (buffer) {
    Serial.println("Freeing buffer");
    free(buffer);
  }
  if (entries) {
    Serial.println("Freeing entries");
    for (unsigned int i = 0; i < list_size; i++) {
      free(entries[i].name);
      free(entries[i].passwd);
    }
    free(entries);
    free(filtered_entries);
    free(fav_entries);
  }

  buffer = (char*)malloc(line_length + 1);
  if (!buffer) {
    Serial.println("Can't allocate buffer");
    return;
  }
  entries = (Entry*)malloc(sizeof(Entry) * list_size);
  if (!entries) {
    Serial.println("Can't allocate entries");
    return;
  }
  filtered_entries = (Entry**)malloc(sizeof(Entry*) * list_size);
  if (!filtered_entries) {
    Serial.println("Can't allocate filtered_entries");
    return;
  }
  fav_entries = (Entry**)malloc(sizeof(Entry*) * list_size);
  if (!fav_entries) {
    Serial.println("Can't allocate fav_entries");
    return;
  }

  Serial.println("Reading...");
  while (file.available()) {
    readLine(file, &entries[i]);
    i++;
  }
  Serial.println("Done reading");
  file.close();

  Serial.println("Done.");
}

void
writeLine(File file, Entry* entry) {
  file.write(entry->name);
  file.write("\t");
  file.write(xxtea.encrypt(entry->passwd).c_str());
  file.write("\n");
}

void
writeFile(const char* path, Entry* newent) {
  Serial.print("Writing file: ");
  Serial.println(path);
  File file = SD.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  bool written = false;
  for (unsigned int i = 0; i < list_size; i++) {
    if (0 > strcmp(newent->name, entries[i].name)) {
      writeLine(file, newent);
      written = true;
    }
    writeLine(file, &entries[i]);
  }
  if (!written) {
    writeLine(file, newent);
  }
  file.close();
  Serial.println("Done.");
}

class MyFS: public fs::SDFS {
  public:
    MyFS(fs::FS src) {
      memmove(this, &src, sizeof(fs::FS));
    }
    int getID() { return this->rootSD.id; }
};


void
writeFav(const char* path) {
  Serial.print("Writing fav: ");
  Serial.print(path);
  Serial.print(", size: ");
  Serial.print(fav_list_size);
  Serial.print(", fs: ");
  Serial.print(((MyFS)SD).getID(), HEX);
  Serial.print(", type: ");
  Serial.print(SD.cardType());  
  Serial.println("");
  File file = SD.open(path, FILE_WRITE);
  Serial.println("opened");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  Serial.println("Start");
  for (unsigned int i = 0; i < fav_list_size; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.println(fav_entries[i]->name);
    file.write(fav_entries[i]->name);
    file.write("\n");
  }
  file.close();
  Serial.println("Done.");
}


void
readFav(const char* path) {
  char* name;
  bool found;

  Serial.print("Reading fav: ");
  Serial.println(path);
  File file = SD.open(path, FILE_READ);
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

  Serial.print("SD: ");
  Serial.print(MyFS(SD).getID(), HEX);
  Serial.print(", type: ");
  Serial.print(SD.cardType());  
  Serial.println("");

  readFile("/crypted.txt");
  readFav("/fav.txt");

  return true;
}
