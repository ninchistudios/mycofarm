/*
  You can delete the blink code obv
*/
#include "DHT.h" // Adafruit Temp & Humidity sensor library
#include "Adafruit_CCS811.h" // Adafruit CCS811 eCO2/TVOC sensor library

const int RELAYPIN1 = 13; // Relay pin
const int RELAYPIN2 = 12; // Relay pin
const int RELAYPIN3 = 11; // Relay pin
const int RELAYPIN4 = 10; // Relay pin
const int DHTPIN = 8; // Digital pin connected to the DHT sensor
const char DHTTYPE = DHT11; // DHT 11
// const char DHTTYPE = DHT22; // DHT 22  (AM2302), AM2321
// const char DHTTYPE = DHT21; // DHT 21 (AM2301)
const int TEMPTUNEF = 32; // Tuning for CO2 sensor temp
const char *TUBTYPE[] = {"Nameko","KingTrumpet","Shiitake","LionsMane","GardenGiant","Other"};
// choose 1 grow CONFIG: TubType, MinTemp, MaxTemp, MinHumi, MaxHumi, MaxCO2, LightOn, LightOff
// const int CONFIG[] = {0,13,17,85,95,10000,10,16}; // Nameko
// const int CONFIG[] = {1,13,17,85,95,10000,10,16}; // King Trumpet
const int CONFIG[] = {2,13,17,85,95,10000,10,16}; // Shiitake
// const int CONFIG[] = {3,13,17,85,95,10000,10,16}; // Lions Mane
// const int CONFIG[] = {4,13,17,85,95,10000,10,16}; // Garden Giant
const String TUBID = "A"; // ensure this matches the tub

Adafruit_CCS811 ccs;
DHT dht(DHTPIN, DHTTYPE);

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital I/O
  pinMode(RELAYPIN1, OUTPUT);
  pinMode(RELAYPIN2, OUTPUT);
  pinMode(RELAYPIN3, OUTPUT);
  pinMode(RELAYPIN4, OUTPUT);
  // serial comm
  Serial.begin(9600);
  Serial.println(F("MycoFarm Init"));
  // init the DHT sensor
  dht.begin();
  if(!ccs.begin()){
    Serial.println("FATAL: Failed to start CO2 sensor - check wiring");
    while(1);
  }
  // Wait for the sensor to be ready
  while(!ccs.available());
  ccs.setTempOffset(TEMPTUNEF);
}

// the loop function runs over and over again forever
void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Identifying Info
  Serial.print("|Tub:" + TUBID + "|Type:" + TUBTYPE[CONFIG[0]]);

  // CO2, TVOC and Check Temp
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
      Serial.print("|WARN: CCS811 sensor fail");
    }
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

  // Fin
  Serial.println(F("|"));
}
