/**
 */
#include "controllers.h"
#include <TFT_eSPI.h>

extern unsigned int mode;
extern const char* PASSWD;
extern char* buffer;

extern uint8_t cursor_x, cursor_y;
extern TFT_eSprite display;

const char lock[] = "123456789<0>";

void about();

void
LockController::show() {
  Serial.println("Lock.show");
  unsigned int x = 0, y = 0;

  display.fillScreen(TFT_BLACK);
  display.setFreeFont(PROMPT_FONT);
  display.setTextColor(TFT_WHITE, TFT_BLACK);

  display.drawString(">", 20, 10);
  display.drawString(buffer, 40, 10);

  display.drawFastHLine(0, 30, 240, TFT_WHITE);

  for (unsigned int i = 0; i < sizeof(lock); i++) {
    char c[2] = { lock[i], '\0' };
    if (x == cursor_x && y == cursor_y) {
      display.setTextColor(TFT_BLACK, TFT_WHITE);
    }
    else {
      display.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    display.drawString(c, 90 + x * 20, 50 + y * 20);
    x++;
    if (x >= LOCK_WIDTH) {
      x = 0;
      y++;
    }
  }

  display.drawFastHLine(0, 150, 240, TFT_WHITE);
  display.setFreeFont(ABOUT_FONT);
  display.drawCentreString("Please unlock", 120, 160, 1);

  about();

  display.pushSprite(0, 0);
}


void
LockController::update() {
  int i;
  int buffer_len;
  int cmd = getButtons();

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
      if (cursor_y < sizeof(lock) / LOCK_WIDTH - 1) {
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
            setMode(MODE_FILTER);
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
