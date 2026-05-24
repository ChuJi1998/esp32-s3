#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define WIDTH 128
#define HEIGHT 64
#define SCL_PIN 11
#define SDA_PIN 12

Adafruit_SSD1306 display(WIDTH, HEIGHT, &Wire, -1);
//初始化显示屏对象，参数分别为宽度、高度、I2C对象和复位引脚（-1表示不使用复位引脚）
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    while(1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,10);
  display.println("Hello ESP32");
  display.display();
}

void loop(){}