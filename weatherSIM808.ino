#define led 13
#define m_tx 2
#define m_rx 3
#define m_pw 7

#include <SoftwareSerial.h>
SoftwareSerial mSerial(m_rx, m_tx);

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
  Serial.begin(115200);
  mSerial.begin(9600);
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

