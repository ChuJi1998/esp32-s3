
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SSD1306.h"
#include "HardwareSerial.h"

#include "Haredware/rs485.h"
#include "Haredware/relay.h"
#include "Haredware/oled.h"
#include "Haredware/wifi_handler.h"
#include "Haredware/web_server.h"

void setup(void)
{
  Serial.begin(115200);

  setup_RS485();
  setup_OLED();
  setupWiFi();
  setupWebServer();
}

void loop(void)
{
  webServerLoop();
}
