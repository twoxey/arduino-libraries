typedef struct {
  bool changed: 1;
  bool skipped: 1;
  uint8_t state: 4;
  int count;
} Encoder;

void encoder_init(Encoder* e, int a_pin, int b_pin);
bool encoder_update(Encoder* e, int a_state, int b_state);


//
// Usage
//

// Encoder e;
//
// void setup() {
//   Serial.begin(9600);
//   encoder_init(&e, 2, 3);
// }
//
// void loop() {
//   if (e.changed) {
//     Serial.println(e.count);
//     e.changed = false;
//   }
// }


//
// Implementation
//

bool encoder_update(Encoder* e, int a_state, int b_state) {
  // Encoder pin state when turned
  //       AB AB AB AB
  // CW:   00 10 11 01
  // CCW:  00 01 11 10

  uint8_t current_state = a_state << 1 | b_state;
  if (current_state == (e->state & 0b11)) return false;

  uint8_t state = (e->state << 2 | current_state) & 0b1111;
  if      (state == 0b0010 || state == 0b1011 || state == 0b1101 || state == 0b0100) ++e->count;
  else if (state == 0b0001 || state == 0b0111 || state == 0b1110 || state == 0b1000) --e->count;
  else e->skipped = true;
  e->state = state;
  e->changed = true;

  return true;
}

#ifndef ARDUINO_ISR_ATTR
#define ARDUINO_ISR_ATTR
#endif
template <Encoder* e, int a_pin, int b_pin>
void ARDUINO_ISR_ATTR encoder_isr() {
  encoder_update(e, digitalRead(a_pin), digitalRead(b_pin));
}

#define encoder_init(e, a, b) encoder_init_<e, a, b>()
template <Encoder* e, int a_pin, int b_pin>
void encoder_init_() {
  *e = {};
  attachInterrupt(digitalPinToInterrupt(a_pin), encoder_isr<e, a_pin, b_pin>, CHANGE);
  attachInterrupt(digitalPinToInterrupt(b_pin), encoder_isr<e, a_pin, b_pin>, CHANGE);
}
