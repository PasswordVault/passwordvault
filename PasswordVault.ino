
/*WiFi
#include <rpcWiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiManager.h>
*/

#include <SPI.h>
#include <TFT_eSPI.h>

#include "Keyboard.h"

TFT_eSPI tft = TFT_eSPI();

#define UP 1
#define DOWN 2
#define SELECT 3

#define SCREEN_SIZE 4
#define LIST_SIZE 10

typedef struct {
  String passwd;
  String name;
} Entry;

Entry entries[] = {
  { "Lx1jOf3l", "Amazon" },
  { "bbbb", "B..." },
  { "cccc", "C..." },
  { "dddd", "D..." },
  { "eeee", "E..." },
  { "ffff", "F..." },
  { "gggg", "G..." },
  { "hhhh", "H..." },
  { "iiii", "I..." },
  { "jjjj", "J..." },
};

int offset = 0;
int cursor = 0;

void
setupJoystick() {
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
}

void
setup() {
  Serial.begin(115200);
  while(!Serial);
  
  /*WiFi
  WiFiManager wifiManager;
  wifiManager.autoConnect("PasswordVault");
  Serial.println("Connected :)");
  Serial.println(WiFi.localIP());
  */
  
  Keyboard.begin();
  
  tft.init();
  tft.setRotation(2);

  setupJoystick();
}

void
showList(int offset, int cursor) {
  Serial.print("offset/cursor: ");
  Serial.print(offset);
  Serial.print(" ");
  Serial.println(cursor);

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);

  for (int i = 0; offset + i < LIST_SIZE && i < SCREEN_SIZE; i++) {
    if (i == cursor) {
      tft.setTextColor(TFT_BLACK, TFT_WHITE);
    }
    else {
      tft.setTextColor(TFT_WHITE, TFT_BLACK);  
    }
    tft.drawString(entries[offset + i].name, 20, 20 * i);
  }
}

int
checkJoystick() {
  if (digitalRead(WIO_5S_LEFT) == LOW) {
    return UP;
  }
  else 
  if (digitalRead(WIO_5S_RIGHT) == LOW) {
    return DOWN;
  }
  else 
  if (digitalRead(WIO_5S_PRESS) == LOW) {
    return SELECT;
  }
  return 0;
}

void
loop() {
  int cmd = 0;
  showList(offset, cursor);

  delay(200);
  while (!(cmd = checkJoystick())) {
    delay(100);
  }
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
      if (cursor < SCREEN_SIZE - 1) {
        cursor++;
      }
      else
      if (offset < LIST_SIZE - SCREEN_SIZE) {
        offset++;
      }
      break;

    case SELECT:
      Keyboard.print(entries[offset + cursor].passwd);
      Serial.println(entries[offset + cursor].name);
      break;
  }
}
