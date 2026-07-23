#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include <Arduino.h>

/**
 * @file stepper_motor.h
 * @brief 42步进电机控制模块 (共阴极接法)
 *
 * 接线方式 (共阴极):
 *   驱动器 ENA- / DIR- / PUL-  →  ESP32 GND
 *   驱动器 ENA+ → GPIO42
 *   驱动器 DIR+ → GPIO41
 *   驱动器 PUL+ → GPIO40
 *
 * 控制逻辑:
 *   ENA:  LOW  = 电机使能(可转)
 *         HIGH = 电机禁止(锁死)
 *   PUL:  上升沿触发一步
 *   DIR:  HIGH = 正向, LOW = 反向
 */

/* ── 步进电机引脚定义 ── */
#define MOTOR_ENA_PIN  42    /* 使能控制 (LOW=使能) */
#define MOTOR_DIR_PIN  41    /* 方向控制 */
#define MOTOR_PUL_PIN  40    /* 脉冲信号 */

/* ── 默认脉冲间隔(μs), 决定转速 ── */
#define MOTOR_DEFAULT_PULSE_US 500

/**
 * @brief 初始化步进电机引脚
 */
void setup_StepperMotor(void);

/**
 * @brief 使能电机 (ENA=LOW, 电机可转)
 */
void motor_enable(void);

/**
 * @brief 禁止电机 (ENA=HIGH, 电机锁死)
 */
void motor_disable(void);

/**
 * @brief 设置电机方向
 * @param forward true=正向, false=反向
 */
void motor_setDirection(bool forward);

/**
 * @brief 步进电机走指定步数 (阻塞式)
 * @param steps      步数 (>0)
 * @param delayUs    每步脉冲间隔(微秒), 越小越快
 * @param forward    方向, true=正向
 */
void motor_step(uint32_t steps, uint32_t delayUs, bool forward);

/**
 * @brief 步进电机走指定步数 (使用默认速度)
 * @param steps   步数 (>0)
 * @param forward 方向
 */
void motor_step(uint32_t steps, bool forward);

/**
 * @brief 步进电机测试: 正转一圈, 反转一圈
 * @param stepsPerRev 每圈步数 (如 200/400/800 取决于细分)
 */
void motor_test(uint32_t stepsPerRev);

#endif /* STEPPER_MOTOR_H */
