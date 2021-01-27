# 1 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
# 1 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
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

# 18 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino" 2
# 19 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino" 2






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
const char key_humidity[] = "19";
const char key_latitude[] = "21";
const char key_longitude[] = "22";
const char key_pm1_value[] = "23";
const char key_pm2_value[] = "24";
const char key_timestamp[] = "26";
char post_buffer[120];

DHT dht(PB9, 21 /**< DHT TYPE 21 */);
// variables to manage PM1
const int pm1_buff_size = 15;
int pm1_i = 0;
char pm1_buff[pm1_buff_size];
bool pm1_ok = 0x0;

// variables to manage PM2
const int pm2_buff_size = 40;
int pm2_i = 0;
char pm2_buff[pm2_buff_size];
bool pm2_ok = 0x0;

// Modem management variables
volatile bool flagOK = 0x0;
volatile bool flagERROR = 0x0;

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
  if (pm1_buff[pm1_i] == 0xC0 &&
      pm1_buff[pm1_i-1] == 0xAA &&
      pm1_buff[pm1_i-2] == 0xAB) {
    pm1_value = (pm1_buff[2]*256 + pm1_buff[1])/10;
    pm1_i=0;
    pm1_ok=0x1;
  }
  pm1_i++;
  if (pm1_i>=pm1_buff_size) {pm1_i=0; pm1_ok=0x0;}
}

void processingPM2Data(char c) {
  // Serial.write(c);
  pm2_buff[pm2_i]=c;
  if (pm2_buff[pm2_i] == 0x1C &&
      pm2_buff[pm2_i-1] == 0x00 &&
      pm2_buff[pm2_i-2] == 0x4D &&
      pm2_buff[pm2_i-3] == 0x42) {
    pm2_value = pm2_buff[1]*256 + pm2_buff[2];
    pm2_i=0;
    pm2_ok=0x1;
  }
  pm2_i++;
  if (pm2_i>=pm2_buff_size) {pm2_i=0; pm2_ok=0x0;}
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
  digitalWrite(PA0,0x0);
  vTaskDelay(1000);
  digitalWrite(PA0,0x1);
}

void blink() {
  digitalWrite(PC13, 0x1); vTaskDelay(500);
  digitalWrite(PC13, 0x0); vTaskDelay(500);
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
    if (flagOK || flagERROR) return 0x1;
    vTaskDelay(100);
  }
  return 0x0;
}

bool sim808Init()
{
  while(0x1)
  {
    if(sendCommand("GSN",5) && sendCommand("CGNSPWR=1",5))
    break;
  }

}

static void task_modem(void *pvParameters) {
  Serial1.begin(9600);
  Serial1.println("Start >>>");
  while(0x1) {
    vTaskDelay(500);
    Serial1.println("Succcess");
  }
}

static void task_sensors(void *pvParameters) {
  pinMode(PC13, OUTPUT);
  Serial.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  dht.begin();
  Serial.println("Start >>>");
  while(0x1) {
    blink();
    readTempHum();
    readPM2();
    readPM1();
    displayValues();
    Serial.println();
  }
}

void setup() {
  xTaskGenericCreate( ( task_modem ), ( "TModem" ), ( 64 ), ( __null ), ( 2 ), ( __null ), ( __null ), ( __null ) )


   ;
  xTaskGenericCreate( ( task_sensors ), ( "TSensors" ), ( 256 ), ( __null ), ( 1 ), ( __null ), ( __null ), ( __null ) )


   ;
  vTaskStartScheduler();
  while(0x1);
}

void loop(){
  while (0x1){
    ;
  }

}
