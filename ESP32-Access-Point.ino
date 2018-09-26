#include <WiFi.h>

// The ESP32 default IP address is 192.168.4.1
// To set a different static IP see:
// https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiClientStaticIP/WiFiClientStaticIP.ino

// password should be equal or longer than 8 chars
// for enabling this configuration
const char *ssid = "BonaDrone";
const char *password = "12345678";

// Set web server port number to 80
WiFiServer server(80);

void setup() 
{
    Serial.begin(115200);

    // Connect to Wi-Fi network with SSID and password
    // Remove the password parameter, if you want the AP (Access Point) to be open
    Serial.print("Setting AP (Access Point)…");
    WiFi.softAP(ssid, password);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
  
    server.begin();
}


void loop()
{
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client)                               // If a new client connects, 
  {
      Serial.println("New Client");         // print a message out in the serial port
      while (client.connected())            // loop while the client's connected
      {
          while (client.available() > 0)    // if there's bytes to read from the client,
          {
            char c = client.read();         // read a byte, then
            Serial.write(c);                // print it out the serial monitor
            client.write(c);                // Send byte back
          }
      }
      Serial.println();
      // Close the connection
      client.stop();
      Serial.println("Client disconnected");
  }
}
