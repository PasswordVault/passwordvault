/**
 */
#include "controllers.h"
#include <TFT_eSPI.h>

extern char filter[];
extern char* buffer;
extern Entry* entries;
extern Entry** filtered_entries;
extern unsigned int list_size;
extern unsigned int filtered_list_size;
extern uint8_t cursor_x, cursor_y;
extern unsigned int mode;
extern TFT_eSprite display;

void about();

void
FilterController::setup() {
  Serial.println("Filter.setup");
  Serial.println(filter);
  this->textEntry.setup(buffer, ">", filter, FILTER_WIDTH);
  Serial.println("Filter.setup done.");
}

bool 
prefix(const char *pre, const char *str) {
  return strncasecmp(pre, str, strlen(pre)) == 0;
}

int
filterEntries() {
  int count = 0;
  for (unsigned int i = 0; i < list_size; i++) {
    if (prefix(buffer, entries[i].name)) {
      filtered_entries[count++] = &entries[i];
    }
  }
  return count;
}

void
FilterController::show() {
  this->textEntry.show();
  Serial.print("Key lines: ");
  Serial.println(this->textEntry.keyLines);

  filtered_list_size = filterEntries();

  display.drawFastHLine(0, 80 + this->textEntry.keyLines * 20, 240, TFT_WHITE);
  display.setFreeFont(LIST_FONT);
  display.setCursor(20, 100 + this->textEntry.keyLines * 20);
  display.print(filtered_list_size);
  display.print(" passwords");

  about();

  display.pushSprite(0, 0);
}

void
FilterController::update() {
  this->textEntry.update();
}
