#include <OSCMessage.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <i2c_touch_sensor.h>
#include <mpr121.h>

i2ctouchsensor touchsensor; // keep track of 4 pads' states
long previousMillis = 0;
long interval = 10;
const unsigned int localPort = 8888; 
const unsigned int outPort = 9999;
const char *ssid = "Devansh's WiFi Network";
const char *pass = "akdk9597";
WiFiUDP Udp;    
const IPAddress outIp(192,168,0,100);  
unsigned int sampleIntervalMilliseconds = 1;

int states[12];

void setup() 
  {    
    Wire.begin(); // needed by the GroveMultiTouch lib     
    touchsensor.initialize(); // initialize the feelers  // initialize the containers     
    
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
  }
  
void loop()
{     
    unsigned long currentMillis = millis();
    
    if(currentMillis - previousMillis > interval)
    {
       OSCMessage msg("/Key");
       
       previousMillis = currentMillis;
       touchsensor.getTouchState();

       for (int i=0;i<12;i++)
        {
          if (touchsensor.touched&(1<<i))
          {
            states[i] = 1;
          }
          else
          {
            states[i] = 0;
          }
          
          msg.add(states[i]);
        }

      Udp.beginPacket(outIp, outPort);
      msg.send(Udp);
      Udp.endPacket();
      msg.empty();
   }
      
}  
