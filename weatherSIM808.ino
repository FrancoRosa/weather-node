#define led 13
void setup(){
  pinMode(led, OUTPUT);
}

void loop(){
  delay(1000);
  digitalWrite(led, HIGH);
  delay(1000);
  digitalWrite(led, LOW);
}
