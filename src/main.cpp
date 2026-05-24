/*
ESP32 端：串口接收测试代码
功能：接收串口数据并打印到调试串口
*/

#include <Arduino.h>

#define UART_RX_PIN 16   // 连接 K230 TX (GPIO32)
#define UART_TX_PIN 17   // 连接 K230 RX (GPIO33)

void setup() {
  Serial.begin(115200);                      // 电脑调试串口
  Serial2.begin(115200, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
  
  Serial.println("ESP32 已启动，等待 K230 数据...");
  Serial.println("接线：GPIO16 ← K230_TX, GPIO17 → K230_RX");
}

void loop() {
  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n');
    data.trim();
    Serial.print("收到: ");
    Serial.println(data);
  }
  
  delay(10);
}