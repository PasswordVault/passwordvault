/**
 * Controllers
 */
#include "constants.h"
#include "entries.h"

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

class LockController: public Controller {
  public:
    void show();
    void update();
};

class FilterController: public Controller {
  private:
    TextEntry textEntry;
  public:
    void setup();
    void show();
    void update();
};

class ListController: public Controller {
  public:
    void show();
    void update();
};

class GenController: public Controller {
  private:
    TextEntry textEntry;
    void genPassword();
  public:
    void setup();
    void show();
    void update();
};

class FavController: public Controller {
  private:
    Entry** entries;
    unsigned int list_size;

  public:
    void setup(Entry** entries, int list_size);
    void show();
    void update();
};

class DetailController: public Controller {
  public:
    void show();
    void update();
};

typedef struct Controllers {
  DetailController detail;
  FavController fav;
  FilterController filter;
  GenController gen;
  ListController list;
  LockController lock;
} Controllers;

int getButtons();
void setMode(unsigned int m);
