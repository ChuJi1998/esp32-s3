#include "relay.h"

#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SSD1306.h"
#include "HardwareSerial.h"

#include "oled.h"

/* ── RS485 串口对象定义（UART1） ── */
HardwareSerial RS485_SER(1);

/**
 * @brief 初始化 RS485（UART1: 9600bps, RX=GPIO17, TX=GPIO16）
 */
void setup_RS485(void)
{
  RS485_SER.begin(9600, SERIAL_8N1, 17, 16);
}

/* ── 数据帧常量 ── */
#define STX1 0x1A
#define STX2 0x2B
#define STX3 0x3C
#define STX4 0x4D
#define BOARD_ADDR 0x00
#define CMD_ON     0x91
#define CMD_OFF    0x96
#define CMD_BLINK  0x90
#define BLINK_TIME 0x0A  /* 频闪间隔: 10 × 100ms = 1秒 */

/**
 * @brief 通用继电器命令发送函数
 * @param cmd  指令字 (0x91开 / 0x96关 / 0x90闪)
 * @param ch   继电器通道号 (1~12)
 * @return void
 */
static void relay_send_cmd(uint8_t cmd, uint8_t ch)
{
  uint8_t buf[11];   /* 最大11字节 */
  uint8_t len;
  uint8_t checksum;

  /* 起始符 */
  buf[0] = STX1;
  buf[1] = STX2;
  buf[2] = STX3;
  buf[3] = STX4;

  if (cmd == CMD_BLINK) {
    len = 11;
    buf[4] = 0x0B;        /* 帧长度: 11 */
    buf[5] = BOARD_ADDR;  /* 板地址 */
    buf[6] = CMD_BLINK;   /* 指令字: 频闪 */
    buf[7] = ch;          /* 继电器号 */
    buf[8] = 0x00;        /* 频闪时间高字节 */
    buf[9] = BLINK_TIME;  /* 频闪时间低字节 */
  } else {
    len = 9;
    buf[4] = 0x09;        /* 帧长度: 9 */
    buf[5] = BOARD_ADDR;  /* 板地址 */
    buf[6] = cmd;         /* 指令字: 开/关 */
    buf[7] = ch;          /* 继电器号 */
  }

  /* 计算校验和 (从起始符到数据域末尾逐字节XOR) */
  checksum = 0;
  for (uint8_t i = 0; i < len - 1; i++) {
    checksum ^= buf[i];
  }
  buf[len - 1] = checksum;

  /* 通过RS485发送命令 */
  RS485_SER.write(buf, len);
}

/* ════════════════════════════════════════════
   Relay 1
   ════════════════════════════════════════════ */
void Relay1_on(void)    { relay_send_cmd(CMD_ON,    0x01); }
void Relay1_off(void)   { relay_send_cmd(CMD_OFF,   0x01); }
void Relay1_blink(void) { relay_send_cmd(CMD_BLINK, 0x01); }

/* ════════════════════════════════════════════
   Relay 2
   ════════════════════════════════════════════ */
void Relay2_on(void)    { relay_send_cmd(CMD_ON,    0x02); }
void Relay2_off(void)   { relay_send_cmd(CMD_OFF,   0x02); }
void Relay2_blink(void) { relay_send_cmd(CMD_BLINK, 0x02); }

/* ════════════════════════════════════════════
   Relay 3
   ════════════════════════════════════════════ */
void Relay3_on(void)    { relay_send_cmd(CMD_ON,    0x03); }
void Relay3_off(void)   { relay_send_cmd(CMD_OFF,   0x03); }
void Relay3_blink(void) { relay_send_cmd(CMD_BLINK, 0x03); }

/* ════════════════════════════════════════════
   Relay 4
   ════════════════════════════════════════════ */
void Relay4_on(void)    { relay_send_cmd(CMD_ON,    0x04); }
void Relay4_off(void)   { relay_send_cmd(CMD_OFF,   0x04); }
void Relay4_blink(void) { relay_send_cmd(CMD_BLINK, 0x04); }

/* ════════════════════════════════════════════
   Relay 5
   ════════════════════════════════════════════ */
void Relay5_on(void)    { relay_send_cmd(CMD_ON,    0x05); }
void Relay5_off(void)   { relay_send_cmd(CMD_OFF,   0x05); }
void Relay5_blink(void) { relay_send_cmd(CMD_BLINK, 0x05); }

/* ════════════════════════════════════════════
   Relay 6
   ════════════════════════════════════════════ */
