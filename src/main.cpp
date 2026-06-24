
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SSD1306.h"
#include "HardwareSerial.h"

#include "relay.h"
#include "oled.h"
#include "wifi_handler.h"
#include "web_server.h"
#include "sensor.h"

/* ── 传感器数据刷新间隔(ms) ── */
#define SENSOR_READ_INTERVAL 2000

static unsigned long lastSensorRead = 0;

void setup(void)
{
  Serial.begin(115200);
  delay(500);  /* 等待串口稳定 */

  Serial.println("\n========================================");
  Serial.println("  ESP32-S3 水质监测控制系统启动中...");
  Serial.println("========================================\n");

  setup_SensorRS485();  /* 传感器RS485: UART2, GPIO4(TX)/GPIO5(RX) */
  setup_RS485();        /* 继电器RS485: UART1, GPIO16(TX)/GPIO17(RX) */
  setup_OLED();
  setupWiFi();
  setupWebServer();

  Serial.println("\n[MAIN] 系统初始化完成");
  Serial.println("========================================\n");
}

void loop(void)
{
  webServerLoop();

  /* ── 定时读取传感器数据 ── */
  if (millis() - lastSensorRead >= SENSOR_READ_INTERVAL) {
    lastSensorRead = millis();

    /* 读取 CO2 浓度 */
    int co2 = readCO2();
    if (co2 >= 0) {
      Serial.printf("[CO2]  浓度: %d ppm\n", co2);
    } else {
      Serial.println("[CO2]  读取失败, 请检查传感器连接");
    }

    /* 读取氧气浓度 */
    float o2 = readOxygen();
    if (o2 >= 0) {
      Serial.printf("[O2]   浓度: %.1f %%\n", o2);
    } else {
      Serial.println("[O2]   读取失败, 请检查传感器连接");
    }

    Serial.println("----------------------------------------");
  }
}
