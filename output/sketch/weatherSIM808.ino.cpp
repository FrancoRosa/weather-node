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
#define pkey_pin PB15
#define dht_type DHT21

// Enable to see console verbose
const bool debug = true;

// Environment variables
DHT dht(dht_pin, dht_type);
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
const int post_period = 10; // Seconds before next post

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
const int modem_buffer_size = 200;
char modem_buffer[modem_buffer_size];
int modem_i=0;

volatile bool flagOK = false;
volatile bool flagERROR = false;
volatile bool flagREG = false;
volatile bool flagGNS = false;
volatile bool flagHTTPACT = false;
volatile bool flagDOWNLOAD = false;

#line 77 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void readTempHum();
#line 82 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void readPM1();
#line 88 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void readPM2();
#line 94 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void processingPM1Data(char c);
#line 108 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void processingPM2Data(char c);
#line 123 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void displayValues();
#line 139 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void buildJSON();
#line 159 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void showBuffers();
#line 180 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void mPower();
#line 187 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void blink();
#line 192 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void procCGR();
#line 204 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void procCGN();
#line 218 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
bool sendCommand(const char *command,int timeout);
#line 225 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
bool waitOk(int timeout);
#line 242 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
bool sim808Init();
#line 250 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
static void task_modem(void *pvParameters);
#line 304 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
static void task_readModem(void *pvParameters);
#line 329 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
static void task_sensors(void *pvParameters);
#line 339 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void setup();
#line 367 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void loop();
#line 77 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void readTempHum() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
}

void readPM1() {
  while(Serial2.available()) {
    processingPM1Data(Serial2.read());
  }
}

void readPM2() {
  while(Serial3.available()) {
    processingPM2Data(Serial3.read());
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

void buildJSON() {
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
  if (debug) Serial.println("... turn modem on");
  digitalWrite(pkey_pin,LOW);
  vTaskDelay(1000);
  digitalWrite(pkey_pin,HIGH);
}

void blink() {
  digitalWrite(led_pin, HIGH); vTaskDelay(500);
  digitalWrite(led_pin, LOW); vTaskDelay(500);
}

void procCGR() {
  char *pch;
  int n;
  pch = (char*) memchr(modem_buffer, ',', 10);
  if (pch != NULL) {
    n = pch-modem_buffer+1;
    if (modem_buffer[n]=='1') {
      flagREG=true;
    } else flagREG=false;
  }
} 

void procCGN() {
  flagGNS = false;
  char *pch;
  pch = (char*) memchr(modem_buffer, ':', 10);
  if (pch != NULL) {
    if (modem_buffer[12]=='1') { // if GNS is fixed
      flagGNS = true;
      memmove(latitude , modem_buffer+33, 10); //JustPick lat and lon
      memmove(longitude, modem_buffer+44, 10); //JustPick lat and lon
      memmove(timestamp, modem_buffer+14, 18); //JustPick lat and lon
    } 
  }
} 

bool sendCommand(const char *command,int timeout) {
  Serial1.print("AT+");
  Serial1.print(command);
  Serial1.print("\r");
  return waitOk(timeout);
}

bool waitOk(int timeout) {
  flagOK=0;
  flagERROR=0;
  timeout= timeout*10;
  int t = 0;
  while (timeout > t)   {
    t++;
    if (flagOK || flagERROR) {
      vTaskDelay(100);
      return true;
    }
    vTaskDelay(100);
  }
  mPower();
  return false;
}

bool sim808Init() {
  mPower();
  while(true) {
    if(sendCommand("GSN",5) && sendCommand("CGNSPWR=1",5))
    break;
  }
}

static void task_modem(void *pvParameters) {
  sim808Init();
  while(true) {
    vTaskDelay(post_period*1000);
    if (!sendCommand("CGREG?", 5)) continue;
    if (!sendCommand("CGNSINF", 5)) continue;
    if (flagREG) {
      sendCommand("CGNSINF",5); //reads GLONASS data
      if (flagGNS){
        int timeout;
        sendCommand("SAPBR=1,1", 5);
        sendCommand("SAPBR=2,1", 5);
        sendCommand("HTTPINIT", 5);
        sendCommand("HTTPPARA=\"CID\",1", 5);
        sendCommand("HTTPPARA=\"CONTENT\",\"application/json\"", 5);
        sendCommand("HTTPPARA=\"URL\",\"http://sensor-network-lora.herokuapp.com/api/sensors\"", 5);
        buildJSON();
        char request_buffer[20] = "";
        sprintf(request_buffer, "AT+HTTPDATA=%d,1000\r", strlen(post_buffer));
        Serial1.print(request_buffer);
        flagHTTPACT = false;
        flagDOWNLOAD = false;
        timeout = 100;
        while (timeout > 0) {
          if (flagDOWNLOAD) {
            Serial1.println(post_buffer);
            Serial.println(post_buffer);
            timeout = 0;
          }
          timeout--;
          vTaskDelay(100);
        }
        sendCommand("HTTPACTION=1",10);
        timeout = 100;
        while (timeout > 0) {
          if (flagHTTPACT) {
            sendCommand("HTTPREAD",10);
            sendCommand("HTTPTERM",10);
            sendCommand("SAPBR=0,1",10);
            timeout=0;
          }
          timeout--;
          vTaskDelay(100);
        }
        vTaskDelay(10000);
      } else {
        continue;
      }
    } else {
      continue;
    }
  }
}

static void task_readModem(void *pvParameters) {
  while (true) {
    while(Serial1.available()) {
      char c = Serial1.read();
      Serial.print(c);
      modem_buffer[modem_i]=c;
      modem_i++;
      if (modem_i > modem_buffer_size) modem_i=0;
      if ((modem_i >= 2) && ((c == '\n') || (c == '\n'))) {
        modem_buffer[modem_i]='\0';
        if (memcmp("OK",       modem_buffer, 2)==0) flagOK=true;
        if (memcmp("ERROR",    modem_buffer, 4)==0) flagERROR=true;
        if (memcmp("+CGR",     modem_buffer, 4)==0) procCGR();
        if (memcmp("+CGN",     modem_buffer, 4)==0) procCGN();
        if (memcmp("+HTTPACT", modem_buffer, 8)==0) flagHTTPACT=true;
        if (memcmp("DOWNLOAD", modem_buffer, 8)==0) flagDOWNLOAD=true;
        modem_i=0;
        for(int i=0; i<modem_buffer_size; i++) modem_buffer[i]=0; 
      }

    }
    vTaskDelay(10);
  }
}

static void task_sensors(void *pvParameters) {
  while(true) {
    blink();
    readTempHum();
    readPM2();
    readPM1();
    // displayValues();
  }
}

void setup() {
  pinMode(pkey_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  Serial.begin(115200);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  dht.begin();
  Serial.println("Start >>>");
  
  xTaskCreate(
    task_modem,"TModem",
    256,NULL,2,NULL
  );
  
  xTaskCreate(
    task_sensors,"TSensors",
    256,NULL,2,NULL
  );
  
  xTaskCreate(
    task_readModem,"TReadModem",
    256,NULL,1,NULL
  );
  vTaskStartScheduler();
  while(true);
}

void loop() {
}