void Relay6_on(void)    { relay_send_cmd(CMD_ON,    0x06); }
void Relay6_off(void)   { relay_send_cmd(CMD_OFF,   0x06); }
void Relay6_blink(void) { relay_send_cmd(CMD_BLINK, 0x06); }

/* ════════════════════════════════════════════
   Relay 7
   ════════════════════════════════════════════ */
void Relay7_on(void)    { relay_send_cmd(CMD_ON,    0x07); }
void Relay7_off(void)   { relay_send_cmd(CMD_OFF,   0x07); }
void Relay7_blink(void) { relay_send_cmd(CMD_BLINK, 0x07); }

/* ════════════════════════════════════════════
   Relay 8
   ════════════════════════════════════════════ */
void Relay8_on(void)    { relay_send_cmd(CMD_ON,    0x08); }
void Relay8_off(void)   { relay_send_cmd(CMD_OFF,   0x08); }
void Relay8_blink(void) { relay_send_cmd(CMD_BLINK, 0x08); }

/* ════════════════════════════════════════════
   Relay 9
   ════════════════════════════════════════════ */
void Relay9_on(void)    { relay_send_cmd(CMD_ON,    0x09); }
void Relay9_off(void)   { relay_send_cmd(CMD_OFF,   0x09); }
void Relay9_blink(void) { relay_send_cmd(CMD_BLINK, 0x09); }

/* ════════════════════════════════════════════
   Relay 10
   ════════════════════════════════════════════ */
void Relay10_on(void)    { relay_send_cmd(CMD_ON,    0x0A); }
void Relay10_off(void)   { relay_send_cmd(CMD_OFF,   0x0A); }
void Relay10_blink(void) { relay_send_cmd(CMD_BLINK, 0x0A); }

/* ════════════════════════════════════════════
   Relay 11
   ════════════════════════════════════════════ */
void Relay11_on(void)    { relay_send_cmd(CMD_ON,    0x0B); }
void Relay11_off(void)   { relay_send_cmd(CMD_OFF,   0x0B); }
void Relay11_blink(void) { relay_send_cmd(CMD_BLINK, 0x0B); }

/* ════════════════════════════════════════════
   Relay 12
   ════════════════════════════════════════════ */
void Relay12_on(void)    { relay_send_cmd(CMD_ON,    0x0C); }
void Relay12_off(void)   { relay_send_cmd(CMD_OFF,   0x0C); }
void Relay12_blink(void) { relay_send_cmd(CMD_BLINK, 0x0C); }


#define DEMO_DELAY 1000     /* 每路继电器之间的间隔(ms) */
#define RELAY_ON_TIME 100    /* 继电器吸合保持时间(ms) */

void Relay_demo(void)
{ 

  oled_showText("Relay1 ON");
  Relay1_on();
  delay(RELAY_ON_TIME);
  Relay1_off();
  delay(DEMO_DELAY);


  oled_showText("Relay2 ON");
  Relay2_on();
  delay(RELAY_ON_TIME);
  Relay2_off();
  delay(DEMO_DELAY);

  oled_showText("Relay3 ON");
  Relay3_on();
  delay(RELAY_ON_TIME);
  Relay3_off();
  delay(DEMO_DELAY);

  oled_showText("Relay4 ON");
  Relay4_on();
  delay(RELAY_ON_TIME);
  Relay4_off();
  delay(DEMO_DELAY);

  oled_showText("Relay5 ON");
  Relay5_on();
  delay(RELAY_ON_TIME);
  Relay5_off();
  delay(DEMO_DELAY);

  oled_showText("Relay6 ON");
  Relay6_on();
  delay(RELAY_ON_TIME);
  Relay6_off();
  delay(DEMO_DELAY);

  oled_showText("Relay7 ON");
  Relay7_on();
  delay(RELAY_ON_TIME);
  Relay7_off();
  delay(DEMO_DELAY);

  oled_showText("Relay8 ON");
  Relay8_on();
  delay(RELAY_ON_TIME);
  Relay8_off();
  delay(DEMO_DELAY);

  oled_showText("Relay9 ON");
  Relay9_on();
  delay(RELAY_ON_TIME);
  Relay9_off();
  delay(DEMO_DELAY);

  oled_showText("Relay10 ON");
  Relay10_on();
  delay(RELAY_ON_TIME);
  Relay10_off();
  delay(DEMO_DELAY);

  oled_showText("Relay11 ON");
  Relay11_on();
  delay(RELAY_ON_TIME);
  Relay11_off();
  delay(DEMO_DELAY);

  oled_showText("Relay12 ON");
  Relay12_on();
  delay(RELAY_ON_TIME);
  Relay12_off();
  delay(DEMO_DELAY);
}
