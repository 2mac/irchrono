#include <LiquidCrystal.h>
#include <stdint.h>
#include <stdio.h>

#define SENSOR_START A0

#define THRESHOLD 800

#define DISTANCE_MM 50.0

int sensors[2];
boolean states[2];
unsigned long times[2];

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{
  for (uint8_t i = 0; i < 2; ++i)
  {
    states[i] = false;
    times[i] = 0;
  }
  
  Serial.begin(9600);
  lcd.begin(20, 4);
}

void check_sensor(uint8_t index)
{
  sensors[index] = analogRead(SENSOR_START + index);
  states[index] = sensors[index] > THRESHOLD;
  
  if (states[index])
    times[index] = millis();
}

void loop()
{
  if (!states[0])
  {
    check_sensor(0);
  }
  
  if (states[0])
  {
    check_sensor(1);
  }
  
  if (states[1])
  {
    unsigned long diff = times[1] - times[0];
    double velocity = DISTANCE_MM / diff;
    double mph = velocity * 2.2369;
    char buf[20], temp[10];
    
    Serial.print("Time: ");
    Serial.print(diff);
    Serial.print(", Velocity: ");
    Serial.print(velocity);
    Serial.print(" m/s = ");
    Serial.print(mph);
    Serial.println(" mph");
    
    lcd.clear();
    snprintf(buf, 20, "Time: %lu ms", diff);
    lcd.print(buf);
    lcd.setCursor(0,1);
    dtostrf(velocity, 10, 7, temp);
    snprintf(buf, 20, "m/s: %s", temp);
    lcd.print(buf);
    lcd.setCursor(0,2);
    dtostrf(mph, 10, 7, temp);
    snprintf(buf, 20, "mph: %s", temp);
    lcd.print(buf);
    
    for (uint8_t i = 0; i < 2; ++i)
    {
      states[i] = 0;
    }
  }
}
