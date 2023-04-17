
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ModbusIP_ESP8266.h>
#include <WiFiManager.h>

//Modbus Registers Offsets
const int LED_COIL            = 4001;
const int SWITCH_ISTS         = 4002;
const int HUMIDITY_IREG       = 4003;
const int TEMPERATURE_IREG    = 4004;
const int SENSOR_TYPE         = 4005;

//Used Pins
#define DHTPIN D2       // Digital pin connected to the DHT sensor 
const int ledPin = D1;  //GPIO2
const int switchPin = 0; //GPIO0

//Set the sensor used
#define DHTTYPE    DHT11     // DHT 11

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;


//ModbusIP object
ModbusIP mb;

  
void setup() {
  Serial.begin(115200);

  // Initialize device.
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));

  
  // Setup wifi details
  WiFiManager wm;
  // to reset settings - wipe stored credentials for testing these are stored by the esp library
  // uncomment the function below
  // wm.resetSettings();

  // Else, Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  bool res;
  res = wm.autoConnect("Humidity sensor","password"); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("connected...yeey :)");
  }
 
  //These are captured in the WIFI manager
  // Serial.println("");
  // Serial.println("WiFi connected");  
  // Serial.println("IP address: ");
  // Serial.println(WiFi.localIP());

  //Start the Modbus server
  mb.server();

  pinMode(ledPin, OUTPUT);
  mb.addCoil(LED_COIL);
  pinMode(switchPin, INPUT);
  // Add SWITCH_ISTS register - Use addIsts() for digital inputs
  mb.addIsts(SWITCH_ISTS);
  mb.addIreg(TEMPERATURE_IREG);
  mb.addIreg(HUMIDITY_IREG);

  
}
 
void loop() {
   //Call once inside loop() - all magic here
   mb.task();


  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }

   //Attach ledPin to LED_COIL register
   digitalWrite(ledPin, mb.Coil(LED_COIL));
  //  Serial.println(mb.Coil(LED_COIL));
   //Attach switchPin to SWITCH_ISTS register
   mb.Ists(SWITCH_ISTS, digitalRead(switchPin));
   mb.Ireg(HUMIDITY_IREG, event.relative_humidity);
   delay(10);
}