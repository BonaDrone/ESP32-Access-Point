void setup() {
 Serial.begin(115200);
 Serial4.begin(115200);
}

void loop() {
  while ( Serial4.available() ) { Serial.write(Serial4.read() ); }
  while ( Serial.available() ) { Serial4.write(Serial.read() ); }
}
