// ESP32 reset is L476 pin 42
// EESP32 IO0 is L476 pin 41
const int RST = 42;
const int GPIO0 = 41;

const int LED_R = 25;
const int LED_G = 26;
const int LED_B = 38;

void setup() 
{
    // begin communications between ESP32 <-> STM32L476 (Serial4)
    // and STM32L476 <-> PC (Serial)
    Serial.begin(115200);
    Serial4.begin(115200);
    // PIN MODES
    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(LED_B, OUTPUT);

    digitalWrite(LED_R, HIGH);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, HIGH);
}

void loop() 
{
    String cmd = "";
    
    cmd = Serial.readString();
    cmd.replace("\n","");
    
    if (cmd == "Idle")
    {
      digitalWrite(LED_R, LOW);
      digitalWrite(LED_G, HIGH);
      digitalWrite(LED_B, HIGH);
    } 
    else if (cmd == "ByPass")
    {
      digitalWrite(LED_R, HIGH);
      digitalWrite(LED_G, LOW);
      digitalWrite(LED_B, HIGH);
      while (1 > 0)
      {
        while (Serial4.available())
          Serial.write(Serial4.read()); 
      }
    }
    else if (cmd == "LoadESP")
    {
      digitalWrite(LED_R, HIGH);
      digitalWrite(LED_G, HIGH);
      digitalWrite(LED_B, LOW);

      pinMode(RST, OUTPUT);
      pinMode(GPIO0, OUTPUT);
      
      digitalWrite(GPIO0, LOW);
      // Reset ESP32
      digitalWrite(RST, LOW);
      delay(100);
      digitalWrite(RST, HIGH);
      delay(100);
      
      while (1 > 0)
      {
        while ( Serial4.available() ) { Serial.write(Serial4.read() ); }
        while ( Serial.available() ) { Serial4.write(Serial.read() ); }
      } 
    }
}
