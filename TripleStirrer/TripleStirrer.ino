/*
  MycoFarm TripleStirrer
  https://github.com/ninchistudios/mycofarm
*/

// ### NO NEED TO CHANGE THESE IN PRODUCTION
const int BUTTON1PIN = 2;
const int BUTTON2PIN = 7;
const int BUTTON3PIN = 8;
// const int LED1PIN = ; // A0
// const int LED2PIN = ; // A1
// const int LED3PIN = ; // A2
const int FAN1PIN = 3; // PWM
const int FAN2PIN = 5; // PWM
const int FAN3PIN = 6; // PWM
const int IODELAY = 1000;
int LEDVALUE = 255; // value 0-255
int LEDTEST = 1;
// ###

// ### RARELY CHANGED IN PRODUCTION
// ###

// ### POTENTIALLY CHANGE FOR EACH TUB AS CAKES ARE BIRTHED AND GROW PARAMS ARE TUNED
// ###

// ### PRODUCTION CODE BELOW HERE

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital I/O
  pinMode(A0, OUTPUT); // A0
  pinMode(A1, OUTPUT); // A1
  pinMode(A2, OUTPUT); // A2
  pinMode(BUTTON1PIN, INPUT); // D2
  pinMode(BUTTON2PIN, INPUT); // D7
  pinMode(BUTTON3PIN, INPUT); // D8
  pinMode(FAN1PIN, OUTPUT); // PWM3
  pinMode(FAN2PIN, OUTPUT); // PWM5
  pinMode(FAN3PIN, OUTPUT); // PWM6
  
  // serial comm
  Serial.begin(9600);
  Serial.println(F("MycoFarm TripleStirrer v1 Init"));
  //analogWrite(FANPWMPIN,255);
  //delay(1000);
  digitalWrite(A0,LOW);
  digitalWrite(A1,LOW);
  digitalWrite(A2,LOW);
  analogWrite(FAN1PIN,0);
  analogWrite(FAN2PIN,0);
  analogWrite(FAN3PIN,0);
  
}

void loop() {
  delay(IODELAY);
  if (LEDTEST == 1) {
    digitalWrite(A0, HIGH);
    digitalWrite(A1, LOW);
    digitalWrite(A2, LOW);
    LEDTEST = 2;
  } else if (LEDTEST == 2) {
    digitalWrite(A0, LOW);
    digitalWrite(A1, HIGH);
    digitalWrite(A2, LOW);
    LEDTEST = 3;
  } else if (LEDTEST == 3) {
    digitalWrite(A0, LOW);
    digitalWrite(A1, LOW);
    digitalWrite(A2, HIGH);
    LEDTEST = 1;
  } else {
    LEDTEST = 1;
  }

}
