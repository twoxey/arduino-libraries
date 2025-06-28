//
// Interface
//

typedef struct EC11_Encoder EC11_Encoder; 
void encoder_init(EC11_Encoder* e, int pin_a, int pin_b, bool interrupt = true);
int encoder_get_offset(EC11_Encoder* e, bool reset = false);
void encoder_update(EC11_Encoder* e); // This function reads and updates the encoder state, 
                                      // it should be polled frequectly or called from the interrupt routine.

//
// Implementation
//

struct EC11_Encoder {
  int pin_a, pin_b;
  uint8_t state;
  int offset;
};

void encoder_update(EC11_Encoder* e) {
  int a = digitalRead(e->pin_a);
  int b = digitalRead(e->pin_b);

  // Encoder pin A and B produces the following sequcnces when turned:
  //      AB AB AB
  // CW:  01 00 10
  // CCW: 00 01 11

  int current_state = (a << 1) | b;
  if (current_state != (e->state & 0x3)) {
    e->state = ((e->state << 2) | current_state) & 0b00111111;
    if      (e->state == 0b010010) ++e->offset; // turned clock wise
    else if (e->state == 0b000111) --e->offset; // turned counter clock wise
  }
}

int encoder_get_offset(EC11_Encoder* e, bool reset) {
  int result = e->offset;
  if (reset) e->offset = 0;
  return result;
}

//
// Gross macro workaround for setting up interrupt routine on AVR boards,
// this is to keep a minimal and consistant interface defination for encoder_init()
//
#if defined(ARDUINO_ARCH_AVR)

template <EC11_Encoder* e>
void encoder_isr() {
  encoder_update(e);
}

#define encoder_attach_interrupt(e) encoder_attach_interrupt_<e>()
template <EC11_Encoder* e>
void encoder_attach_interrupt_() {
  attachInterrupt(digitalPinToInterrupt(e->pin_a), encoder_isr<e>, CHANGE);
  attachInterrupt(digitalPinToInterrupt(e->pin_b), encoder_isr<e>, CHANGE);
}

#define encoder_init(e, pin_a, pin_b, ...) encoder_init_<e>(pin_a, pin_b, ##__VA_ARGS__)
template<EC11_Encoder* e>
void encoder_init_(int pin_a, int pin_b, bool interrupt = true) {

#elif defined(ARDUINO_ARCH_ESP32)

void ARDUINO_ISR_ATTR encoder_isr(void* arg) {
  encoder_update((EC11_Encoder*)arg);
}

void encoder_attach_interrupt(EC11_Encoder* e) {
  attachInterruptArg(e->pin_a, encoder_isr, e, CHANGE);
  attachInterruptArg(e->pin_b, encoder_isr, e, CHANGE);
}

void encoder_init(EC11_Encoder* e, int pin_a, int pin_b, bool interrupt) {
#endif
  *e = {};
  e->pin_a = pin_a;
  e->pin_b = pin_b;
  pinMode(pin_a, INPUT);
  pinMode(pin_b, INPUT);
  if (interrupt) encoder_attach_interrupt(e);
}
