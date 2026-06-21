#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>
#include <HardwareSerial.h>

/**
 * @file relay.h
 * @brief RS485继电器控制模块头文件（12路继电器）
 * @details 提供1~12号继电器的开启、关闭、频闪等控制函数
 *
 * 协议数据帧格式:
 * [起始符4B] [帧长度1B] [板地址1B] [指令字1B] [数据域nB] [校验1B]
 * 起始符: 0x1A 0x2B 0x3C 0x4D
 * 指令字: 0x91(开) 0x96(关) 0x90(闪)
 * 校验: 从起始符到数据域末逐字节异或(XOR)
 */

/* 外部引用全局RS485串口对象（由main.cpp定义） */
extern HardwareSerial RS485_SER;  //extern声明，表示该对象在其他文件中定义

/* ── Relay 1 ── */
void Relay1_on(void);
void Relay1_off(void);
void Relay1_blink(void);

/* ── Relay 2 ── */
void Relay2_on(void);
void Relay2_off(void);
void Relay2_blink(void);

/* ── Relay 3 ── */
void Relay3_on(void);
void Relay3_off(void);
void Relay3_blink(void);

/* ── Relay 4 ── */
void Relay4_on(void);
void Relay4_off(void);
void Relay4_blink(void);

/* ── Relay 5 ── */
void Relay5_on(void);
void Relay5_off(void);
void Relay5_blink(void);

/* ── Relay 6 ── */
void Relay6_on(void);
void Relay6_off(void);
void Relay6_blink(void);

/* ── Relay 7 ── */
void Relay7_on(void);
void Relay7_off(void);
void Relay7_blink(void);

/* ── Relay 8 ── */
void Relay8_on(void);
void Relay8_off(void);
void Relay8_blink(void);

/* ── Relay 9 ── */
void Relay9_on(void);
void Relay9_off(void);
void Relay9_blink(void);

/* ── Relay 10 ── */
void Relay10_on(void);
void Relay10_off(void);
void Relay10_blink(void);

/* ── Relay 11 ── */
void Relay11_on(void);
void Relay11_off(void);
void Relay11_blink(void);

/* ── Relay 12 ── */
void Relay12_on(void);
void Relay12_off(void);
void Relay12_blink(void);

void Relay_demo(void);
#endif /* RELAY_H */
