struct Button {
  int pin;
  bool state;
  bool last_state;
  bool last_debounce_state;
  unsigned long last_debounce_ms;
};

Button button_init(int pin, bool set_pullup = true) {
  Button b = {};
  b.pin = pin;
  if (set_pullup) pinMode(pin, INPUT_PULLUP);

  return b;
}

void button_update(Button* b, int debounce_ms = 50) {
  unsigned long now = millis();
  int state = digitalRead(b->pin);
  if (state != b->last_debounce_state) {
    b->last_debounce_ms = now;
  }
  if (now > b->last_debounce_ms + debounce_ms) {
    b->last_state = b->state;
    b->state = state;
  }
  b->last_debounce_state = state;
}

bool button_pressed(Button b) {
  return b.last_state == 1 && b.state == 0;
}

bool button_released(Button b) {
  return b.last_state == 0 && b.state == 1;
}

bool button_is_down(Button b) {
  return b.state == 0;
}

// This function will block for *timeout_ms* amount of time, it's necessary to block
// because we need to distinguish it from a single click.
bool button_double_clicked(Button* b, int timeout_ms) {
  if (button_released(*b)) {
    unsigned long t0 = millis();
    while (true) {
      button_update(b);
      unsigned long dt = millis() - t0;
      if (dt > timeout_ms) return false;
      if (button_pressed(*b)) return true;
    }
  }
  return false;
}
