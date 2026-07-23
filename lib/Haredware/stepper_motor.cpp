#include "stepper_motor.h"

/**
 * @brief 初始化步进电机控制引脚
 */
void setup_StepperMotor(void)
{
  pinMode(MOTOR_ENA_PIN, OUTPUT);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  pinMode(MOTOR_PUL_PIN, OUTPUT);

  /* 初始状态: 禁止电机, 正向, 脉冲低 */
  digitalWrite(MOTOR_ENA_PIN, HIGH);   /* HIGH=禁止 */
  digitalWrite(MOTOR_DIR_PIN, HIGH);   /* 正向 */
  digitalWrite(MOTOR_PUL_PIN, LOW);

  Serial.println("[MOTOR] 步进电机初始化完成 (ENA=42, DIR=41, PUL=40)");
}

/**
 * @brief 使能电机 (ENA=LOW)
 */
void motor_enable(void)
{
  digitalWrite(MOTOR_ENA_PIN, LOW);
}

/**
 * @brief 禁止电机 (ENA=HIGH)
 */
void motor_disable(void)
{
  digitalWrite(MOTOR_ENA_PIN, HIGH);
}

/**
 * @brief 设置电机方向
 * @param forward true=正向, false=反向
 */
void motor_setDirection(bool forward)
{
  digitalWrite(MOTOR_DIR_PIN, forward ? HIGH : LOW);
}

/**
 * @brief 步进电机走指定步数 (阻塞式, 指定速度)
 * @param steps      步数 (>0)
 * @param delayUs    每步脉冲间隔(微秒)
 * @param forward    方向
 */
void motor_step(uint32_t steps, uint32_t delayUs, bool forward)
{
  if (steps == 0) return;

  motor_setDirection(forward);
  motor_enable();

  /* 方向建立时间 (驱动器要求至少 5μs) */
  delayMicroseconds(10);

  for (uint32_t i = 0; i < steps; i++) {
    digitalWrite(MOTOR_PUL_PIN, HIGH);
    delayMicroseconds(delayUs);
    digitalWrite(MOTOR_PUL_PIN, LOW);
    delayMicroseconds(delayUs);
  }

  /* 走完后可选择保持使能或禁止, 这里保持使能 */
}

/**
 * @brief 步进电机走指定步数 (默认速度)
 * @param steps   步数 (>0)
 * @param forward 方向
 */
void motor_step(uint32_t steps, bool forward)
{
  motor_step(steps, MOTOR_DEFAULT_PULSE_US, forward);
}

/**
 * @brief 步进电机自检: 正转一圈再反转一圈
 * @param stepsPerRev 每圈步数 (取决于驱动器细分配置)
 *        200  = 整步 (1.8°/步)
 *        400  = 1/2 细分
 *        800  = 1/4 细分
 *       1600  = 1/8 细分
 *       3200  = 1/16 细分
 */
void motor_test(uint32_t stepsPerRev)
{
  Serial.println("[MOTOR] 开始自检...");

  Serial.printf("[MOTOR] 正转 %u 步 (1圈)...\n", stepsPerRev);
  motor_step(stepsPerRev, MOTOR_DEFAULT_PULSE_US, true);
  delay(500);

  Serial.printf("[MOTOR] 反转 %u 步 (1圈)...\n", stepsPerRev);
  motor_step(stepsPerRev, MOTOR_DEFAULT_PULSE_US, false);
  delay(500);

  motor_disable();
  Serial.println("[MOTOR] 自检完成, 电机已禁止");
}
