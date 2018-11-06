#include <WiFi.h>

// The ESP32 default IP address is 192.168.4.1
// To set a different static IP see:
// https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiClientStaticIP/WiFiClientStaticIP.ino

// password should be equal or longer than 8 chars
// for enabling this configuration
const char *ssid = "Bonadrone";
const char *password = "12345678";

// Set web server port number to 80
WiFiServer server(80);

void setup() 
{
    Serial.begin(115200);

    // Connect to Wi-Fi network with SSID and password
    // Remove the password parameter, if you want the AP (Access Point) to be open
    //Serial.print("Setting AP (Access Point)…");
    WiFi.softAP(ssid, password);

    IPAddress IP = WiFi.softAPIP();
    //Serial.print("AP IP address: ");
    //Serial.println(IP);
  
    server.begin();
}

void loop()
{
  bool waitingResponse = false;
  bool writeDataWifi = false;
  uint8_t dataWifi;
  WiFiClient wifi = server.available();   // Listen for incoming clients
  
  if (wifi)                               // If a new client connects, 
  {   
      while (wifi.connected())            // loop while the client's connected
      {
        while (wifi.available() > 0 && !waitingResponse) {
           if(writeDataWifi)
           {
              Serial.write(dataWifi);
              writeDataWifi = false;
           }
           dataWifi = wifi.read(); 
           uint8_t state = parse(dataWifi);
           uint8_t _direction;
          
           if(state == 0)
           {
              if(_direction == 0)
              {
                writeDataWifi = true;
                waitingResponse = true;
              }
              else
              {
                Serial.write(dataWifi);
              }
           }
           else if(state == 1)
           {
              _direction = 1;
              Serial.write(dataWifi);
           }
           else if(state == 2)
           {
              _direction = 0;
              Serial.write(dataWifi);
           }
           else if(state == 3)
           {
              Serial.write(dataWifi);
           }
        }
        delay(500);
        while (Serial.available() > 0 || waitingResponse) 
        {
          uint8_t dataSTM = Serial.read();
          wifi.write(dataSTM);
          waitingResponse = false;
        }
      }
      
      // Close the connection
      wifi.stop();
  }
}

// returns true if reboot request, false otherwise
uint8_t parse(uint8_t c)
{
    uint8_t _state;
    switch (c) 
    {
        case 36:
            //START
            _state = 0;
            break;

        case 62:
            //CMD
            _state = 1;
            break;
            
        case 60:
            //REQ
            _state = 2;
            break;
            
        default:
            //OTHER
            _state = 3;
     }
     
     return _state;
} 
