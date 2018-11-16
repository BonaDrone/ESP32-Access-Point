#include <WiFi.h>

const int BATTERY_PIN = 33; // specific to BonaDrone FC board
bool unknownBatteryType = true;
int _lowBattery = 0;

// The ESP32 default IP address is 192.168.4.1
// To set a different static IP see:
// https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiClientStaticIP/WiFiClientStaticIP.ino

// password should be equal or longer than 8 chars
// for enabling this configuration
const char *ssid = "Mosquito150";
const char *password = "12345678";

// Set web server port number to 80
WiFiServer server(80);

void checkSignal(bool hadClient)
{
    if(hadClient)
    {
        // Quick and dirty hack to send lost signal message
        // The message ID is 223 and the full message is
        // $M<\x01\xdf\x01\xdf, where \x01 -> 1 and \xdf -> 223
        Serial.write(36);
        Serial.write(77);
        Serial.write(60);
        Serial.write(1);
        Serial.write(223);
        Serial.write(1);
        Serial.write(223);
    }
}

void checkBattery(void)
{
    if (analogRead(BATTERY_PIN) < _lowBattery)
    {
        // trigger low battery message (currently Lost signal)
        // The message ID is 223 and the full message is
        // $M<\x01\xdf\x01\xdf, where \x01 -> 1 and \xdf -> 223
        Serial.write(36);
        Serial.write(77);
        Serial.write(60);
        Serial.write(1);
        Serial.write(223);
        Serial.write(1);
        Serial.write(223);
    }
}

void setup() 
{
    Serial.begin(115200);
    pinMode(BATTERY_PIN, INPUT);
    // Wait until type of battery (number of cells) is received
    while (unknownBatteryType)
    {
      // XXX To be implemented
      unknownBatteryType = false;
      _lowBattery = 0;
    }
    // Connect to Wi-Fi network with SSID and password
    // Remove the password parameter, if you want the AP (Access Point) to be open
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
    server.begin();
}


void loop()
{
    checkBattery();                           // Check battery status
    
    bool _hadClient = false;                  // Reset had client flag
    WiFiClient client = server.available();   // Listen for incoming clients

    if (client)                               // If a new client connects, 
    {
        while (client.connected())            // loop while the client's connected
        {
            _hadClient = true;
            if (client.available() > 0)       // if there's bytes to read from the client,
            {
              char c = client.read();         // read a byte, then
              Serial.write(c);                // send it out through the serial
            }
            if (Serial.available() > 0)       // if there's bytes to read from the serial
            {
              char c = Serial.read();         // read a byte, then
              client.write(c);                // send it to the client
            }
            checkBattery();                   // and check the battery status
        }                                     // When the client disconnects
        client.stop();                        // close the connection
        checkSignal(_hadClient);              // and send lost signal message
    }
}
