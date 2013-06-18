// Blinds - A Program to operate a motor that opens and closes blinds according to a schedule.

//Circuit:
//  2 -> Push Button Hour
//  3 -> Push Button Min
// Note - There are only 2 external interrupt capable pins! I need a mode switch, and to re-label the buttons up and down.
// I can poll the mode switch pin to see what needs to go up or down - the trigger time or the current time.

#define BLIND_MOVING 0
#define BLIND_TOP 1
#define BLIND_BOTTOM -1

const int MODE_PIN = 2;
const int SET_PIN = 3;
const int MODESELECT_PIN = 4;
const int MOTOR_CONTROL_PIN = 5;

const int MOVING_UP_TIME = 10 * 1000;
const int MOVING_DOWN_TIME = 10 * 1000;

//
volatile int triggerMins = 0;
volatile int triggerHours = 7;

volatile int hours = 0;
volatile int mins = 0;
volatile int seconds = 0;

// Modes for UI
volatile int mode = 0;
const int NUMBER_OF_MODES = 4;
#define MODE_CLOCK_SETMINS 0
#define MODE_CLOCK_SETHOURS 1
#define MODE_TRIGGER_SETMINS 2
#define MODE_TRIGGER_SETHOURS 3

volatile int blindState;

void setup() {
  attachInterrupt(MODE_PIN, modeButton, FALLING);
  attachInterrupt(SET_PIN, setButton, FALLING);
  TCCR1A = 0;    // set entire TCCR1A register to 0
  TCCR1B = 0;


  // Set CS10 and CS12 bits for 1024 prescaler:
  TCCR1B |= (1 << CS10);
  TCCR1B |= (1 << CS12);
  //Set compare match to 15625: (16000000Hz / 1024 = 15625)
  OCR1A = 0x3D09;
  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);
  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);
  
  hours = 0;
  mins = 0;
  sei();
}

void loop() {
  
}

ISR(TIMER1_COMPA_vect) {
  seconds++;
  if (seconds >= 60) {
    mins++;
    seconds = 0;
  }
  if (mins >= 60) {
    hours ++;
    mins = 0;
  }
  if (hours >= 24) {
    hours = 0;
  }
  if ((hours == triggerHours)  && (mins == triggerMins)) {
    openBlinds();
  }
  return;
}

void modeButton(){ //toggles the set button mode
  mode++;
  mode %= (NUMBER_OF_MODES - 1);
  return;
}

void setButton() { //Add a minuit to a clock
  switch (mode) {
    case MODE_CLOCK_SETMINS:
        mins++;
        if (mins >= 60) {
          mins = 0;
        }
        break;
    case MODE_CLOCK_SETHOURS:
        hours++;
        if (hours >= 24) {
          hours = 0;
        }
        break;
    case MODE_TRIGGER_SETMINS:
        triggerMins++;
        if (triggerMins >= 60) {
          triggerMins = 0;
        }
        break;
    case MODE_TRIGGER_SETHOURS:
        triggerHours++;
        if (triggerHours >= 24) {
          triggerHours = 0;
        }
        break;
  }
  return;
}

void openBlinds(){
  if (blindState == BLIND_BOTTOM) {
    digitalWrite(MOTOR_CONTROL_PIN, HIGH);
    delay(MOVING_UP_TIME);
    digitalWrite(MOTOR_CONTROL_PIN, LOW);
    blindState = BLIND_TOP;
  }
  return;
}

void closeBlinds(){
  if (blindState == BLIND_TOP) {
    digitalWrite(MOTOR_CONTROL_PIN, HIGH);
    delay(MOVING_DOWN_TIME);
    digitalWrite(MOTOR_CONTROL_PIN, LOW);
    blindState = BLIND_BOTTOM;
  }
  return;
}

