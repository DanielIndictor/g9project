#include "hardware/gpio.h"
#include <hardware/clocks.h>
#include <hardware/pwm.h>
#include <pico/stdlib.h>

#define PWM_PIN 16


static pwm_config
get_my_pwm_config()
{
  pwm_config c = {0, 0, 0};
  pwm_config_set_phase_correct(&c, false);
  pwm_config_set_clkdiv_mode(&c, PWM_DIV_FREE_RUNNING);
  pwm_config_set_output_polarity(&c, false, false);
  // PWM increments at 1MHZ, running from 0 to 999, inclusive 100 times
  pwm_config_set_clkdiv_int(&c, clock_get_hz(clk_sys) / 1000u);
  pwm_config_set_wrap(&c, 1000u);
  return c;
}

static pwm_config my_pwm_config = get_my_pwm_config();

int
main()
{
  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
  auto slice_num = pwm_gpio_to_slice_num(PWM_PIN);
  auto channel = pwm_gpio_to_channel(PWM_PIN);
  pwm_set_wrap(slice_num, 3);
  pwm_set_chan_level(slice_num, channel, 2);
  pwm_set_clkdiv_mode(slice_num, 255);
  pwm_set_enabled(slice_num, true);
  //
  // Not sure why this no work.
  // pwm_init(pwm_gpio_to_slice_num(PWM_PIN), &my_pwm_config, true);
  // pwm_set_chan_level(pwm_gpio_to_slice_num(PWM_PIN), 
  //     pwm_gpio_to_channel(PWM_PIN), 
  //     500);
  // pwm_set_enabled(pwm_gpio_to_slice_num(PWM_PIN), true);

  while (true)
  {
    gpio_put(LED_PIN, 1);
    sleep_ms(500);
    gpio_put(LED_PIN, 0);
    sleep_ms(500);


    //    gpio_put(LED_PIN, 0);
    //    motor_backward();
    //    sleep_ms(5000);
  }
}
