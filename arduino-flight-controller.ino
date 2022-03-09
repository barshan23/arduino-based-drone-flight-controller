volatile byte last_channel_1 = 0, last_channel_2 = 0,last_channel_3 = 0, last_channel_4 = 0;
//pin: func -> 8: Roll, 9: Pitch, 10: Throttle, 11: Yaw
volatile int receiver_input_channel_1, receiver_input_channel_2, receiver_input_channel_3, receiver_input_channel_4;
volatile unsigned long timer_1, timer_2, timer_3, timer_4;
volatile unsigned long main_loop_timer, esc_timer_1, esc_timer_2, esc_timer_3, esc_timer_4, esc_loop_timer;

void setup() {
  DDRD |= B11110000; // configure ports 4,5,6,7 pin as output
  
  // Enable pin change interrupt
  PCICR |= (1 << PCIE0);
  PCMSK0 |= (1 << PCINT0);
  PCMSK0 |= (1 << PCINT1);
  PCMSK0 |= (1 << PCINT2);
  PCMSK0 |= (1 << PCINT3);

  Serial.begin(9600);

  // Set the main loop timer for the first loop
  main_loop_timer = micros();
}

// Capture the receiver inputs
ISR (PCINT0_vect) {
  unsigned long current_time = micros();

  if (last_channel_1 == 0 && PINB & B00000001) {
    last_channel_1 = 1;
    timer_1 = current_time;
  }
  else if (last_channel_1 == 1 && !(PINB & B00000001)) {
    last_channel_1 = 0;
    receiver_input_channel_1 = current_time - timer_1;  
  }

  if (last_channel_2 == 0 && PINB & B00000010) {
    last_channel_2 = 1;
    timer_2 = current_time;
  }
  else if (last_channel_2 == 1 && !(PINB & B00000010)) {
    last_channel_2 = 0;
    receiver_input_channel_2 = current_time - timer_2;  
  }

  if (last_channel_3 == 0 && PINB & B00000100) {
    last_channel_3 = 1;
    timer_3 = current_time;
  }
  else if (last_channel_3 == 1 && !(PINB & B00000100)) {
    last_channel_3 = 0;
    receiver_input_channel_3 = current_time - timer_3;  
  }

  if (last_channel_4 == 0 && PINB & B00001000) {
    last_channel_4 = 1;
    timer_4 = current_time;
  }
  else if (last_channel_4 == 1 && !(PINB & B00001000)) {
    last_channel_4 = 0;
    receiver_input_channel_4 = current_time - timer_4;  
  }
}

void loop() {
  // wait for 4k microseconds
  // this is for limiting the refresh rate to 250Hz (1000ms / 4ms)
  while(main_loop_timer + 4000 > micros());
  main_loop_timer = micros(); // reset the timer

  PORTD |= B11110000; // Setting 4,5,6,7 pin high

  esc_timer_1 = receiver_input_channel_3 + main_loop_timer;
  esc_timer_2 = receiver_input_channel_3 + main_loop_timer;
  esc_timer_3 = receiver_input_channel_3 + main_loop_timer;
  esc_timer_4 = receiver_input_channel_3 + main_loop_timer;

  while(PORTD >= 16) {
    esc_loop_timer = micros();

    // Setting esc pins low when the required pulse is generated
    if (esc_timer_1 <= esc_loop_timer) PORTD &= B11101111;
    if (esc_timer_2 <= esc_loop_timer) PORTD &= B11011111;
    if (esc_timer_3 <= esc_loop_timer) PORTD &= B10111111;
    if (esc_timer_4 <= esc_loop_timer) PORTD &= B01111111;
  }

  print_receiver_inputs();
}

void print_receiver_inputs() {
  Serial.print("Roll: ");
  Serial.println(receiver_input_channel_1);

  Serial.print("Pitch: ");
  Serial.println(receiver_input_channel_2);

  Serial.print("Throttle: ");
  Serial.println(receiver_input_channel_3);

  Serial.print("Yaw: ");
  Serial.println(receiver_input_channel_4);

  Serial.println("-------------------");
}
