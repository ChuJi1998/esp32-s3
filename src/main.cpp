
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SSD1306.h"
#include "HardwareSerial.h"

#include "Haredware/rs485.h"
#include "Haredware/relay.h"
#include "Haredware/oled.h"

void setup(void)
{
  setup_RS485();

  setup_OLED();

}

void loop(void)
{
 oled_showText("Hello, World!");
}
