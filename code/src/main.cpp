#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"

#define MOTOR_LIN_B2 16
#define MOTOR_HIN_B2 17
#define MOTOR_LIN_A2 18
#define MOTOR_HIN_A2 19
#define MOTOR_PWM_HIGH 1000u - 10u
#define MOTOR_PWM_LOW 0u

static pwm_config
motor_get_default_pwm_config()
{
  pwm_config c = {0, 0, 0};
  pwm_config_set_phase_correct(&c, false);
  pwm_config_set_clkdiv_mode(&c, PWM_DIV_FREE_RUNNING);
  pwm_config_set_output_polarity(&c, false, false);
  // PWM increments at 1MHZ, running from 0 to 999, inclusive 100 times
  pwm_config_set_clkdiv_int(&c, clock_get_hz(clk_sys) / 100000u);
  pwm_config_set_wrap(&c, 1000u);
  return c;
}

static pwm_config motor_pwm_config = motor_get_default_pwm_config();

static void
motor_brake()
{
  pwm_set_gpio_level(MOTOR_HIN_A2, MOTOR_PWM_LOW);
  pwm_set_gpio_level(MOTOR_HIN_B2, MOTOR_PWM_LOW);
  sleep_us(40);
  gpio_put(MOTOR_LIN_B2, 1);
  gpio_put(MOTOR_LIN_A2, 1);
}

static void
motor_forward()
{
  pwm_set_gpio_level(MOTOR_HIN_A2, MOTOR_PWM_LOW);
  gpio_put(MOTOR_LIN_B2, 0);
  sleep_us(40);

  pwm_set_gpio_level(MOTOR_HIN_B2, MOTOR_PWM_HIGH);
  gpio_put(MOTOR_LIN_A2, 1);
}

static void
motor_backward()
{
  pwm_set_gpio_level(MOTOR_HIN_B2, MOTOR_PWM_LOW);
  gpio_put(MOTOR_LIN_A2, 0);
  sleep_us(40);
  pwm_set_gpio_level(MOTOR_HIN_A2, MOTOR_PWM_HIGH);
  gpio_put(MOTOR_LIN_B2, 1);
}


static void
motor_init()
{

  gpio_init(MOTOR_LIN_B2);
  gpio_init(MOTOR_LIN_A2);
  gpio_set_dir(MOTOR_LIN_B2, GPIO_OUT);
  gpio_set_dir(MOTOR_LIN_A2, GPIO_OUT);
  gpio_put(MOTOR_LIN_B2, 0);
  gpio_put(MOTOR_LIN_A2, 0);

  gpio_set_function(MOTOR_HIN_A2, GPIO_FUNC_PWM);
  gpio_set_function(MOTOR_HIN_B2, GPIO_FUNC_PWM);
  pwm_init(pwm_gpio_to_slice_num(MOTOR_HIN_A2), &motor_pwm_config, true);
  pwm_init(pwm_gpio_to_slice_num(MOTOR_HIN_B2), &motor_pwm_config, true);

  sleep_us(40);
}


int
main()
{
  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  motor_init();

  while (true)
  {
    gpio_put(LED_PIN, 1);
    motor_forward();
    sleep_ms(5000);


    //    gpio_put(LED_PIN, 0);
    //    motor_backward();
    //    sleep_ms(5000);
  }
}
