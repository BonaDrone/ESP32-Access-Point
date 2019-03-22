#include <WiFi.h>

// The ESP32 default IP address is 192.168.4.1
// To set a different static IP see:
// https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiClientStaticIP/WiFiClientStaticIP.ino

// password should be equal or longer than 8 chars
// for enabling this configuration
const char *ssid = "Mosquito90_A";
const char *password = "12345678";

// Set web server port number to 80
WiFiServer server(80);

// Battery
typedef union
{
  float number;
  uint8_t bytes[4];
} floatUnion_t;
const int BATTERY_PIN = 33; // specific to BonaDrone FC board
floatUnion_t batteryVoltage;



void measureBatteryVoltage()
{
  const float voltageDivider = 1.15;
  const float voltageResolution = 4096.0;
  const float voltageMaximum = 12.60;
  
  static float batLast = 0.0;
  static float batTrend = 0.0;
  static int ii = 0;
  
  float sFact_1 = 0.2;
  float sFact_2 = 0.8;
  
  int meas = analogRead(BATTERY_PIN);
  float batMeas = (float)meas/voltageResolution*voltageMaximum;

  float bat = 0.0;
  if (ii == 0)
  {
    bat = batMeas;
    ++ii;
  }
  else if (ii == 1)
  {
    bat = batMeas;
    batTrend = bat - batLast;
    ++ii;
  }
  else
  {
    bat = sFact_1*batMeas + (1 - sFact_1)*(batLast + batTrend);
    batTrend = sFact_2*(bat - batLast) + (1 - sFact_2)*batTrend;
  }
  batLast = bat;
  
  batteryVoltage.number = bat*voltageDivider;
}

void sendBatteryVoltage()
{
  char b1 = '$';
  char b2 = 'M';
  char b3 = '<';
  uint8_t N = 4;
  byte T = 0xE4;

  uint8_t check;
  check = N ^ T;
  for (int ii = 0; ii<4; ++ii)
  {
    check = check ^ batteryVoltage.bytes[ii];
  } 
  
  Serial.write(b1);
  Serial.write(b2);
  Serial.write(b3);
  Serial.write(N);
  Serial.write(T);
  Serial.write(batteryVoltage.bytes[0]);
  Serial.write(batteryVoltage.bytes[1]);
  Serial.write(batteryVoltage.bytes[2]);
  Serial.write(batteryVoltage.bytes[3]);
  Serial.write(check);
}

void checkBattery()
{
  static float lastTime = 0.0;

  if (micros()/1.0e6 - lastTime > 1)
  {
    measureBatteryVoltage();
    sendBatteryVoltage();
    lastTime = micros()/1.0e6;
  }
}

void setup() 
{
    Serial.begin(115200);

    // Connect to Wi-Fi network with SSID and password
    // Remove the password parameter, if you want the AP (Access Point) to be open
    WiFi.softAP(ssid, password);

    IPAddress IP = WiFi.softAPIP();
  
    server.begin();
    
    // Battery
    pinMode(BATTERY_PIN, INPUT);
}


void loop()
{
  
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client)                               // If a new client connects, 
  {
      while (client.connected())            // loop while the client's connected
      {
          while (client.available() > 0)    // if there's bytes to read from the client,
          {
            char c = client.read();         // read a byte, then
            Serial.write(c);                // send it out through the serial
          }
          while (Serial.available() > 0)    // if there's bytes to read from the serial
          {
            char c = Serial.read();         // read a byte, then
            client.write(c);                // send it to the client
          }
          checkBattery();
      }                                     // When the client disconnects
      client.stop();
  }
  else
  {
    checkBattery();
  }
}
