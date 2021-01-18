/**
   PasswordVault - Use a WIO Terminal to type passwords
   (c) 2021 Olav Schettler <olav@schettler.net>
*/
#define CODE_VERSION "v1.0"

#include <xxtea-lib.h>

#include <SPI.h>
#include <TFT_eSPI.h>

#include "Keyboard.h"

#include <Seeed_FS.h>
#include "SD/Seeed_SD.h"
#define SERIAL Serial

TFT_eSPI tft = TFT_eSPI();

char* PASSWD;

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define SELECT 5
#define MODE_FILTER 6
#define MODE_LIST 7
#define MODE_LOCK 8
int mode = MODE_LOCK;

bool locked = true;

#define SCREEN_SIZE 13
int list_size;
int filtered_list_size;

typedef struct {
  char* passwd;
  char* name;
} Entry;

Entry* entries;
char* buffer;

Entry** filtered_entries;

#define FILTER_SIZE 256
char filter[FILTER_SIZE];
#define FILTER_WIDTH 10
int filter_lines;
int filter_size = 0;

char lock[] = "123456789<0>";
#define LOCK_WIDTH 3

int cursor_x = 0, cursor_y = 0;

int offset = 0;
int cursor = 0;
int prefix_pos = 0;


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
  filter_size = k;

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

  buffer = (char*)malloc(line_length + 1);
  entries = (Entry*)malloc(sizeof(Entry) * list_size);
  filtered_entries = (Entry**)malloc(sizeof(Entry*) * list_size);

  while (file.available()) {
    readLine(file, &entries[i]);
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

  pinMode(WIO_KEY_C, INPUT_PULLUP);
}


void
setup() {
  Serial.begin(115200);
  //while (!Serial);

# include "./env.h"
  ; 

  xxtea.setKey(PASSWD);

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

  //readFile(SD, "/olav.txt");
  readFile(SD, "/crypted.txt");

  Keyboard.begin();

  strcpy(buffer, "");
}


bool 
prefix(const char *pre, const char *str) {
  return strncmp(pre, str, strlen(pre)) == 0;
}


int
filterEntries() {
  int count = 0;
  for (int i = 0; i < list_size; i++) {
    if (prefix(buffer, entries[i].name)) {
      filtered_entries[count++] = &entries[i];
    }
  }
  return count;
}


void
about() {
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.drawCentreString("PasswordVault", 120, 220, 2);
  tft.setTextSize(1);
  tft.drawCentreString(CODE_VERSION, 120, 260, 1);
  tft.drawCentreString("(c) 2021 Olav Schettler", 120, 276, 1);
  tft.drawCentreString("info@passwordvault.de", 120, 292, 1);  
}


