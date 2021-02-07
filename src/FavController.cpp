/**
 */
#include "controllers.h"
#include <TFT_eSPI.h>

extern unsigned int mode;
extern unsigned int offset;
extern uint8_t cursor;
extern Entry** fav_entries;
extern Entry* current_entry;
extern char* buffer;
extern TFT_eSprite display;

extern void typeAndFavEntry(Entry* entry);

void
FavController::setup(Entry** entries, int list_size) {
  this->entries = entries;
  this->list_size = list_size;
}

void
FavController::show() {
  Serial.print("Fav offset=");
  Serial.print(offset);
  Serial.print(" cursor=");
  Serial.print(cursor);
  Serial.print(" size=");
  Serial.println(this->list_size);

  display.fillScreen(TFT_BLACK);
  display.setFreeFont(PROMPT_FONT);
  display.setTextColor(TFT_WHITE, TFT_BLACK);

  display.drawString("*", 20, 10);
  display.drawString(buffer, 40, 10);

  display.drawFastHLine(0, 30, 240, TFT_WHITE);
  display.setFreeFont(LIST_FONT);

  if (this->list_size > 0) {
    for (unsigned int i = 0; offset + i < this->list_size && i < SCREEN_SIZE; i++) {
      if (i == cursor) {
        display.setTextColor(TFT_BLACK, TFT_WHITE);
      }
      else {
        display.setTextColor(TFT_WHITE, TFT_BLACK);
      }
      //D Serial.println(offset + i);
      //D Serial.println(fav_entries[offset + i]->name);
      display.drawString(fav_entries[offset + i]->name, LIST_LEFT_MARGIN, LIST_TOP_MARGIN + LIST_VSPACE * i);
    }
  }
  else {
    display.setTextColor(TFT_YELLOW, TFT_BLACK);
    display.setFreeFont(ABOUT_FONT);
    display.drawCentreString("No favorites yet", 120, 160, 1);
  }
  display.pushSprite(0, 0);
}

void
FavController::update() {
  int cmd = getButtons();

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
      if (cursor < min(SCREEN_SIZE - 1, this->list_size - 1)) {
        cursor++;
      }
      else 
      if (cursor == SCREEN_SIZE - 1 && offset < this->list_size - SCREEN_SIZE) {
        offset++;
      }
      break;

    case RIGHT:
    case LEFT:
      break;

    case SELECT:
      current_entry = this->entries[offset + cursor];
      typeAndFavEntry(current_entry);
      setMode(MODE_DETAIL);
      break;

    default:
      setMode(cmd);
      break;
  }
}
