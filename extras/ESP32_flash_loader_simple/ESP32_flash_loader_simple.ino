// ESP32 reset is L476 pin 42
// EESP32 IO0 is L476 pin 41
int RST = 42;
int GPIO0 = 41;

void setup() 
{
    // begin communications between ESP32 <-> STM32L476 (Serial4)
    // and STM32L476 <-> PC (Serial)
    Serial.begin(115200);
    Serial4.begin(115200);
    pinMode(RST, OUTPUT);
    pinMode(GPIO0, OUTPUT);
    // Force the ESP32 to enter boot mode: GPIO0 to 0V and reset ESP32
    digitalWrite(GPIO0, LOW);
    // Reset ESP32
    digitalWrite(RST, LOW);
    delay(100);
    digitalWrite(RST, HIGH);
}

void loop() 
{
    while ( Serial4.available() ) { Serial.write(Serial4.read() ); }
    while ( Serial.available() ) { Serial4.write(Serial.read() ); }
}