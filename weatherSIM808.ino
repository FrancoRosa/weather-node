// Weather station node
// BoardURL: http://dan.drown.org/stm32duino/package_STM32duino_index.json
// Version: 2020.12.26
//    board: stm32duino:STM32F1:genericSTM32F103C6,
//    configuration: upload_method=STLinkMethod,
//                   cpu_speed=speed_48mhz,
//                   opt=osstd

// Conections:
//   Device   Port      TX   RX
//   USB      Serial0   --   --  
//   SIM808   Serial1   PA9  PA10  
//   PM1      Serial2   PA2  PA3
//   PM2      Serial3   PB10 PB11
//   AM3201   GPIO      PB0

#include "DHT.h"

#define led_pin  PC13
#define dht_pin  PB0
#define dht_type DHT21
#define m_tx 2
#define m_rx 3
#define m_pw 7

DHT dht(dht_pin, dht_type);

// variables to save temp and humidity
volatile float temperature = 0;
volatile float humidity = 0;

// variables to manage PM1
const int pm1_buff_size = 15;
volatile int pm1_i = 0;
volatile int pm1_value = 0;
char pm1_buff[pm1_buff_size];
volatile bool pm1_ok = false;

// variables to manage PM2
const int pm2_buff_size = 40;
volatile int pm2_i = 0;
volatile int pm2_value = 0;
char pm2_buff[pm2_buff_size];
volatile bool pm2_ok = false;

// json keys
const char key_temperature[] = "18";
const char key_humidity[]    = "19";
const char key_latitude[]    = "21";
const char key_longitude[]   = "22";
const char key_pm1_value[]   = "23";
const char key_pm2_value[]   = "24";
const char key_timestamp[]   = "26";

// GNSS Variables
char timestamp[] = "20210125060840.000";
char latitude[] = "-13.536150";
char longitude[] = "-71.953617";

// POST Variables
#define post_size 30
#define id_size 50
#define value_size 50


char post_buffer[post_size];
char id_buffer[id_size];
char value_buffer[value_size];

void readTempHum() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  // Serial.print("temp: ");
  // Serial.print(temperature);
  // Serial.print(", hum: ");
  // Serial.print(humidity);
  // Serial.println();
}

void readPM2() {
  while(Serial3.available()) {
    processingPM2Data(Serial3.read());
  }
}

void readPM1() {
  while(Serial2.available()) {
    processingPM1Data(Serial2.read());
  }
}

void processingPM2Data(char c) {
  // Serial.write(c);
  pm2_buff[pm2_i]=c;
  if (pm2_buff[pm2_i]   == 0x1C && 
      pm2_buff[pm2_i-1] == 0x00 && 
      pm2_buff[pm2_i-2] == 0x4D &&
      pm2_buff[pm2_i-3] == 0x42) {
    pm2_value = pm2_buff[1]*256 + pm2_buff[2];
    pm2_i=0;
    pm2_ok=true;
  }
  pm2_i++;
  if (pm2_i>=pm2_buff_size) {pm2_i=0; pm2_ok=false;}
}

void processingPM1Data(char c) {
  // Serial.write(c);
  pm1_buff[pm1_i]=c;
  if (pm1_buff[pm1_i]   == 0xC0 && 
      pm1_buff[pm1_i-1] == 0xAA && 
      pm1_buff[pm1_i-2] == 0xAB) {
    pm1_value = pm1_buff[2]*256 + pm1_buff[1];
    pm1_i=0;
    pm1_ok=true;
  }
  pm1_i++;
  if (pm1_i>=pm1_buff_size) {pm1_i=0; pm1_ok=false;}
}

void displayValues() {
  Serial.print("temp: ");
  Serial.print(temperature);
  Serial.print(", hum: ");
  Serial.print(humidity);
  Serial.print(", PM1: ");
  Serial.print(pm1_value);
  Serial.print(", PM2: ");
  Serial.print(pm2_value);
  Serial.println();
  Serial.println();
}

void sendFrame(Stream *s) {
  s->print("{\"sensor\":{");
  s->print("\"id\":[");
  s->print(key_temperature); s->print(",");
  s->print(key_humidity); s->print(",");
  s->print(key_latitude); s->print(",");
  s->print(key_longitude); s->print(",");
  s->print(key_pm1_value); s->print(",");
  s->print(key_pm2_value); s->print(",");
  s->print(key_timestamp); s->print("],");
  s->print("\"value\":[");
  s->print(padding3(temperature)); s->print(",");
  s->print(padding3(humidity)); s->print(",");
  s->print(latitude); s->print(",");
  s->print(longitude); s->print(",");
  s->print(padding4(pm1_value)); s->print(",");
  s->print(padding4(pm2_value)); s->print(",");
  s->print(timestamp); s->print("]");
  s->print("}}");
}

void padding4(int number){
  int count = 0;
  if(number < 1000) count++;
  if(number < 100) count++;
  if(number < 10) count++;
  while (count > 0){
    Serial.print('0');  
    count--;}
  Serial.print(number);
}

void padding3(int number){
  int count = 0;
  if(number < 100) count++;
  if(number < 10) count++;
  while (count > 0){
    Serial.print('0');  
    count--;}
  Serial.print(number);
}

void showBuffers(){
  Serial.print("Buffer PM1: ");
  for (int i = 0; i < 10; i++) {
    Serial.print(pm1_buff[i],HEX);
    Serial.print(" ");
  }
  Serial.print(" ");
  Serial.print(pm1_ok ? "OK": "ERROR");
  Serial.println("");
  
  Serial.print("Buffer PM2: ");
  for (int i = 0; i < 10; i++) {
    Serial.print(pm2_buff[i],HEX);
    Serial.print(" ");
  }
  Serial.print(" ");
  Serial.print(pm2_ok ? "OK": "ERROR");
  Serial.println("");
  
}

void mPower() {
  digitalWrite(m_pw,LOW);
  delay(1000);
  digitalWrite(m_pw,HIGH);
}

void blink() {
  digitalWrite(led_pin, HIGH); delay(500);
  digitalWrite(led_pin, LOW); delay(500);
}

void setup() {
  pinMode(led_pin, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  dht.begin();
}

void loop(){
  blink();
  readTempHum();
  readPM2();
  readPM1();
  // showBuffers();
  // displayValues();
  sendFrame(&Serial);
  Serial.println();
  Serial.println();
}

