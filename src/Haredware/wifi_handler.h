#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>

/**
 * @file wifi_handler.h
 * @brief WiFi 连接管理模块
 */

/* ── WiFi 配置 ── */
#define WIFI_SSID "HUAWEI Q2 Por"

/**
 * @brief 连接 WiFi 并显示信息到 OLED
 * @return true=连接成功, false=连接失败
 */
bool setupWiFi(void);

#endif /* WIFI_HANDLER_H */
