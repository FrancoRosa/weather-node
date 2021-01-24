# 1 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
# 1 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
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
# 17 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino" 2
# 25 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
DHT dht(PB0, 21 /**< DHT TYPE 21 */);

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
  digitalWrite(7,0x0);
  delay(1000);
  digitalWrite(7,0x1);
}

void setup(){
  pinMode(PC13, OUTPUT);

  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  dht.begin();
}

void loop(){

  delay(500);
  digitalWrite(PC13, 0x1);
  delay(500);
  digitalWrite(PC13, 0x0);
  Serial.println("Serial_0");
  Serial1.println("Serial_1");
  Serial2.println("Serial_2");
  Serial3.println("Serial_3");
  readTempHum();
}
