
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

  /* 启动时扫描一次RS485总线, 确认所有传感器地址 */
  scanModbusDevices();

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

    Serial.println("======== 传感器数据 ========");

    /* 1# 风速 */
    float ws = readWindSpeed();
    if (ws >= 0) Serial.printf("  [风速]   %.1f m/s\n", ws);
    else         Serial.println("  [风速]   读取失败");

    /* 2# 风向 (角度 + 16方位) */
    WindDirData wd;
    if (readWindDirData(&wd))
      Serial.printf("  [风向]   %.1f° (%s)\n", wd.angle, wd.name);
    else
      Serial.println("  [风向]   读取失败");

    /* 3# 空气温湿度 */
    float at = readAirTemp();
    float ah = readAirHumidity();
    if (at > -900 && ah >= 0)
      Serial.printf("  [空气]   温度: %.1f°C  湿度: %.1f%%RH\n", at, ah);
    else
      Serial.println("  [空气]   读取失败");

    /* 4# 光照 */
    float lux = readLight();
    if (lux >= 0) Serial.printf("  [光照]   %.0f lux\n", lux);
    else          Serial.println("  [光照]   读取失败");

    /* 5# 土壤 */
    SoilData soil;
    if (readSoilData(&soil))
      Serial.printf("  [土壤]   湿度: %.1f%%  温度: %.1f°C  EC: %.0f μS/cm\n",
                    soil.moisture, soil.temp, soil.ec);
    else
      Serial.println("  [土壤]   读取失败");

    /* 6# CO2 */
    int co2 = readCO2();
    if (co2 >= 0) Serial.printf("  [CO2]    %d ppm\n", co2);
    else          Serial.println("  [CO2]    读取失败");

    /* 7# 氧气 */
    float o2 = readOxygen();
    if (o2 >= 0) Serial.printf("  [O2]     %.1f %%\n", o2);
    else         Serial.println("  [O2]     读取失败");

    Serial.println("============================\n");
  }
}
