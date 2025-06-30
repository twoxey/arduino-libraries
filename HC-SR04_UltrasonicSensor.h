// Library for controlling the HC-SR04 ultrasonic sensor for getting distance

void hc_sr04_init(int trig_pin, int echo_pin) {
  digitalWrite(trig_pin, LOW);
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
}

float hc_sr04_get_distance_cm(int trig_pin, int echo_pin) {
  digitalWrite(trig_pin, LOW);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  unsigned long us = pulseIn(echo_pin, HIGH);
  float cm = us * 0.017;

  return cm;
}

