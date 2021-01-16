
/*WiFi
#include <rpcWiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
*/

#include <SPI.h>
#include <TFT_eSPI.h>

#include "Keyboard.h"

#include <Seeed_FS.h>
#include "SD/Seeed_SD.h"
#define SERIAL Serial

TFT_eSPI tft = TFT_eSPI();

#define UP 1
#define DOWN 2
#define SELECT 3

#define SCREEN_SIZE 15
int list_size;

typedef struct {
  char* passwd;
  char* name;
} Entry;

Entry* entries;

int offset = 0;
int cursor = 0;

int
countLines(File file, int* line_length) {
  int i = 0;
  int l = 0;

  *line_length = 0;
  
  while (file.available()) {
    if (file.read() == '\n') {
      i++;

      if (l > *line_length) {
        *line_length = l;
      }
      l = 0;
    }
    else {
      l++;
    }
  }
  file.seek(0);
  return i;
}

char*
readField(File file, char* buffer) {
  char c;
  int pos = 0;
  while (file.available() && (c = file.read()) != '\t' && c != '\n') {
    buffer[pos++] = c;
  }
  buffer[pos] = '\0';
  return buffer;
}

void
readLine(File file, Entry* entry, char* buffer) {
  entry->name = strdup(readField(file, buffer));
  entry->passwd = strdup(readField(file, buffer));
}

void 
readFile(fs::FS& fs, const char* path) {
  int i = 0;
  int line_length;
  
  SERIAL.print("Reading file: ");
  SERIAL.println(path);
  File file = fs.open(path);
  if (!file) {
      SERIAL.println("Failed to open file for reading");
      return;
  }

  list_size = countLines(file, &line_length);
  SERIAL.print("Lines: ");
  SERIAL.println(list_size);

  char buffer[line_length + 1];
  
  entries = (Entry*)malloc(sizeof(Entry) * list_size);
  
  while (file.available()) {
    SERIAL.println(i);

    readLine(file, &entries[i], buffer);
    i++;
  }
  file.close();

  SERIAL.println("Done.");
}

void
setupJoystick() {
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
}

void
setup() {
  Serial.begin(115200);
  while(!Serial);
  
  tft.init();
  tft.setRotation(2);
  
  setupJoystick();
      
  if (!SD.begin(SDCARD_SS_PIN, SDCARD_SPI, 4000000UL)) {
    SERIAL.println("Card mount failed");
    return;
  }

  uint8_t card_type = SD.cardType();
  if (card_type == CARD_NONE) {
    SERIAL.println("No SD card attached");
    return;
  }

  readFile(SD, "/olav.txt");

  /*
  SERIAL.print(list_size);
  SERIAL.println(" Entries");
  for (int i=0; i<list_size; i++) {
    SERIAL.print("Entry n=");
    SERIAL.print(entries[i].name);
    SERIAL.print(", p=");
    SERIAL.println(entries[i].passwd);
  }
  */
  
  Keyboard.begin();
}

void
showList(int offset, int cursor) {
  Serial.print("offset/cursor: ");
  Serial.print(offset);
  Serial.print(" ");
  Serial.println(cursor);

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);

  for (int i = 0; offset + i < list_size && i < SCREEN_SIZE; i++) {
    if (i == cursor) {
      tft.setTextColor(TFT_BLACK, TFT_WHITE);
    }
    else {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);  
    }
    tft.drawString(entries[offset + i].name, 20, 20 * i);
  }
}

int
checkJoystick() {
  if (digitalRead(WIO_5S_LEFT) == LOW) {
    return UP;
  }
  else 
  if (digitalRead(WIO_5S_RIGHT) == LOW) {
    return DOWN;
  }
  else 
  if (digitalRead(WIO_5S_PRESS) == LOW) {
    return SELECT;
  }
  return 0;
}

void
loop() {
  int cmd = 0;
  showList(offset, cursor);

  delay(200);
  while (!(cmd = checkJoystick())) {
    delay(100);
  }
  switch (cmd) {
    case UP:
      if (cursor > 0) {
        cursor--;
      }
      else
      if (offset > 0) {
        offset--;
      }
      break;

    case DOWN:
      if (cursor < SCREEN_SIZE - 1) {
        cursor++;
      }
      else
      if (offset < list_size - SCREEN_SIZE) {
        offset++;
      }
      break;

    case SELECT:
      Keyboard.print(entries[offset + cursor].passwd);
      Serial.println(entries[offset + cursor].name);
      break;
  }
}
