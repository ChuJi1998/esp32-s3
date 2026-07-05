#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <WebServer.h>
#include "relay.h"
#include "sensor.h"

/**
 * @file web_server.h
 * @brief Web 服务器模块（传感器仪表盘 + 12路继电器控制）
 */

/* 外部全局 WebServer 对象 */
extern WebServer server;

void setupWebServer(void);
void webServerLoop(void);

#endif /* WEB_SERVER_H */
