/**
 */
#include "controllers.h"
#include <TFT_eSPI.h>

extern unsigned int mode;
extern TFT_eSprite display;

void
TextEntry::setup(char* buffer, const char* prompt, const char* keys, unsigned int width) {
  this->buffer = buffer;
  this->buffer[0] = '\0';
  this->prompt = prompt;
  this->keys = keys;
  this->width = width;
  this->keyLen = strlen(keys);
  this->cursor_x = this->cursor_y = 0;
}

void
TextEntry::show() {
  Serial.println("TextEntry.show");
  Serial.print("keys: ");
  Serial.println(keys);

  unsigned int x = 0, y = 0;

  display.fillScreen(TFT_BLACK);
  display.setFreeFont(PROMPT_FONT);
  display.setTextColor(TFT_WHITE, TFT_BLACK);

  display.drawString(this->prompt, 20, 10);
  display.drawString(this->buffer, 40, 10);

  display.drawFastHLine(0, 30, 240, TFT_WHITE);

  for (unsigned int i = 0; i < this->keyLen; i++) {
    char c[2] = { this->keys[i], '\0' };
    if (x == this->cursor_x && y == this->cursor_y) {
      display.setTextColor(TFT_BLACK, TFT_WHITE);
    }
    else {
      display.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    display.drawString(c, 20 + x * 20, 50 + y * 20);
    x++;
    if (x >= this->width) {
      x = 0;
      y++;
    }
  }
  this->keyLines = y;
}

void
TextEntry::update() {
  unsigned int i, new_i;
  unsigned int buffer_len;
  int cmd = getButtons();

  switch (cmd) {
    case LEFT:
      if (this->cursor_x > 0) {
        this->cursor_x--;
      }
      break;

    case RIGHT:
      new_i = this->cursor_y * this->width + this->cursor_x + 1;
      if (this->cursor_x < this->width - 1 && new_i < this->keyLen - 1) {
        this->cursor_x++;
      }
      break;

    case UP:
      if (this->cursor_y > 0) {
        this->cursor_y--;
      }
      break;

    case DOWN:
      new_i = (this->cursor_y + 1) * this->width + this->cursor_x;
      if (this->cursor_y < this->keyLines && new_i < this->keyLen) {
        this->cursor_y++;
      }
      break;

    case SELECT:
      i = this->cursor_y * this->width + this->cursor_x;
      buffer_len = strlen(this->buffer);
      this->buffer[buffer_len] = this->keys[i];
      this->buffer[buffer_len + 1] = '\0';
      break;

    default:
      setMode(cmd);
      break;
  }
  i = this->cursor_y * this->width + this->cursor_x;
  Serial.print("Filter [");
  Serial.print(i);
  Serial.print("] = ");
  Serial.print(this->keys[i]);
  Serial.print(", filter size: ");
  Serial.println(this->keyLen);
}
