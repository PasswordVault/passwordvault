/**
 */
#include "controllers.h"
#include <TFT_eSPI.h>

extern unsigned int mode;
extern TFT_eSprite display;

extern int getButtons();
extern void setMode(unsigned int _mode);

void
TextEntry::setup(
    char* buffer,               // the input buffer 
    unsigned int buf_size,      // the maximum buffer size
    const char* prompt,         // the prompt displayed left of the input buffer
    const char* keys,           // the list of available characters
    unsigned int width,         // number of columns in which to present the available colors
    unsigned int next_mode      // if != 0, determines the mode to set on ">" 
  ) {
  this->buffer = buffer;
  this->buffer[0] = '\0';
  this->buf_size = buf_size;
  this->prompt = prompt;
  this->keys = keys;
  this->width = width;
  this->x_offs = DISPLAY_WIDTH / 2 - 10 * this->width;
  this->next_mode = next_mode;
  this->key_len = strlen(keys);
  this->key_lines = ceil(this->key_len / (float)this->width);
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

  display.drawFastHLine(0, 30, DISPLAY_WIDTH, TFT_WHITE);

  for (unsigned int i = 0; i < this->key_len; i++) {
    char c[2] = { this->keys[i], '\0' };
    if (x == this->cursor_x && y == this->cursor_y) {
      display.setTextColor(TFT_BLACK, TFT_WHITE);
    }
    else {
      display.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    display.drawString(c, this->x_offs + x * 20, 50 + y * 22);
    x++;
    if (x >= this->width) {
      x = 0;
      y++;
    }
  }
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.drawFastHLine(0, 70 + this->key_lines * 20, DISPLAY_WIDTH, TFT_WHITE);
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
      if (this->cursor_x < this->width - 1 && new_i < this->key_len) {
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
      if (this->cursor_y < this->key_lines && new_i < this->key_len) {
        this->cursor_y++;
      }
      break;

    case SELECT:
      i = this->cursor_y * this->width + this->cursor_x;
      buffer_len = strlen(this->buffer);
      switch (this->keys[i]) {
        case '<':
          if (buffer_len > 0) {
            this->buffer[buffer_len - 1] = '\0';
          }
          break;

        case '>':
          if (this->next_mode && buffer_len > 0) {
            setMode(this->next_mode);
            break;
          }

        default:
          if (buffer_len + 1 >= this->buf_size) {
            break;
          }
          this->buffer[buffer_len] = this->keys[i];
          this->buffer[buffer_len + 1] = '\0';
      }
      break;

    default:
      if (!this->next_mode || this->buffer[0] == '\0') {
        setMode(cmd);
      }
      break;
  }
  i = this->cursor_y * this->width + this->cursor_x;
  Serial.print("keys[");
  Serial.print(i);
  Serial.print("] = ");
  Serial.print(this->keys[i]);
  Serial.print(", key_len: ");
  Serial.print(this->key_len);
  Serial.print(", mode: ");
  Serial.println(mode);
}
