#include <Arduino.h>
#include "stepper_motor.h"

/**
 * @brief 42步进电机测试程序 (共阴极接法, 1600脉冲/圈)
 *
 * 接线: ENA-/DIR-/PUL- → GND
 *       ENA+ → GPIO42
 *       DIR+ → GPIO41
 *       PUL+ → GPIO40
 */

#define STEPS_PER_REV  1600   /* 1/8细分: 1600脉冲/圈 */
#define TURNS            5    /* 转动圈数 */
#define TOTAL_STEPS   (STEPS_PER_REV * TURNS)  /* 8000步 */

/* ── 加减速参数 ── */
#define START_DELAY_US  1500   /* 起步脉冲间隔(μs) */
#define MIN_DELAY_US     600   /* 最快脉冲间隔(μs) */
#define ACCEL_STEPS      300   /* 加速段步数 */

/**
 * @brief 带加减速的步进电机运动 (梯形加速曲线)
 */
void motor_step_accel(uint32_t steps, bool forward)
{
  motor_setDirection(forward);
  motor_enable();
  delayMicroseconds(10);

  if (steps <= ACCEL_STEPS * 2) {
    /* 步数太少, 匀速慢走 */
    for (uint32_t i = 0; i < steps; i++) {
      digitalWrite(MOTOR_PUL_PIN, HIGH);
      delayMicroseconds(START_DELAY_US);
      digitalWrite(MOTOR_PUL_PIN, LOW);
      delayMicroseconds(START_DELAY_US);
    }
    return;
  }

  uint32_t accelEnd   = ACCEL_STEPS;
  uint32_t decelStart = steps - ACCEL_STEPS;
  uint32_t delayUs;

  for (uint32_t i = 0; i < steps; i++) {
    if (i < accelEnd) {
      /* 加速段: 从慢到快 */
      delayUs = START_DELAY_US - (START_DELAY_US - MIN_DELAY_US) * i / accelEnd;
    } else if (i < decelStart) {
      /* 匀速段: 最快 */
      delayUs = MIN_DELAY_US;
    } else {
      /* 减速段: 从快到慢 */
      delayUs = MIN_DELAY_US + (START_DELAY_US - MIN_DELAY_US) * (i - decelStart) / ACCEL_STEPS;
    }

    digitalWrite(MOTOR_PUL_PIN, HIGH);
    delayMicroseconds(delayUs);
    digitalWrite(MOTOR_PUL_PIN, LOW);
    delayMicroseconds(delayUs);
  }
}

void setup(void)
{
  Serial.begin(115200);
  delay(500);

  Serial.println("\n========================================");
  Serial.println("  42步进电机测试 (1600脉冲/圈)");
  Serial.println("========================================\n");

  setup_StepperMotor();

  Serial.printf("[TEST] 正转 %d 圈 (%d 步)...\n", TURNS, TOTAL_STEPS);
  motor_step_accel(TOTAL_STEPS, true);
  delay(2000);

  Serial.printf("[TEST] 反转 %d 圈 (%d 步)...\n", TURNS, TOTAL_STEPS);
  motor_step_accel(TOTAL_STEPS, false);
  delay(2000);

  motor_disable();
  Serial.println("[DONE] 测试完成!");
}

void loop(void)
{
  /* 正转5圈 → 停2秒 → 反转5圈 → 停2秒 */
  Serial.printf("正转 %d 圈...\n", TURNS);
  motor_step_accel(TOTAL_STEPS, true);
  delay(2000);

  Serial.printf("反转 %d 圈...\n", TURNS);
  motor_step_accel(TOTAL_STEPS, false);
  delay(2000);
}
