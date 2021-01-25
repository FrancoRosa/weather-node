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

const int pm1_buff_size = 15;
volatile int pm1_i = 0;
volatile int pm1_value = 0;
char pm1_buff[pm1_buff_size];
volatile bool pm1_ok = 0x0;

const int pm2_buff_size = 40;
volatile int pm2_i = 0;
volatile int pm2_value = 0;
char pm2_buff[pm2_buff_size];
volatile bool pm2_ok = 0x0;

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
  if (pm2_buff[pm2_i] == 0x1C &&
      pm2_buff[pm2_i-1] == 0x00 &&
      pm2_buff[pm2_i-2] == 0x4D &&
      pm2_buff[pm2_i-3] == 0x42) {
    pm2_value = pm2_buff[7]*256 + pm2_buff[8];
    pm2_i=0;
    pm2_ok=0x1;
  }
  pm2_i++;
  if (pm2_i>=pm2_buff_size) {pm2_i=0; pm2_ok=0x0;}
}

void processingPM1Data(char c) {
  // Serial.write(c);
  pm1_buff[pm1_i]=c;
  if (pm1_buff[pm1_i] == 0xC0 &&
      pm1_buff[pm1_i-1] == 0xAA &&
      pm1_buff[pm1_i-2] == 0xAB) {
    pm1_value = pm1_buff[2]*256 + pm1_buff[1];
    pm1_i=0;
    pm1_ok=0x1;
  }
  pm1_i++;
  if (pm1_i>=pm1_buff_size) {pm1_i=0; pm1_ok=0x0;}
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
  digitalWrite(7,0x0);
  delay(1000);
  digitalWrite(7,0x1);
}

void blink() {
  digitalWrite(PC13, 0x1); delay(500);
  digitalWrite(PC13, 0x0); delay(500);
}

void setup() {
  pinMode(PC13, OUTPUT);
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
  showBuffers();
  displayValues();
}
