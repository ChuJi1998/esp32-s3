#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <HardwareSerial.h>

/**
 * @file sensor.h
 * @brief Modbus RTU 传感器采集模块（氧气 + CO2）
 * @details 通过 TTL转RS485 模块 (UART2: TX=GPIO4, RX=GPIO5, 9600bps)
 *          读取氧气传感器和CO2传感器数据
 *
 * CO2传感器 Modbus 参数:
 *   从机地址: 0x06
 *   波特率:   9600
 *   寄存器:   0x0010 (保持寄存器)
 *   数据格式: 16位无符号整数, 单位 ppm
 *
 * 氧气传感器 Modbus 参数:
 *   从机地址: 0x07 (设备地址=7)
 *   寄存器:   0x000A (氧气浓度, UINT16)
 *   量程:     0-25% O₂
 *   转换公式: 原始值 ÷ 10 = 氧气浓度(%)
 *   典型值:   211(原始值) → 21.1% O₂
 */

/* ── 传感器 Modbus 地址配置 ── */
#define CO2_SLAVE_ADDR      0x06    /* CO2传感器从机地址 */
#define CO2_REG_ADDR        0x0010  /* CO2浓度寄存器地址 */

#define O2_SLAVE_ADDR       0x07    /* 氧气传感器从机地址 (设备地址=7) */
#define O2_REG_ADDR         0x000A  /* 氧气浓度寄存器地址 (0x000A) */

/* ── RS485 方向控制引脚(若模块不支持自动收发, 请设置实际引脚) ── */
#define RS485_DE_RE_PIN     -1      /* -1 表示不使用方向控制(自动收发模块) */

/* ── Modbus 读取超时(ms) ── */
#define MODBUS_TIMEOUT_MS   200

/* ── 外部全局传感器RS485串口对象 ── */
extern HardwareSerial SENSOR_SER;

/* ════════════════════════════════════════════
   初始化函数
   ════════════════════════════════════════════ */

/**
 * @brief 初始化传感器RS485通信 (UART2: 9600, TX=GPIO4, RX=GPIO5)
 */
void setup_SensorRS485(void);

/* ════════════════════════════════════════════
   Modbus RTU 读取函数
   ════════════════════════════════════════════ */

/**
 * @brief 读取CO2浓度
 * @return CO2浓度(ppm), 读取失败返回 -1
 */
int readCO2(void);

/**
 * @brief 读取氧气浓度
 * @return 氧气浓度(单位取决于传感器, 通常为 % 或 ppm), 读取失败返回 -1
 */
float readOxygen(void);

/**
 * @brief Modbus RTU 读取保持寄存器 (功能码 0x03)
 * @param slaveAddr  从机地址
 * @param regAddr    寄存器起始地址
 * @param value      输出参数, 读取到的16位寄存器值
 * @return true=成功, false=失败
 */
bool modbusReadHoldingRegister(uint8_t slaveAddr, uint16_t regAddr, uint16_t *value);

#endif /* SENSOR_H */
