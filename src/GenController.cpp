/**
 */
#include "controllers.h"
#include <TFT_eSPI.h>

extern char* buffer;
extern char* password;
extern char* newent;
extern TFT_eSprite display;

void
GenController::setup() {
  this->textEntry.setup(buffer, "!", newent, NEWENT_WIDTH);

  const char specials[] = "!$%&/()=?+*#.,-@";
  randomSeed(millis());

  for (int i = 0; i < PASSWORD_LENGTH; i++) {
    password[i] = random('a', 'z' + 1);
  }
  password[PASSWORD_LENGTH] = '\0';

  char special = specials[random(strlen(specials) - 1)];
  int special_pos = random(PASSWORD_LENGTH);
  password[special_pos] = special;

  char digit = random('0', '9' + 1);
  int digit_pos = 0;
  do {
    digit_pos = random(PASSWORD_LENGTH);
  }
  while (digit_pos == special_pos);
  password[digit_pos] = digit;

  char capital = random('A', 'Z' + 1);
  int capital_pos = 0;
  do {
    capital_pos = random(PASSWORD_LENGTH);
  }
  while (capital_pos == special_pos || capital_pos == digit_pos);
  password[capital_pos] = capital;
}


void
GenController::show() {
  this->textEntry.show();

  display.setFreeFont(PROMPT_LARGE_FONT);
  display.drawString(password, 20, 200);

  display.pushSprite(0, 0);
}


void
GenController::update() {
  this->textEntry.update();
}
