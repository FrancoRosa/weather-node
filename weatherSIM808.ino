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

volatile float temperature = 0;
volatile float humidity = 0;

void readTempHum() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  Serial.print("temp: ");
  Serial.print(temperature);
  Serial.print(", hum: ");
  Serial.print(humidity);
  Serial.println();
}

void processingModeData(char c) {
  Serial.write(c);
}

void mPower(){
  digitalWrite(m_pw,LOW);
  delay(1000);
  digitalWrite(m_pw,HIGH);
}

void setup(){
  pinMode(led_pin, OUTPUT);

  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  dht.begin();
}

void loop(){
  
  delay(500);
  digitalWrite(led_pin, HIGH);
  delay(500);
  digitalWrite(led_pin, LOW);
  Serial.println("Serial_0");
  Serial1.println("Serial_1");
  Serial2.println("Serial_2");
  Serial3.println("Serial_3");
  readTempHum();
}

