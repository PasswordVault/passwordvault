/**
 */
#include "controllers.h"
#include <TFT_eSPI.h>

extern unsigned int mode;
extern Entry** filtered_entries;
extern unsigned int filtered_list_size;
extern unsigned int offset;
extern uint8_t cursor;
extern char* buffer;
extern Entry* current_entry;
extern TFT_eSprite display;

extern void typeAndFavEntry(Entry* entry);

void
ListController::show() {
  Serial.print("All offset / cursor: ");
  Serial.print(offset);
  Serial.print(" ");
  Serial.println(cursor);

  display.fillScreen(TFT_BLACK);
  display.setFreeFont(PROMPT_FONT);
  display.setTextColor(TFT_WHITE, TFT_BLACK);

  display.drawString("#", 20, 10);
  display.drawString(buffer, 40, 10);

  display.drawFastHLine(0, 30, 240, TFT_WHITE);
  display.setFreeFont(LIST_FONT);

  for (unsigned int i = 0; offset + i < filtered_list_size && i < SCREEN_SIZE; i++) {
    if (i == cursor) {
      display.setTextColor(TFT_BLACK, TFT_WHITE);
    }
    else {
      display.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    display.drawString(filtered_entries[offset + i]->name, LIST_LEFT_MARGIN, LIST_TOP_MARGIN + LIST_VSPACE * i);
  }
  display.pushSprite(0, 0);
}


void
ListController::update() {
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
      if (cursor < min(SCREEN_SIZE - 1, filtered_list_size - 1)) {
        cursor++;
      }
      else
      if (cursor == SCREEN_SIZE - 1 && offset < filtered_list_size - SCREEN_SIZE) {
        offset++;
      }
      break;

    case RIGHT:
    case LEFT:
      break;

    case SELECT:
      current_entry = filtered_entries[offset + cursor];
      typeAndFavEntry(current_entry);
      setMode(MODE_DETAIL);
      break;

    default:
      setMode(cmd);
      break;
  }
}
