#include "wifi_handler.h"
#include "oled.h"

/**
 * @brief 连接 WiFi（开放网络，无密码）
 * @return true=成功, false=超时失败
 */
bool setupWiFi(void)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID);

  Serial.print("Connecting to WiFi");
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    /* OLED 显示 IP */
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("WiFi Connected");
    display.setCursor(0, 16);
    display.println(WiFi.localIP().toString());
    display.setCursor(0, 32);
    display.println("Port: 80");
    display.setCursor(0, 48);
    display.println("Web Relay Ctrl");
    display.display();

    return true;
  } else {
    Serial.println("\nFailed to connect to WiFi");
    oled_showText("WiFi Failed");
    return false;
  }
}
