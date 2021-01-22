#include <Arduino.h>
#line 1 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
#line 1 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
#define led 13
#define m_tx 2
#define m_rx 3
#define m_pw 7

#include <SoftwareSerial.h>
SoftwareSerial mSerial(m_rx, m_tx);

#line 9 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void processingModeData(char c);
#line 13 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void mPower();
#line 19 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void setup();
#line 26 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void loop();
#line 9 "c:\\Users\\fx\\Upwork\\weather-node\\weatherSIM808.ino"
void processingModeData(char c) {
  Serial.write(c);
}

void mPower(){
  digitalWrite(m_pw,LOW);
  delay(1000);
  digitalWrite(m_pw,HIGH);
}

void setup(){
  pinMode(led, OUTPUT);
  // pinMode(m_pw, OUTPUT);
  Serial.begin(19200);
  mSerial.begin(19200);
}

void loop(){
  delay(1000);
  digitalWrite(led, HIGH);
  delay(1000);
  digitalWrite(led, LOW);
  while(mSerial.available()) {
    processingModeData(mSerial.read());
  }
}