void
showLock() {
  int x = 0, y = 0;

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.drawString(">", 20, 10);
  tft.drawString(buffer, 40, 10);

  tft.drawFastHLine(0, 30, 240, TFT_WHITE);

  for (int i = 0; i < sizeof(lock); i++) {
    char c[2] = { lock[i], '\0' };
    if (x == cursor_x && y == cursor_y) {
      tft.setTextColor(TFT_BLACK, TFT_WHITE);
    }
    else {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    tft.drawString(c, 90 + x * 20, 50 + y * 20);
    x++;
    if (x >= LOCK_WIDTH) {
      x = 0;
      y++;
    }
  }

  tft.drawFastHLine(0, 150, 240, TFT_WHITE);
  tft.drawCentreString("Please unlock", 120, 160, 1);
  
  about();
}


void
showFilter() {
  int x = 0, y = 0;

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.drawString(">", 20, 10);
  tft.drawString(buffer, 40, 10);

  tft.drawFastHLine(0, 30, 240, TFT_WHITE);

  for (int i = 0; i < filter_size; i++) {
    char c[2] = { filter[i], '\0' };
    if (x == cursor_x && y == cursor_y) {
      tft.setTextColor(TFT_BLACK, TFT_WHITE);
    }
    else {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    tft.drawString(c, 20 + x * 20, 50 + y * 20);
    x++;
    if (x >= FILTER_WIDTH) {
      x = 0;
      y++;
    }
  }
  filter_lines = y;

  filtered_list_size = filterEntries();

  tft.drawFastHLine(0, 80 + filter_lines * 20, 240, TFT_WHITE);

  tft.setCursor(20, 100 + filter_lines * 20);
  tft.print(filtered_list_size);
  tft.print(" passwords");

  about();
}


void
showList() {
  Serial.print("offset / cursor: ");
  Serial.print(offset);
  Serial.print(" ");
  Serial.println(cursor);

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.drawString("#", 20, 10);
  tft.drawString(buffer, 40, 10);

  tft.drawFastHLine(0, 30, 240, TFT_WHITE);

  for (int i = 0; offset + i < filtered_list_size && i < SCREEN_SIZE; i++) {
    if (i == cursor) {
      tft.setTextColor(TFT_BLACK, TFT_WHITE);
    }
    else {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    tft.drawString(filtered_entries[offset + i]->name, 20, 50 + 20 * i);
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
  if (digitalRead(WIO_5S_UP) == LOW) {
    return RIGHT;
  }
  else
  if (digitalRead(WIO_5S_DOWN) == LOW) {
    return LEFT;
  }
  else
  if (digitalRead(WIO_5S_PRESS) == LOW) {
    return SELECT;
  }
  else
  if (digitalRead(WIO_KEY_C) == LOW) {
    if (mode == MODE_LIST) {
      buffer[0] = '\0';
      return MODE_FILTER;
    }
    else {
      return MODE_LIST;
    }
  }

  return 0;
}


void
lockCursor() {
  int i;
  int buffer_len;
  int cmd = 0;

  delay(200);
  while (!(cmd = checkJoystick())) {
    delay(100);
  }
  switch (cmd) {
    case LEFT:
      if (cursor_x > 0) {
        cursor_x--;
      }
      break;

    case RIGHT:
      if (cursor_x < LOCK_WIDTH - 1) {
        cursor_x++;
      }
      break;

    case UP:
      if (cursor_y > 0) {
        cursor_y--;
      }
      break;

    case DOWN:
      if (cursor_y < sizeof(lock) / LOCK_WIDTH) {
        cursor_y++;
      }
      break;

    case SELECT:
      i = cursor_y * LOCK_WIDTH + cursor_x;
      buffer_len = strlen(buffer);
      switch (lock[i]) {
        case '<':
          if (buffer_len > 0) {
            buffer[buffer_len - 1] = '\0';
          }
          break;

        case '>':
          if (0 == strcmp(PASSWD, buffer)) {
            Serial.println("Unlocked :)");
            mode = MODE_FILTER;
            buffer[0] = '\0';
            cursor_x = 0;
            cursor_y = 0;
          }
          else {
            buffer[0] = '\0';
          }
          break;

        default:
          if (buffer_len > 7) {
            break;
          }
          buffer[buffer_len] = lock[i];
          buffer[buffer_len + 1] = '\0';
      }
      break;
  }
}

void
filterCursor() {
  int i, new_i;
  int buffer_len;
  int cmd = 0;

  delay(200);
  while (!(cmd = checkJoystick())) {
    delay(100);
  }
  switch (cmd) {
    case LEFT:
      if (cursor_x > 0) {
        cursor_x--;
      }
      break;

    case RIGHT:
      new_i = cursor_y * FILTER_WIDTH + cursor_x + 1;
      if (cursor_x < FILTER_WIDTH - 1 && new_i < filter_size - 1) {
        cursor_x++;
      }
      break;

    case UP:
      if (cursor_y > 0) {
        cursor_y--;
      }
      break;

    case DOWN:
      new_i = (cursor_y + 1) * FILTER_WIDTH + cursor_x;
      if (cursor_y < filter_lines && new_i < filter_size) {
        cursor_y++;
      }
      break;

    case SELECT:
      i = cursor_y * FILTER_WIDTH + cursor_x;
      buffer_len = strlen(buffer);
      buffer[buffer_len] = filter[i];
      buffer[buffer_len + 1] = '\0';
      break;

    case MODE_LIST:
      offset = 0;
      cursor = 0;
    
    case MODE_FILTER:
      mode = cmd;
      break;
  }
  i = cursor_y * FILTER_WIDTH + cursor_x;
  Serial.print("Filter [");
  Serial.print(i);
  Serial.print("] = ");
  Serial.print(filter[i]);
  Serial.print(", filter size: ");
  Serial.println(filter_size);
}

void
listCursor() {
  int cmd = 0;

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
      if (cursor < min(SCREEN_SIZE - 1, filtered_list_size - 1)) {
        cursor++;
      }
      else
      if (offset < filtered_list_size - SCREEN_SIZE) {
        offset++;
      }
      break;

    case SELECT:
      Keyboard.print(filtered_entries[offset + cursor]->passwd);
      Serial.println(filtered_entries[offset + cursor]->name);
      break;

    case MODE_LIST:
      offset = 0;
    
    case MODE_FILTER:
      mode = cmd;
      break;
  }
}

void
loop() {
  switch (mode) {
    case MODE_FILTER:    
      showFilter();
      filterCursor();
      break;

    case MODE_LIST:
      showList();
      listCursor();
      break;

    case MODE_LOCK:
      showLock();
      lockCursor();
      break;
  }
}
