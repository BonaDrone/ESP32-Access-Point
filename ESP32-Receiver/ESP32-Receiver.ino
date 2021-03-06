#include <WiFi.h>

const int BATTERY_PIN = 33; // specific to BonaDrone FC board
// Low battery depends on the number of cells he battery has.
// 1S operative range (3.5V-4.2V)
// 2S operative range (7V-8.3V)
// 3S operative range (10.5V-12.5V)
// Since operative ranges do not overlap, we can determine the type
// of battery being used from the first reading and set the low
// battery limit accordingly. The values defined below assume that
// 11.0V = 3860 and are rounded to the upper ten
const int LOW_BATTERY_1S = 1230;  // 3.5V for 1S batteries
const int LOW_BATTERY_2S = 2450;  // 7.0V for 2S batteries
const int LOW_BATTERY_3S = 3670;  // 10.5V for 3S batteries

int _lowBattery = 0;

// Monitor battery status
int const BATTERY_SAMPLES = 150;
int BATTERY_HISTORY[BATTERY_SAMPLES];
int batteryIndex = 0;
int batterySum = 0;
// Battery sampling 
int sampleFrequency = 10; // In Hz
unsigned long lastSampleTime = millis();
unsigned long currentTime = millis();

void setLowBatteryLimit(void)
{
    int batteryStatus = analogRead(BATTERY_PIN);
    if (batteryStatus > LOW_BATTERY_3S)
    {
        _lowBattery = LOW_BATTERY_3S;
        return;
    }
    else if (batteryStatus > LOW_BATTERY_2S)
    {
        _lowBattery = LOW_BATTERY_2S;
        return;
    }
    else if (batteryStatus > LOW_BATTERY_1S)
    {
        _lowBattery = LOW_BATTERY_1S;
        return;
    }
    // XXX What if none of the above criteria is met?
}

// Sampling at 10Hz (10 samples in one second) yields
// 150 samples in 15 seconds. If the average of the 
// samples goes below the low battery threshold then
// trigger low battery
void checkBattery(void)
{
    // update battery reading
    currentTime = millis();
    if (currentTime - lastSampleTime > 1000.0f/sampleFrequency)
    {
        int batteryStatus = analogRead(BATTERY_PIN);
        batterySum = batterySum - BATTERY_HISTORY[batteryIndex] + batteryStatus;
        BATTERY_HISTORY[batteryIndex] = batteryStatus;
        batteryIndex = (batteryIndex + 1) % BATTERY_SAMPLES;
        // Avoid sending low battery message if no battery is
        // connected (analogRead = _lowBattery = 0)
        if (batterySum / BATTERY_SAMPLES < _lowBattery)
        {
            // XXX implement low battery message if we want to
            // handle it different than a lost signal
            // trigger low battery message (currently Lost signal)
            // The message ID is 223 and the full message is
            // $M<\x01\xe0\x01\xe0, where \x01 -> 1 and \xe0 -> 224
            Serial.write(36);
            Serial.write(77);
            Serial.write(60);
            Serial.write(1);
            Serial.write(224);
            Serial.write(1);
            Serial.write(224);
        }
        lastSampleTime = currentTime;
    }
}

// The ESP32 default IP address is 192.168.4.1
// To set a different static IP see:
// https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiClientStaticIP/WiFiClientStaticIP.ino

// password should be equal or longer than 8 chars
// for enabling this configuration
const char *ssid = "Mosquito90";
const char *password = "12345678";
// Set web server port number to 80
WiFiServer server(80);

void checkSignal(bool hadClient)
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
    pinMode(BATTERY_PIN, INPUT);
    // Determine type of battery (number of cells)
    setLowBatteryLimit();
    int batteryOnStartup = analogRead(BATTERY_PIN);
    for (int i=0;i<BATTERY_SAMPLES;++i)
    {
      BATTERY_HISTORY[i] = batteryOnStartup;
      batterySum += batteryOnStartup;
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
