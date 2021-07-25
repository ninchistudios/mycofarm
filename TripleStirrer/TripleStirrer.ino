/*
  MycoFarm TripleStirrer
  https://github.com/ninchistudios/mycofarm
*/

// ### NO NEED TO CHANGE THESE IN PRODUCTION
// consts
const bool DEBUG = false;
const int BUTTONPIN[] = {2,7,8}; // Digital
const int LEDPIN[] = {A0,A1,A2}; // Analog
const int SPINPIN[] = {3,5,6}; // PWM
const int IODELAY = 1000;
const int CHANNELS = 3;
const int STATE_TIMER_WAIT = 0;
const int STATE_TIMER_ACTIVE = 1;
const int STATE_OVERRIDE_ON = 2;
const int MAXVAL = 255;
const unsigned long DEBOUNCEDELAY = 150; 
const unsigned long DEBUGDELAY = 10000;
const unsigned long BLINKINTERVAL = 500;
// vars
int BUTTONSTATE[] = {HIGH,HIGH,HIGH};
int BUTTONSTATELAST[] = {HIGH,HIGH,HIGH};
int LEDSTATE[] = {LOW,LOW,LOW};
int SPINSTATE[] = {0,0,0};
int CHANNELSTATE[] = {STATE_TIMER_WAIT,STATE_TIMER_WAIT,STATE_TIMER_WAIT};
unsigned long LASTDEBOUNCE[] = {0,0,0};
unsigned long LASTBLINKON[] = {0,0,0};
unsigned long LASTDEBUG = 0;
// ###

// ### PRODUCTION CODE BELOW HERE

void setup() {
  
  // initialize I/O
  for (int i=0; i<CHANNELS; i++) {
    pinMode(LEDPIN[i], OUTPUT);
    // enables the 5v pullup resistor so we don't need it externally
    // note that a press will go LOW - open read HIGH
    pinMode(BUTTONPIN[i], INPUT_PULLUP); 
    pinMode(SPINPIN[i], OUTPUT);
    digitalWrite(LEDPIN[i],LEDSTATE[i]);
    analogWrite(SPINPIN[i],SPINSTATE[i]);
  }
  
  if (DEBUG) {
    // serial comm
    Serial.begin(9600);
    Serial.println(F("MycoFarm TripleStirrer v1 Init"));
  }

  // delay for ethernet?
  delay(1000);
  
}

void loop() { 
  checkButtons();
  setSpinners();
  setLeds();
  if (DEBUG) doDebug();
}

void setSpinners() {
  for (int i=0; i<CHANNELS; i++) {
    if ((CHANNELSTATE[i] == STATE_OVERRIDE_ON) || (CHANNELSTATE[i] == STATE_TIMER_ACTIVE)) {
      SPINSTATE[i] = MAXVAL;
    } else {
      SPINSTATE[i] = 0;
    }
    analogWrite(SPINPIN[i],SPINSTATE[i]);
  }
}

void setLeds() {
  for (int i=0; i<CHANNELS; i++) {
    switch (CHANNELSTATE[i]) {
      case STATE_OVERRIDE_ON:
        LEDSTATE[i] = HIGH;
        break;
      case STATE_TIMER_WAIT:
        LEDSTATE[i] = LOW;
        break;
      case STATE_TIMER_ACTIVE:
        LEDSTATE[i] = LOW;
        // TODO blinking
        break;
      default:
        // should never get here?
        LEDSTATE[i] = LOW;
        break;
    }
    digitalWrite(LEDPIN[i],LEDSTATE[i]);
  }
}

void checkButtons() {
  for (int i=0; i<CHANNELS; i++) {
    int reading = digitalRead(BUTTONPIN[i]);
    if (reading != BUTTONSTATELAST[i]) { // is there a button state change?
      LASTDEBOUNCE[i] = millis(); // reset the debounce timer
    }
    if ((millis() - LASTDEBOUNCE[i]) > DEBOUNCEDELAY) { // we'll only enter here if the state has been stable for > DEBOUNCEDELAY
      if (reading != BUTTONSTATE[i]) {
        if (DEBUG) doDebugButtonPress(i);
        BUTTONSTATE[i] = reading;
        if (BUTTONSTATE[i] == LOW) {
          CHANNELSTATE[i] = (CHANNELSTATE[i] == STATE_OVERRIDE_ON) ? STATE_TIMER_WAIT : STATE_OVERRIDE_ON;
        }
      }
    }
    BUTTONSTATELAST[i] = reading;
  }
}

void doDebugButtonPress(int channel) {
    Serial.print(F("Button "));
    Serial.print(channel);
    Serial.print(F(" state "));
    Serial.println(BUTTONSTATE[channel]);
}

void doDebug() {
  if (millis() > DEBUGDELAY && ((millis() - DEBUGDELAY) > LASTDEBUG)) {
    Serial.println(F("#############"));
    for (int i=0; i<CHANNELS; i++) {
      Serial.print(F("Channel State "));
      Serial.print(i);
      Serial.print(F(":"));
      Serial.println(CHANNELSTATE[i]);
      Serial.print(F("Button State "));
      Serial.print(i);
      Serial.print(F(":"));
      Serial.println(BUTTONSTATE[i]);
      Serial.print(F("Button Read State "));
      Serial.print(i);
      Serial.print(F(":"));
      Serial.println(digitalRead(BUTTONPIN[i]));
    }
    
    LASTDEBUG = millis();
  }
}
