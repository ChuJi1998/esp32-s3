#include "sensor.h"

/* ── 传感器 RS485 串口对象 (UART2) ── */
HardwareSerial SENSOR_SER(2);

/**
 * @brief Modbus RTU CRC16 校验计算
 * @param buf  数据缓冲区
 * @param len  数据长度(字节)
 * @return 16位CRC值
 */
static uint16_t modbusCRC16(const uint8_t *buf, uint8_t len)
{
  uint16_t crc = 0xFFFF;
  for (uint8_t pos = 0; pos < len; pos++) {
    crc ^= (uint16_t)buf[pos];
    for (uint8_t i = 8; i != 0; i--) {
      if ((crc & 0x0001) != 0) {
        crc >>= 1;
        crc ^= 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

/**
 * @brief 初始化传感器 RS485 (UART2: 9600bps, TX=GPIO4, RX=GPIO5)
 */
void setup_SensorRS485(void)
{
  SENSOR_SER.begin(9600, SERIAL_8N1, 5, 4);  /* RX=GPIO5, TX=GPIO4 */

  /* 如果使用方向控制引脚, 配置为输出 */
  #if (RS485_DE_RE_PIN >= 0)
    pinMode(RS485_DE_RE_PIN, OUTPUT);
    digitalWrite(RS485_DE_RE_PIN, LOW);
  #endif

  Serial.println("[SENSOR] RS485传感器模块初始化完成 (UART2, 9600bps, TX=4, RX=5)");
}

/**
 * @brief 设置RS485收发方向
 * @param txMode true=发送模式, false=接收模式
 */
static void rs485_setDirection(bool txMode)
{
  #if (RS485_DE_RE_PIN >= 0)
    digitalWrite(RS485_DE_RE_PIN, txMode ? HIGH : LOW);
  #endif
  /* 自动收发模块无需操作, 但给一个微小的延时让硬件稳定 */
  delayMicroseconds(50);
}

/**
 * @brief Modbus RTU 读取保持寄存器 (功能码 0x03)
 * @param slaveAddr  从机地址
 * @param regAddr    寄存器起始地址
 * @param value      输出参数, 读取到的16位寄存器值
 * @return true=成功, false=失败
 */
bool modbusReadHoldingRegister(uint8_t slaveAddr, uint16_t regAddr, uint16_t *value)
{
  uint8_t txBuf[8];
  uint8_t rxBuf[7];
  uint16_t crc;

  /* ── 构建 Modbus RTU 请求帧 ── */
  txBuf[0] = slaveAddr;             /* 从机地址 */
  txBuf[1] = 0x03;                  /* 功能码: 读保持寄存器 */
  txBuf[2] = (regAddr >> 8) & 0xFF; /* 寄存器地址高字节 */
  txBuf[3] = regAddr & 0xFF;        /* 寄存器地址低字节 */
  txBuf[4] = 0x00;                  /* 寄存器数量高字节 */
  txBuf[5] = 0x01;                  /* 寄存器数量低字节 (读取1个寄存器) */

  /* 计算并追加 CRC16 */
  crc = modbusCRC16(txBuf, 6);
  txBuf[6] = crc & 0xFF;            /* CRC低字节 */
  txBuf[7] = (crc >> 8) & 0xFF;     /* CRC高字节 */

  /* ── 清空接收缓冲区 ── */
  while (SENSOR_SER.available()) {
    SENSOR_SER.read();
  }

  /* ── 切换到发送模式并发送请求 ── */
  rs485_setDirection(true);
  SENSOR_SER.write(txBuf, 8);
  SENSOR_SER.flush();
  rs485_setDirection(false);

  /* ── 等待并接收响应 (预期7字节) ── */
  unsigned long startTime = millis();
  uint8_t rxIndex = 0;

  while (rxIndex < 7) {
    if (SENSOR_SER.available()) {
      rxBuf[rxIndex++] = SENSOR_SER.read();
      startTime = millis();  /* 重置超时计时器 */
    }
    if (millis() - startTime > MODBUS_TIMEOUT_MS) {
      Serial.printf("[SENSOR] Modbus 读取超时 (addr=0x%02X, reg=0x%04X)\n", slaveAddr, regAddr);
      return false;
    }
  }

  /* ── 校验响应帧 ── */
  /* 检查从机地址和功能码 */
  if (rxBuf[0] != slaveAddr) {
    Serial.printf("[SENSOR] 从机地址不匹配: 期望0x%02X, 收到0x%02X\n", slaveAddr, rxBuf[0]);
    return false;
  }
  if (rxBuf[1] != 0x03) {
    Serial.printf("[SENSOR] 功能码不匹配: 期望0x03, 收到0x%02X\n", rxBuf[1]);
    return false;
  }
  if (rxBuf[2] != 0x02) {
    Serial.printf("[SENSOR] 数据字节数异常: 期望2, 收到%d\n", rxBuf[2]);
    return false;
  }

  /* 校验 CRC */
  uint16_t rxCRC = rxBuf[5] | ((uint16_t)rxBuf[6] << 8);
  uint16_t calcCRC = modbusCRC16(rxBuf, 5);
  if (rxCRC != calcCRC) {
    Serial.printf("[SENSOR] CRC校验失败: 计算0x%04X, 收到0x%04X\n", calcCRC, rxCRC);
    return false;
  }

  /* ── 提取寄存器值 ── */
  *value = ((uint16_t)rxBuf[3] << 8) | rxBuf[4];
  return true;
}

/**
 * @brief 读取CO2浓度
 * @return CO2浓度(ppm), 读取失败返回 -1
 */
int readCO2(void)
{
  uint16_t rawValue;
  if (modbusReadHoldingRegister(CO2_SLAVE_ADDR, CO2_REG_ADDR, &rawValue)) {
    return (int)rawValue;
  }
  return -1;
}

/**
 * @brief 读取氧气浓度
 * @return 氧气浓度(%), 读取失败返回 -1.0
 *
 * @note 传感器参数:
 *       设备地址: 0x07, 寄存器: 0x000A, UINT16
 *       量程: 0-25% O₂
 *       转换公式: 原始值 ÷ 10 = 氧气浓度(%)
 *       典型值: 211 → 21.1% O₂ (正常空气)
 */
float readOxygen(void)
{
  uint16_t rawValue;
  if (modbusReadHoldingRegister(O2_SLAVE_ADDR, O2_REG_ADDR, &rawValue)) {
    /* 原始值 ÷ 10 = 氧气浓度(%) */
    return (float)rawValue / 10.0f;
  }
  return -1.0f;
}
