void setup() 
{
    Serial.begin(115200);
    pinMode(33, INPUT);
}


void loop()
{
  Serial.println(analogRead(33));
}
