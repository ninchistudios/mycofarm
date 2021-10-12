/*
  MycoFarm
  https://github.com/ninchistudios/mycofarm
*/
#include "DHT.h" // Adafruit Temp & Humidity sensor library
#include "Adafruit_CCS811.h" // Adafruit CCS811 eCO2/TVOC sensor library

// ### NO NEED TO CHANGE THESE IN PRODUCTION
const int RELAYPIN1 = 12; // Relay pin IN1
const int RELAYPIN2 = 11; // Relay pin IN2 - 24VAC irrigation
const int RELAYPIN3 = 10; // Relay pin
const int RELAYPIN4 = 9; // Relay pin
bool RELAYON = false; // used for debugging to flip relay state
const int DHTPIN = 8; // Digital pin connected to the DHT sensor
const int FANPWMPIN = 6; // PWM pin to control a 3-pin FAE case fan
const char DHTTYPE = DHT11; // DHT 11
// const char DHTTYPE = DHT22; // DHT 22  (AM2302), AM2321
// const char DHTTYPE = DHT21; // DHT 21 (AM2301)
const int TEMPTUNEF = 32; // Tuning for CO2 sensor temp
const int IODELAY = 5000; // delay between sensor readings in ms
int FANPWM = 0; // value 0-255 to send to FAE Fan PWM
const bool ENABLEFAN = false; // do we run the FAE fan code
const bool ENABLERELAY = false; // do we run the relay code
const bool ENABLECO2 = false; // do we run the CO2/VOC sensor code
// ###

// ### RARELY CHANGED IN PRODUCTION
const char *TUBTYPE[] = {"Nameko","KingOyster","Shiitake","LionsMane","GardenGiant","General","Reishi"};
// ###

// ### POTENTIALLY CHANGE FOR EACH TUB AS CAKES ARE BIRTHED AND GROW PARAMS ARE TUNED
// choose 1 grow CONFIG: TubType, MinTemp, MaxTemp, MinHumi, MaxHumi, MaxCO2, LightOn, LightOff
// const int CONFIG[] = {0,13,17,85,95,10000,10,16}; // Nameko
// const int CONFIG[] = {1,13,17,85,95,10000,10,16}; // King Oyster
// const int CONFIG[] = {2,13,17,85,95,10000,10,16}; // Shiitake
// const int CONFIG[] = {3,13,17,85,95,10000,10,16}; // Lions Mane
// const int CONFIG[] = {4,13,17,85,95,10000,10,16}; // Garden Giant
const int CONFIG[] = {5,18,24,85,95,10000,10,16}; // General
// const int CONFIG[] = {6,13,17,85,95,10000,10,16}; // Reishi
const String TUBID = "M"; // ensure this matches the tub that you're deploying to
// ###

// ### PRODUCTION CODE BELOW HERE

Adafruit_CCS811 ccs;
DHT dht(DHTPIN, DHTTYPE);

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital I/O
  pinMode(RELAYPIN1, OUTPUT);
  pinMode(RELAYPIN2, OUTPUT);
  pinMode(RELAYPIN3, OUTPUT);
  pinMode(RELAYPIN4, OUTPUT);
  pinMode(FANPWMPIN, OUTPUT);
  // serial comm
  Serial.begin(9600);
  Serial.println(F("MycoFarm Init"));
  //analogWrite(FANPWMPIN,255);
  //delay(1000);
  analogWrite(FANPWMPIN,0);
  digitalWrite(RELAYPIN1, LOW);
  digitalWrite(RELAYPIN2, LOW);
  digitalWrite(RELAYPIN3, LOW);
  digitalWrite(RELAYPIN4, LOW);
  // init the sensors
  dht.begin();
  if (ENABLECO2) {
    if(!ccs.begin()){
      Serial.println("FATAL: Failed to start CO2 sensor - check wiring");
      while(1);
    }
    // Wait for the sensors to be ready
    while(!ccs.available());
    ccs.setTempOffset(TEMPTUNEF);
  }
}

void loop() {
  delay(IODELAY);
  
  // Identifying Info
  Serial.print("|Tub:" + TUBID + "|Type:" + TUBTYPE[CONFIG[0]]);

  // CO2, TVOC and Check Temp
  if (ENABLECO2) {
    if(ccs.available()){
      if(!ccs.readData()){
        Serial.print("|CO2:");
        Serial.print(ccs.geteCO2());
        Serial.print("|TVOC:");
        Serial.print(ccs.getTVOC());
        Serial.print("|ChkTemp:");
        Serial.print((ccs.calculateTemperature() - 32) * 5 / 9);
      }
      else{
        Serial.print("|WARN:CCS811-FAIL");
      }
    }
  } else {
    Serial.print(F("|CO2/VOC:NC"));
  }

  // Temp and Humidity - takes about 250ms and may be up to 2s old
  float h = dht.readHumidity(); // in % RH
  float t = dht.readTemperature(); // deg C

  if (isnan(h) || isnan(t)) {
    Serial.print(F("|WARN: DHT sensor fail"));
  } else {
    Serial.print(F("|Humi:"));
    Serial.print(h);
    Serial.print(F("|Temp:"));
    Serial.print(t);
  }

  if (ENABLEFAN) {
    // Oscillate the fan for now
    FANPWM = (FANPWM == 0) ? 255 : 0;
    analogWrite(FANPWMPIN,FANPWM);
    Serial.print(F("|FAEFan:"));
    Serial.print(FANPWM);
  } else {
    Serial.print(F("|FAEFan:NC"));
  }
  

  if (ENABLERELAY) {
    // Oscillate the valve for now
    // irrigation
    RELAYON = !RELAYON;
    digitalWrite(RELAYPIN2, RELAYON);
    Serial.print(F("|RELAYON:"));
    Serial.print(RELAYON);
  } else {
    Serial.print(F("|RELAY:NC"));
  }
  

  // Fin
  Serial.println(F("|"));
}
