
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SSD1306.h"
#include "HardwareSerial.h"

#include "relay.h"
#include "oled.h"
#include "wifi_handler.h"
#include "web_server.h"

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
