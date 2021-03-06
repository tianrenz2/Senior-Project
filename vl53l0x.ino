#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }
  
  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
}


void loop() {
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    int dist = measure.RangeMilliMeter;
    int range = -1;
    if(0 < dist && dist < 50){
      range = 1;
    }else if(50 < dist && dist < 100){
      range = 2;
    }
    if(range > 0){
        Serial.println(dist);
        Serial.print("Range: "); Serial.println(range);
    }
  } 
    
  delay(100);
}
