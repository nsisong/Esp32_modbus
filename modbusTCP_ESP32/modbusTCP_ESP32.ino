
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ModbusIP_ESP8266.h>
#include <WiFiManager.h>

//Modbus Registers Offsets
const int BUZZER_COIL         = 4001;
const int HUMIDITY_IREG       = 4003;
const int TEMPERATURE_IREG    = 4004;
const int SENSOR_TYPE         = 4005;

//Used Pins
#define DHTPIN        D2       // Digital pin connected to the DHT sensor 
const int buzzerPin = D1;      // GPIO2
const int switchPin = 0;       // GPIO0

//Set the sensor used
#define DHTTYPE    DHT11      // DHT 11

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;


//ModbusIP object
ModbusIP mb;

  
void setup() {
  Serial.begin(115200);

  // Initialize device.
  dht.begin();

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

  pinMode(buzzerPin, OUTPUT);
  mb.addCoil(BUZZER_COIL);
  pinMode(switchPin, INPUT);
  // Add TEMPERATURE / HUMIDITY register - Use addIreg() for analog inputs
  mb.addIreg(TEMPERATURE_IREG);
  mb.addIreg(HUMIDITY_IREG);
  mb.addIreg(SENSOR_TYPE);

}
 
void loop() {
  //Call once inside loop() - all magic here
  mb.task();
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  mb.Ireg(SENSOR_TYPE, 11);

  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  mb.Ireg(TEMPERATURE_IREG, event.temperature);

  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  mb.Ireg(HUMIDITY_IREG, event.relative_humidity);


  //Attach buzzerPin to BUZZER_COIL register
  digitalWrite(buzzerPin, mb.Coil(BUZZER_COIL));
  
  delay(10);
}