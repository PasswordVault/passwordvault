/**
 */
#include "controllers.h"
#include <TFT_eSPI.h>

extern TFT_eSprite display;

extern unsigned int mode;
extern Entry* current_entry;

void
DetailController::show() {
  display.fillScreen(TFT_BLACK);
  display.setFreeFont(PROMPT_FONT);
  display.setTextColor(TFT_WHITE, TFT_BLACK);

  display.drawString(":", 20, 10);
  display.drawString(current_entry->name, 40, 10);

  display.drawFastHLine(0, 30, 240, TFT_WHITE);
  display.setFreeFont(ABOUT_FONT);

  display.drawString(current_entry->passwd, 20, 50);

  display.pushSprite(0, 0);
}

void
DetailController::update() {
  int cmd = getButtons();

  switch (cmd) {
    case MODE_FAV:
      setMode(cmd);
      break;
  }
}

