/**
   PasswordVault - Use a WIO Terminal to type passwords
   (c) 2021 Olav Schettler <olav@schettler.net>
*/
#include <Arduino.h> // for platformio

#include <SPI.h>
#include <TFT_eSPI.h>
#include <Keyboard.h>

#include "controllers.h"
#include "files.h"

Controllers controllers;

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite display = TFT_eSprite(&tft);

const char* PASSWD;

int strncasecmp(const char*, const char*, int);
char* strdup(const char*);

unsigned int mode;
Controller* ctl;

unsigned int list_size;
unsigned int filtered_list_size;
unsigned int fav_list_size = 0;

Entry* entries;
char* buffer;

char password[PASSWORD_LENGTH + 1];
char newent[] = "-,/0123456789_abcdefghijklmnopqrstuvwxyz<>";

Entry** filtered_entries;
Entry** fav_entries;
Entry* current_entry;

char filter[FILTER_SIZE];

uint8_t cursor_x = 0, cursor_y = 0;

unsigned int offset = 0;
uint8_t cursor = 0;
unsigned int prefix_pos = 0;

void
setupJoystick() {
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);

  pinMode(WIO_KEY_C, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
}


void
setup() {
  Serial.begin(115200);
  while (!Serial);

# include "./env.h"
  ; 

  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);

  display.setColorDepth(8);
  display.createSprite(240, 320);
  display.fillSprite(TFT_BLACK);
  display.setTextSize(1);
  display.setFreeFont(LIST_FONT);

  setupJoystick();

  Serial.print("PasswordVault ");
  Serial.println(CODE_VERSION);

  if (!initFiles(PASSWD)) {
    return;
  }

  Keyboard.begin();

  setMode(MODE_LOCK);

  Serial.println("Setup complete.");
}


void
about() {
  const int OVERLAY_Y = 220;
  display.setTextColor(TFT_YELLOW, TFT_BLACK);
  display.setFreeFont(ABOUT_FONT);
  display.drawCentreString("PasswordVault", 120, OVERLAY_Y, 1);
  display.setFreeFont(ABOUT_SMALL_FONT);
  display.drawCentreString(CODE_VERSION, 120, OVERLAY_Y + 40, 1);
  display.drawCentreString("(c) 2021 Olav Schettler", 120, OVERLAY_Y + 56, 1);
  display.drawCentreString("info@passwordvault.de", 120, OVERLAY_Y + 72, 1);  
}


int
checkButtons() {
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
    offset = 0;
    cursor = 0;
  
    switch (mode) {
      case MODE_LIST:
        buffer[0] = '\0';
        return MODE_FILTER;
      
      case MODE_FILTER:
        if (strlen(buffer) > 0) {
          return MODE_LIST;
        }
        else {
          return MODE_FAV;
        }
      
      case MODE_FAV:
        return MODE_LIST;
      
      case MODE_DETAIL:
        return MODE_FAV;
      
      case MODE_GENPWD:
        return MODE_FAV;
    }
    Serial.print("mode: ");
    Serial.println(mode);
  }
  else
  if (digitalRead(WIO_KEY_B) == LOW) {
    Serial.println("Gen");
    setMode(MODE_GENPWD);
    return MODE_GENPWD;
  }

  return 0;
}


int
getButtons() {
  int cmd = 0;
  do {
    delay(150);
  }
  while (!(cmd = checkButtons()));

  return cmd;
}


void
typeAndFavEntry(Entry* entry) {
  unsigned int i;
  bool grow = true;

  Keyboard.print(entry->passwd);
  Serial.println(entry->name);

  // Insert the entry as the first favorite

  // First, search the entry from the start of the favorites
  for (i = 0; i < fav_list_size; i++) {
    if (fav_entries[i] == entry) {
      grow = false;
      //D Serial.print("already at ");
      //D Serial.println(i);
      break;
    }
  }
  //D Serial.print("move up to ");
  //D Serial.println(i);
  // Second, move all favorites one up, making space for the new entry at the head of the list
  // This overrides an old occurance of there was one, moving it to the head of the list
  for (int j = i; j > 0; j--) {
    fav_entries[j] = fav_entries[j-1];
  }
  fav_entries[0] = entry;
  if (grow) {
    fav_list_size++;
      writeFav(SD, "/fav.txt");
  }
}

void 
setMode(unsigned int _mode) {
  mode = _mode;
  switch (mode) {
    case MODE_FILTER:
      buffer[0] = '\0';
      cursor_x = 0;
      cursor_y = 0;

      controllers.filter.setup();
      ctl = &controllers.filter;
      break;

    case MODE_LIST:
      controllers.list.setup();
      ctl = &controllers.list;
      break;

    case MODE_FAV: 
      controllers.fav.setup(fav_entries, fav_list_size);
      ctl = &controllers.fav;
      break;

    case MODE_LOCK:
      buffer[0] = '\0';

      controllers.lock.setup();
      ctl = &controllers.lock;
      break;
    
    case MODE_DETAIL:
      offset = 0;
      cursor = 0;

      controllers.detail.setup();
      ctl = &controllers.detail;
      break;

    case MODE_GENPWD:
      controllers.gen.setup();
      ctl = &controllers.gen;
      break;
  }
}


void
loop() {
  Serial.println("Loop");
  ctl->show();
  ctl->update();
}
