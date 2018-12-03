#include <WiFi.h>

// The ESP32 default IP address is 192.168.4.1
// To set a different static IP see:
// https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiClientStaticIP/WiFiClientStaticIP.ino

// password should be equal or longer than 8 chars
// for enabling this configuration
const char *ssid = "Mosquito90";
const char *password = "12345678";

// Set web server port number to 80
WiFiServer server(80);

void sendLostSignal(bool hadClient)
{
    if(hadClient)
    {
        // Quick and dirty hack to send lost signal message
        // The message ID is 223 and the full message is
        // $M<\x01\xe2\x01\xe2, where \x01 -> 1 and \xe2 -> 226
        Serial.write(36);
        Serial.write(77);
        Serial.write(60);
        Serial.write(1);
        Serial.write(226);
        Serial.write(1);
        Serial.write(226);
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
}


void loop()
{
    bool _hadClient = false;
    WiFiClient client = server.available();   // Listen for incoming clients

    if (client)                               // If a new client connects, 
    {
        while (client.connected())            // loop while the client's connected
        {
            _hadClient = true;
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
        }                                     // When the client disconnects
        client.stop();                        // close the connection
        sendLostSignal(_hadClient);           // and send lost signal message
    }
}
