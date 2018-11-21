#include "Wire.h"
#include "Adafruit_VL6180X.h"

#include <Adafruit_Sensor.h>
#define TCAADDR 0x70

extern "C" {
  #include "utility/twi.h" // from Wire library, so we can do bus scanning
}
#define TCAADDR 0x70

Adafruit_VL6180X vl1;
Adafruit_VL6180X vl2;

bool left = false;
bool right = false;

unsigned long lastTimeofLeft = millis();
unsigned long lastTimeofRight = millis();


void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}
// standard Arduino setup()
void setup()
{
  while (!Serial);
  delay(1000);
  Wire.begin();
  Serial.begin(115200);
  Serial.println("\nTCAScanner ready!");
  for (uint8_t t=0; t<8; t++) {
    tcaselect(t);
    Serial.print("TCA Port #"); Serial.println(t);
    for (uint8_t addr = 0; addr<=127; addr++) {
      if (addr == TCAADDR) continue;
      uint8_t data;
      if (! twi_writeTo(addr, &data, 0, 1, 1)) {
        if(t == 1){
          vl1 = Adafruit_VL6180X();
          if(vl1.begin()){
            Serial.print("Sensor1 has begun\n");
          }
        }else if(t == 2){
          vl2 = Adafruit_VL6180X();
          if(vl2.begin()){
            Serial.print("Sensor2 has begun\n");
          }else{
            Serial.print("Sensor2 failed\n");
          }
        }

        Serial.print("Found I2C 0x"); Serial.println(addr,HEX);
      }
    }
  }
    Serial.println("\ndone");
}

int determinSide(uint8_t s1, uint8_t s2){
//  Serial.print(s1);
//  Serial.print(",");
//  Serial.println(s2);

  return (((float)s2)/((float)(s1 + s2))) * 100;
}

void loop()
{
  /* Get a new sensor event */
  sensors_event_t event;
  tcaselect(1);
//  mag1.getEvent(&event);
  /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
  float lux1 = vl1.readLux(VL6180X_ALS_GAIN_5);
  uint8_t range1 =  vl1.readRange();
  uint8_t status1 = vl1.readRangeStatus();
//  Serial.print("lux1: "); Serial.print(lux1); Serial.print(" ");
  if (status1 == VL6180X_ERROR_NONE) {
//      Serial.print("Sensor #1 - ");
//      Serial.print("Range1: "); Serial.print(range1); Serial.print(" \n");
//      
      if (range1 < 100){
//        Serial.print("Difference from last left: ");
        int diff1 = (millis() - lastTimeofLeft);
//        Serial.print(diff1); Serial.print("\n");
        if (left && diff1 < 250 && diff1 > 140){
//          Serial.print("Right Swipe Detected\n");
          left = false;
        }else{
          right = true;
        }
        lastTimeofRight = millis();
        
      }
  }
  
  tcaselect(2);
//  mag2.getEvent(&event);
  /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
  float lux2 = vl2.readLux(VL6180X_ALS_GAIN_5);
  uint8_t range2 = vl2.readRange();
  uint8_t status2 = vl2.readRangeStatus();
//  Serial.print("lux2: "); Serial.print(lux1); Serial.print(" ");
  if (status2 == VL6180X_ERROR_NONE) {
//      Serial.print("Sensor #2 - ");
//      Serial.print("Range2: "); Serial.print(range2); Serial.print(" \n");

      if (range2 < 100){
        int diff2 = (millis() - lastTimeofRight);
//        Serial.print("Difference from last right");
//        Serial.print(diff2); Serial.print("\n");
        if (right && diff2 < 200 && diff2 > 120){
//          Serial.print("Left Swipe Detected\n");
          right = false;
        }else{
          left = true;
        }
        lastTimeofLeft = millis();
      }
  }
  if(range1 > 0 && range2 > 0 && (range1 < 200 || range2 < 200)){
    Serial.println(determinSide(range1, range2));
  }
  delay(500);
}
