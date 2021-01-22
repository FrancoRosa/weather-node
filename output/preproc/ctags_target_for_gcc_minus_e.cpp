# 1 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
# 1 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"





# 7 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino" 2
SoftwareSerial mSerial(3, 2);

void processingModeData(char c) {
  Serial.write(c);
}

void mPower(){
  digitalWrite(7,0x0);
  delay(1000);
  digitalWrite(7,0x1);
}

void setup(){
  pinMode(13, 0x1);
  // pinMode(m_pw, OUTPUT);
  Serial.begin(19200);
  mSerial.begin(19200);
}

void loop(){
  delay(1000);
  digitalWrite(13, 0x1);
  delay(1000);
  digitalWrite(13, 0x0);
  while(mSerial.available()) {
    processingModeData(mSerial.read());
  }
}
