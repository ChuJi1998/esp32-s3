#include "rs485.h"
#include <Arduino.h>
#include <Wire.h>
#include "relay.h"

/* RS485串口对象定义（UART1） */
HardwareSerial RS485_SER(1);


void setup_RS485(void)
{
  /* 初始化UART1：波特率9600，RX=GPIO17，TX=GPIO16 */
  RS485_SER.begin(9600, SERIAL_8N1, 17, 16);
}
