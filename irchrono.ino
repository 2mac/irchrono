/*
 * irchrono - Arduino chronograph using infrared sensors
 * Copyright (C) 2020 David McMackins II
 *
 * Redistributions, modified or unmodified, in whole or in part, must retain
 * applicable copyright or other legal privilege notices, these conditions, and
 * the following license terms and disclaimer.  Subject to these conditions,
 * the holder(s) of copyright or other legal privileges, author(s) or
 * assembler(s), and contributors of this work hereby grant to any person who
 * obtains a copy of this work in any form:
 *
 * 1. Permission to reproduce, modify, distribute, publish, sell, sublicense,
 * use, and/or otherwise deal in the licensed material without restriction.
 *
 * 2. A perpetual, worldwide, non-exclusive, royalty-free, irrevocable patent
 * license to reproduce, modify, distribute, publish, sell, use, and/or
 * otherwise deal in the licensed material without restriction, for any and all
 * patents:
 *
 *     a. Held by each such holder of copyright or other legal privilege,
 *     author or assembler, or contributor, necessarily infringed by the
 *     contributions alone or by combination with the work, of that privilege
 *     holder, author or assembler, or contributor.
 *
 *     b. Necessarily infringed by the work at the time that holder of
 *     copyright or other privilege, author or assembler, or contributor made
 *     any contribution to the work.
 *
 * NO WARRANTY OF ANY KIND IS IMPLIED BY, OR SHOULD BE INFERRED FROM, THIS
 * LICENSE OR THE ACT OF DISTRIBUTION UNDER THE TERMS OF THIS LICENSE,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 * A PARTICULAR PURPOSE, AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS,
 * ASSEMBLERS, OR HOLDERS OF COPYRIGHT OR OTHER LEGAL PRIVILEGE BE LIABLE FOR
 * ANY CLAIM, DAMAGES, OR OTHER LIABILITY, WHETHER IN ACTION OF CONTRACT, TORT,
 * OR OTHERWISE ARISING FROM, OUT OF, OR IN CONNECTION WITH THE WORK OR THE USE
 * OF OR OTHER DEALINGS IN THE WORK.
 */

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
			states[i] = 0;
	}
}
