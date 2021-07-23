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
const unsigned long DEBOUNCEDELAY = 50; 
// vars
int BUTTONSTATE[] = {0,0,0};
int BUTTON1STATELAST[] = {0,0,0};
int LEDSTATE[] = {LOW,LOW,LOW};
int CHANNELSTATE[] = {STATE_TIMER_WAIT,STATE_TIMER_WAIT,STATE_TIMER_WAIT};
unsigned long LASTDEBOUNCE[] = {0,0,0};

// ###

// ### RARELY CHANGED IN PRODUCTION
// ###

// ### PRODUCTION CODE BELOW HERE

void setup() {
  
  // initialize I/O
  for (int i=0; i<CHANNELS; i++) {
    pinMode(LEDPIN[i], OUTPUT);
    pinMode(BUTTONPIN[i], INPUT);
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
 
}

void setLeds() {
  for (int i=0; i<CHANNELS; i++) {
    digitalWrite(LEDPIN[i],LEDSTATE[i]);
  }
}

void checkButtons() {
  for (int i=0; i<CHANNELS; i++) {
    
  }
}
