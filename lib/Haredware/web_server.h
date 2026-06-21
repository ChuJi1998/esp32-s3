#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <WebServer.h>
#include "relay.h"

/**
 * @file web_server.h
 * @brief Web 服务器模块（12路继电器控制）
 */

/* 外部全局 WebServer 对象 */
extern WebServer server;

/**
 * @brief 初始化 Web 服务器路由并启动
 */
void setupWebServer(void);

/**
 * @brief 处理 HTTP 请求（须在 loop() 中调用）
 */
void webServerLoop(void);

#endif /* WEB_SERVER_H */
