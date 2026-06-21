#ifndef OLED_H
#define OLED_H


#include <Arduino.h>
#include <Wire.h>
#include "relay.h"
#include "Adafruit_SSD1306.h"
#include "HardwareSerial.h"

/* OLED显示屏对象（128x64, I2C） */
extern Adafruit_SSD1306 display;

void setup_OLED(void);
void oled_showText(const char *text);



#endif /* OLED_H */
