#ifndef ESP32
#error Only works on ESP32 boards
#endif

// pwm resolution, https://docs.espressif.com/projects/arduino-esp32/en/latest/api/ledc.html#ledcattach
#if CONFIG_IDF_TARGET_ESP32
#define PWM_RESOLUTION_BITS 20
#else // other esp32 boards, such as esp32-c3, esp32-s3, etc.
#define PWM_RESOLUTION_BITS 14
#endif
#define PWM_DUTY_VALUE_MAX ((1 << PWM_RESOLUTION_BITS) - 1)

#define SG90_PWM_FREQUENCY 50  // sg90 servo has pwm frequency of 50Hz
#define SG90_PWM_FREQUENCY_US 20000

struct SG90_Servo {
  int pin;
  uint32_t min_pulse_width_us;
  uint32_t max_pulse_width_us;
};

// Common values of min and max pulse width for sg90 servos are:
//   500us  - 2500us
//   1000us - 2000us
// other values are possible and should be tested
SG90_Servo servo_init(int pin, uint32_t min_pulse_width_us = 500, uint32_t max_pulse_width_us = 2500) {
  SG90_Servo s = {
    .pin = pin,
    .min_pulse_width_us = min_pulse_width_us,
    .max_pulse_width_us = max_pulse_width_us,
  };

  if (!ledcAttach(pin, SG90_PWM_FREQUENCY, PWM_RESOLUTION_BITS)) {
    Serial.println("Error: Failed to init ledc");
  }
  return s;
}

void servo_write_pulse_width_us(SG90_Servo s, uint32_t pulse_width_us) {
  if (pulse_width_us < s.min_pulse_width_us) pulse_width_us = s.min_pulse_width_us;
  if (pulse_width_us > s.max_pulse_width_us) pulse_width_us = s.max_pulse_width_us;

  uint32_t duty = PWM_DUTY_VALUE_MAX * ((float)pulse_width_us / SG90_PWM_FREQUENCY_US);
  if (!ledcWrite(s.pin, duty)) {
    Serial.println("Error: Ledc write failed");
  }
}

void servo_write(SG90_Servo s, float angle) {
  servo_write_pulse_width_us(s, s.min_pulse_width_us + (angle / 180.0f) * (s.max_pulse_width_us - s.min_pulse_width_us));
}
