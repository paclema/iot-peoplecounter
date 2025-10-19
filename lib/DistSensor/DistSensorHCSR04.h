#ifndef DistSensorHCSR04_H
#define DistSensorHCSR04_H

#include <Arduino.h>
#include <DistSensor.h>


// Pinout with D1 mini:
//--------------------
//
// Echo -- D8
// Trigger -- D7
// VCC -- 5V
// GND -- GND

#ifdef ESP32
  #define echoPin (8)   // Previously GPIO_ID_PIN(8)
  #define trigPin (7)   // Previously GPIO_ID_PIN(7)
#elif defined(ESP8266)
  #define echoPin D8
  #define trigPin D7
#endif

class DistSensorHCSR04: public DistSensor {

public:

  long duration; // Duration used to calculate distance
  long previousSensorReadMillis = 0;

  String distance_mode;
  String distance_mode_options[JSON_MAX_SIZE_LIST]; //["Short", "Medium", "Long", "Unknown"]


  DistSensorHCSR04(void);
  ~DistSensorHCSR04();


  void setup(void);
  void disableSensor(void);
  bool sensorRead(float *distance);
  String getSensorRange(void);
  void printSensorStatus(void);

  void free() { if (this->debug) Serial.println("\t\t\tfree DistSensorHCSR04");  delete this; }

  void parseWebConfig(JsonObjectConst configObject);


};
#endif
