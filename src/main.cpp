
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SSD1306.h"
#include "HardwareSerial.h"

#include "Haredware/rs485.h"
#include "Haredware/relay.h"
#include "Haredware/oled.h"

#define DEMO_DELAY 10000

void setup(void)
{
  setup_RS485();

  setup_OLED();

}

void loop(void)
{

  oled_showText("Relay5 ON");
  Relay5_on();
  delay(DEMO_DELAY);

  oled_showText("Relay5 OFF");
  Relay5_off();
  delay(DEMO_DELAY);

  oled_showText("Relay5 BLINK");
  Relay5_blink();
  delay(DEMO_DELAY);

  oled_showText("Relay5 Stop Blink");
  Relay5_off();
  delay(DEMO_DELAY);

}
