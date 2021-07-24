/*
  MycoFarm TripleStirrer
  https://github.com/ninchistudios/mycofarm
*/

// ### NO NEED TO CHANGE THESE IN PRODUCTION
const int BUTTONPIN[] = {2,7,8}; // Digital
const int LEDPIN[] = {A0,A1,A2}; // Analog
const int FANPIN[] = {3,5,6}; // PWM
const int IODELAY = 1000;
const int CHANNELS = 3;
const int STATE_TIMER_WAIT = 0;
const int STATE_TIMER_ACTIVE = 1;
const int STATE_OVERRIDE_ON = 2;
const int MAXVAL = 255;
const unsigned long DEBOUNCEDELAY = 150; 
const unsigned long DEBUGDELAY = 10000;
// vars
int BUTTONSTATE[] = {HIGH,HIGH,HIGH};
int BUTTONSTATELAST[] = {HIGH,HIGH,HIGH};
int LEDSTATE[] = {LOW,LOW,LOW};
int CHANNELSTATE[] = {STATE_TIMER_WAIT,STATE_TIMER_WAIT,STATE_TIMER_WAIT};
unsigned long LASTDEBOUNCE[] = {0,0,0};
unsigned long LASTDEBUG = 0;

// ###

// ### RARELY CHANGED IN PRODUCTION
// ###

// ### PRODUCTION CODE BELOW HERE

void setup() {
  
  // initialize I/O
  for (int i=0; i<CHANNELS; i++) {
    pinMode(LEDPIN[i], OUTPUT);
    // enables the 5v pullup resistor so we don't need it externally
    // note that a press will go LOW - open read HIGH
    pinMode(BUTTONPIN[i], INPUT_PULLUP); 
    pinMode(FANPIN[i], OUTPUT);
    digitalWrite(LEDPIN[i],LOW);
    analogWrite(FANPIN[i],0);
  }
  
  // serial comm
  Serial.begin(9600);
  Serial.println(F("MycoFarm TripleStirrer v1 Init"));

  // delay for ethernet?
    delay(1000);
  
}

void loop() { 
  setLeds();
  checkButtons();
  setLedStates();
  doDebug();
}

void setLedStates() {
  for (int i=0; i<CHANNELS; i++) {
    // simple for now, we'll do blinking later
    LEDSTATE[i] = (CHANNELSTATE[i] == STATE_OVERRIDE_ON) ? HIGH : LOW;
  }
}

void setLeds() {
  for (int i=0; i<CHANNELS; i++) {
    digitalWrite(LEDPIN[i],LEDSTATE[i]);
  }
}

void checkButtons() {
  for (int i=0; i<CHANNELS; i++) {
    int reading = digitalRead(BUTTONPIN[i]); // inverted for HIGH default?
    if (reading != BUTTONSTATELAST[i]) { // is there a button state change?
      LASTDEBOUNCE[i] = millis(); // reset the debounce timer
    }
    if ((millis() - LASTDEBOUNCE[i]) > DEBOUNCEDELAY) { // we'll only enter here if the state has been stable for > DEBOUNCEDELAY
      if (reading != BUTTONSTATE[i]) {
        BUTTONSTATE[i] = reading;
        Serial.print(F("Button "));
        Serial.print(i);
        Serial.print(F(" state "));
        Serial.println(BUTTONSTATE[i]);
        if (BUTTONSTATE[i] == LOW) {
          CHANNELSTATE[i] = (CHANNELSTATE[i] == STATE_OVERRIDE_ON) ? STATE_TIMER_WAIT : STATE_OVERRIDE_ON;
        }
      }
    }
    BUTTONSTATELAST[i] = reading;
  }
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
