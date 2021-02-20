/**
 * Controllers
 */
#include "constants.h"

class Controller {
  public:
    virtual void setup() {};
    virtual void show() {};
    virtual void update() {};
};

class TextEntry: public Controller {
  private:
    const char* prompt;
    char* buffer;
    unsigned int buf_size;
    const char* keys;
    unsigned int width;
    unsigned int x_offs;
    unsigned int next_mode;
    unsigned int key_len;
    unsigned int cursor_x, cursor_y;

  public:
    unsigned int key_lines;
    void setup(char* buffer, unsigned int buf_size, const char* prompt,
      const char* keys, unsigned int width, unsigned int next_mode=0);
    void show();
    void update();
};

