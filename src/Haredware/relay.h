#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>
#include <HardwareSerial.h>

/**
 * @file relay.h
 * @brief RS485继电器控制模块头文件
 * @details 提供5区1号继电器的开启、关闭、频闪等控制函数
 */

/* 外部引用全局RS485串口对象（由main.cpp定义） */
extern HardwareSerial RS485_SER;  //extern声明，表示该对象在其他文件中定义

/**
 * @brief 打开继电器
 * @details 发送继电器开启命令到RS485总线
 * @return void
 */
void Relay5_on(void);

/**
 * @brief 关闭继电器
 * @details 发送继电器关闭命令到RS485总线
 * @return void
 */
void Relay5_off(void);

/**
 * @brief 继电器频闪
 * @details 发送继电器频闪命令到RS485总线
 * @return void
 */
void Relay5_blink(void);

#endif /* RELAY_H */