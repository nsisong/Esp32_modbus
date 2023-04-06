
// #ifdef ESP8266
//  #include <ESP8266WiFi.h>
// #else //ESP32
 #include <WiFi.h>
// #endif
#include <ModbusIP_ESP8266.h>
#include <WiFiManager.h>

// Modbus Registers Offsets
const int TEST_HREG = 100;


//ModbusIP object
ModbusIP mb;
  
void setup() {
  Serial.begin(115200);
  WiFiManager wm;
  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  // wm.resetSettings();

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("Humidity sensor","password"); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("connected...yeey :)");
  }
 
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mb.server(502);
  mb.addHreg(TEST_HREG, 0xABCD);
}
 
void loop() {
   //Call once inside loop() - all magic here
   mb.task();
   delay(10);
}