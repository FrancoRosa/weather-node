#include <Arduino.h>
#line 1 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
#line 1 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
// Weather station node
// BoardURL: http://dan.drown.org/stm32duino/package_STM32duino_index.json
// Version: 2020.12.26
//    board: stm32duino:STM32F1:genericSTM32F103C6,
//    configuration: upload_method=STLinkMethod,
//                   cpu_speed=speed_48mhz,
//                   opt=osstd

// Connections:
//   Device   Port      TX   RX
//   USB      Serial    --   --  
//   SIM808   Serial1   PA9  PA10  
//   PM1      Serial2   PA2  PA3      SDS011
//   PM2      Serial3   PB10 PB11     PMS5003
//   AM3201   GPIO      PB0

#include "MapleFreeRTOS821.h"
#include "DHT.h"

#define led_pin  PC13
#define dht_pin  PB9
#define pkey_pin PA0
#define dht_type DHT21

// Variables of environment
volatile float temperature = 0;
volatile float humidity = 0;

// Variables of particle measurement
volatile int pm2_value = 0;
volatile float pm1_value = 0;

// GNSS Variables
char timestamp[19] = "20210125060840.000";
char latitude[11] = "-13.536150";
char longitude[11] = "-71.953617";

// POST Variables
const char key_temperature[] = "18";
const char key_humidity[]    = "19";
const char key_latitude[]    = "21";
const char key_longitude[]   = "22";
const char key_pm1_value[]   = "23";
const char key_pm2_value[]   = "24";
const char key_timestamp[]   = "26";
char post_buffer[120];

DHT dht(dht_pin, dht_type);
// variables to manage PM1
const int pm1_buff_size = 15;
int pm1_i = 0;
char pm1_buff[pm1_buff_size];
bool pm1_ok = false;

// variables to manage PM2
const int pm2_buff_size = 40;
int pm2_i = 0;
char pm2_buff[pm2_buff_size];
bool pm2_ok = false;

// Modem management variables
volatile bool flagOK = false;
volatile bool flagERROR = false;

#line 65 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void readTempHum();
#line 70 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void readPM2();
#line 76 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void readPM1();
#line 82 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void processingPM1Data(char c);
#line 96 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void processingPM2Data(char c);
#line 111 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void displayValues();
#line 127 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void sendFrame(Stream *port);
#line 148 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void showBuffers();
#line 169 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void mPower();
#line 175 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void blink();
#line 180 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
bool sendCommand(const char *command,int timeout);
#line 188 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
bool waitOk(int timeout);
#line 203 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
bool sim808Init();
#line 213 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
static void task_modem(void *pvParameters);
#line 222 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
static void task_sensors(void *pvParameters);
#line 239 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void setup();
#line 252 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void loop();
#line 65 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void readTempHum() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
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

void processingPM1Data(char c) {
  // Serial.write(c);
  pm1_buff[pm1_i]=c;
  if (pm1_buff[pm1_i]   == 0xC0 && 
      pm1_buff[pm1_i-1] == 0xAA && 
      pm1_buff[pm1_i-2] == 0xAB) {
    pm1_value = (pm1_buff[2]*256 + pm1_buff[1])/10;
    pm1_i=0;
    pm1_ok=true;
  }
  pm1_i++;
  if (pm1_i>=pm1_buff_size) {pm1_i=0; pm1_ok=false;}
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

void displayValues() {
  char temp_buffer[120];
  sprintf(
    temp_buffer,
    "temp: %2.2f, hum: %2.2f\n"
    "pm1: %2.2f, pm2: %d\n"
    "lat: %s, lon: %s\n"
    "time: %s\n",
    temperature, humidity,
    pm1_value, pm2_value,
    latitude, longitude,
    timestamp
  );
  Serial.println(temp_buffer);
}

void sendFrame(Stream *port) {
  sprintf(
    post_buffer,
    "{"
      "\"sensor\":{"
        "\"id\":[%s,%s,%s,%s,%s,%s,%s],"
        "\"value\":[%2.1f,%2.1f,%s,%s,%2.1f,%d,%s]"
      "}"
    "}",
    key_temperature, key_humidity,
    key_latitude, key_longitude,
    key_pm1_value, key_pm2_value,
    key_timestamp,
    temperature, humidity,
    latitude, longitude,
    pm1_value, pm2_value,
    timestamp
  );
  port->print(post_buffer);
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
  digitalWrite(pkey_pin,LOW);
  vTaskDelay(1000);
  digitalWrite(pkey_pin,HIGH);
}

void blink() {
  digitalWrite(led_pin, HIGH); vTaskDelay(500);
  digitalWrite(led_pin, LOW); vTaskDelay(500);
}

bool sendCommand(const char *command,int timeout)
{
  Serial1.print("AT+");
  Serial1.print(command);
  Serial1.print("\r");
  return waitOk(timeout);
}

bool waitOk(int timeout)
{
  flagOK=0;
  flagERROR=0;
  timeout= timeout*10;
  int t = 0;
  while (timeout>t)
  {
    t++;
    if (flagOK || flagERROR) return true;
    vTaskDelay(100);
  }
  return false;
}

bool sim808Init()
{
  while(true)
  {
    if(sendCommand("GSN",5) && sendCommand("CGNSPWR=1",5))
    break;
  }

}

static void task_modem(void *pvParameters) {
  Serial1.begin(9600);
  Serial1.println("Start >>>");
  while(true) {
    vTaskDelay(500);
    Serial1.println("Succcess");
  }
}

static void task_sensors(void *pvParameters) {
  pinMode(led_pin, OUTPUT);
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  dht.begin();
  Serial.println("Start >>>");
  while(true) {
    blink();
    readTempHum();
    readPM2();
    readPM1();
    displayValues();
    Serial.println();
  }
}

void setup() {
  xTaskCreate(
    task_modem,"TModem",
    64,NULL,2,NULL
  );
  xTaskCreate(
    task_sensors,"TSensors",
    256,NULL,1,NULL
  );
  vTaskStartScheduler();
  while(true);
}

void loop(){
  while (true){
    ;
  }
  
}


