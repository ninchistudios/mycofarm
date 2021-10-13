/*
  MycoFarm
  https://github.com/ninchistudios/mycofarm
*/
#include "DHT.h" // Adafruit Temp & Humidity sensor library
#include "Adafruit_CCS811.h" // Adafruit CCS811 eCO2/TVOC sensor library
#include "U8glib.h" // OLED display library

// ### NO NEED TO CHANGE THESE IN PRODUCTION
const int RELAYPIN = 12; // Relay pin IN1 - 24VAC irrigation
bool RELAYON = false; // used for debugging to flip relay state
const int DHTPIN = 8; // Digital pin connected to the DHT sensor
const int FANPWMPIN = 6; // PWM pin to control a 3-pin FAE case fan
const int SCKPIN = 13; // OLED SCK
const int MOSIPIN = 11; // OLED MOSI
const int CSPIN = 10; // OLED CS
const int A0PIN = 9; // OLED A0
const char DHTTYPE = DHT11; // DHT 11
// const char DHTTYPE = DHT22; // DHT 22  (AM2302), AM2321
// const char DHTTYPE = DHT21; // DHT 21 (AM2301)
const int TEMPTUNEF = 32; // Tuning for CO2 sensor temp
const int IODELAY = 20000; // delay between sensor readings in ms
int FANPWM = 0; // value 0-255 to send to FAE Fan PWM
const bool ENABLEFAN = false; // do we run the FAE fan code
const bool ENABLERELAY = false; // do we run the relay code
const bool ENABLECO2 = true; // do we run the CO2/VOC sensor code
const bool ENABLEDHT = true; // do we run the DHT temp/pressure sensor code
const bool ENABLEOLED = true; // do we run the OLED display
// ###

// ### RARELY CHANGED IN PRODUCTION
const char *TUBTYPE[] = {"Nameko","KingOyster","Shiitake","LionsMane","GardenGiant","Martha","Reishi"};
// ###

// ### POTENTIALLY CHANGE FOR EACH TUB AS CAKES ARE BIRTHED AND GROW PARAMS ARE TUNED
// choose 1 grow CONFIG: TubType, MinTemp, MaxTemp, MinHumi, MaxHumi, MaxCO2, LightOn, LightOff
// const int CONFIG[] = {0,13,17,85,95,10000,10,16}; // Nameko
// const int CONFIG[] = {1,13,17,85,95,10000,10,16}; // King Oyster
// const int CONFIG[] = {2,13,17,85,95,10000,10,16}; // Shiitake
// const int CONFIG[] = {3,13,17,85,95,10000,10,16}; // Lions Mane
// const int CONFIG[] = {4,13,17,85,95,10000,10,16}; // Garden Giant
const int CONFIG[] = {5,18,24,85,95,10000,10,16}; // Martha
// const int CONFIG[] = {6,13,17,85,95,10000,10,16}; // Reishi
const String TUBID = "1"; // ensure this matches the tub that you're deploying to
const char* HEADMSG = "-= Martha 01 =-";
// ###

// ### PRODUCTION CODE BELOW HERE
Adafruit_CCS811 ccs;
DHT dht(DHTPIN, DHTTYPE);
U8GLIB_SH1106_128X64 u8g(SCKPIN, MOSIPIN, CSPIN, A0PIN); // SCK = 13, MOSI = 11, CS = 10, A0 = 9
float oledtmp = -1.0;
float oledhum = -1.0;
float oledco2 = -1.0;
float oledvoc = -1.0;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital I/O
  pinMode(RELAYPIN, OUTPUT);
  pinMode(FANPWMPIN, OUTPUT);
  // serial comm
  Serial.begin(9600);
  Serial.println(F("MycoFarm Init"));
  //analogWrite(FANPWMPIN,255);
  //delay(1000);
  analogWrite(FANPWMPIN,0);
  digitalWrite(RELAYPIN, LOW);
  
  // init the sensors
  if (ENABLEOLED) {
    u8g.setRot180();
    if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
      u8g.setColorIndex(255); // white
    } else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
      u8g.setColorIndex(3); // max intensity
    } else if ( u8g.getMode() == U8G_MODE_BW ) {
      u8g.setColorIndex(1); // pixel on
    } else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
      u8g.setHiColorByRGB(255,255,255);
    }
    u8g.setFont(u8g_font_unifont);
  }
  if (ENABLEDHT) {
    dht.begin();
  }
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
  if (ENABLEOLED) {
    u8g.firstPage();
    do {
      draw();
    } while (u8g.nextPage());
  }
  
  // Identifying Info
  Serial.print("|Tub:" + TUBID + "|Type:" + TUBTYPE[CONFIG[0]]);

  // CO2, TVOC and Check Temp
  if (ENABLECO2) {
    if(ccs.available()){
      if(!ccs.readData()){
        Serial.print("|CO2:");
        oledco2 = ccs.geteCO2();
        Serial.print(oledco2);
        Serial.print("|TVOC:");
        oledvoc = ccs.getTVOC();
        Serial.print(oledvoc);
        Serial.print("|ChkT:");
        Serial.print((ccs.calculateTemperature() - 32) * 5 / 9);
      }
      else{
        Serial.print("|WARN:CCS811-FAIL");
        oledco2 = -1.0;
        oledvoc = -1.0;
      }
    }
  } else {
    Serial.print(F("|CCS811:NC"));
  }

  // Temp and Humidity - takes about 250ms and may be up to 2s old
  if (ENABLEDHT) {
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
  } else {
    Serial.print(F("|DHT:NC")); 
  }

  if (ENABLEFAN) {
    // Oscillate the fan for now
    FANPWM = (FANPWM == 0) ? 255 : 0;
    analogWrite(FANPWMPIN,FANPWM);
    Serial.print(F("|FAEFan:"));
    Serial.print(FANPWM);
  } else {
    Serial.print(F("|FAN:NC"));
  }
  

  if (ENABLERELAY) {
    // Oscillate the valve for now
    // irrigation
    RELAYON = !RELAYON;
    digitalWrite(RELAYPIN, RELAYON);
    Serial.print(F("|RELAYON:"));
    Serial.print(RELAYON);
  } else {
    Serial.print(F("|RELAY:NC"));
  }
  
  // Fin
  Serial.println(F("|"));

  delay(IODELAY);
}

void draw(void) {
  u8g.drawStr(0,10,HEADMSG);
  u8g.drawStr(0,23,"TMP:        ");
  u8g.setPrintPos(36,23);
  u8g.print(oledtmp);
  u8g.drawStr(0,36,"%RH:        ");
  u8g.setPrintPos(36,36);
  u8g.print(oledhum);
  u8g.drawStr(0,49,"CO2:        ");
  u8g.setPrintPos(36,49);
  u8g.print(oledco2);
  u8g.drawStr(0,62,"VOC:        ");
  u8g.setPrintPos(36,62);
  u8g.print(oledvoc);
}
