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
#include <string.h>

#define SENSOR_START A0
#define THRESHOLD 800
#define DISTANCE_MM 50.0

#define LCD_LINE_LEN 20
#define LCD_NUM_LINES 4
#define DOUBLE_PRINT_LEN 10
#define DOUBLE_PRINT_DEC 7

struct sensor
{
	uint8_t pin;
	unsigned long trigger_time;
	boolean state;
};

struct sensor sensors[2];

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void
setup()
{
	for (uint8_t i = 0; i < 2; ++i)
	{
		sensors[i].pin = SENSOR_START + i;
		sensors[i].state = false;
		sensors[i].trigger_time = 0;
	}

	Serial.begin(9600);
	lcd.begin(LCD_LINE_LEN, LCD_NUM_LINES);
}

static void
check_sensor(struct sensor *sensor)
{
	int value = analogRead(sensor->pin);
	boolean state = value > THRESHOLD;

	sensor->state = state;

	if (state)
		sensor->trigger_time = millis();
}

static char *
format_double(char *buf, const char *label, const double d)
{
	const size_t label_len = strlen(label);

	strcpy(buf, label);
	dtostrf(d, DOUBLE_PRINT_LEN, DOUBLE_PRINT_DEC, buf + label_len);

	return buf;
}

static void
output(const char *buf)
{
	lcd.print(buf);
	Serial.print(buf);
}

void
loop()
{
	if (!sensors[0].state)
		check_sensor(&sensors[0]);

	if (sensors[0].state)
		check_sensor(&sensors[1]);

	if (sensors[1].state)
	{
		unsigned long diff = sensors[1].trigger_time - sensors[0].trigger_time;
		double velocity = DISTANCE_MM / diff;
		double mph = velocity * 2.2369;
		char buf[LCD_LINE_LEN + 1];

		lcd.clear();
		Serial.println();

		snprintf(buf, LCD_LINE_LEN, "Time: %lu ms", diff);
		output(buf);

		lcd.setCursor(0,1);
		output(format_double(buf, "m/s: ", velocity));

		lcd.setCursor(0,2);
		output(format_double(buf, "mph: ", mph));

		for (uint8_t i = 0; i < 2; ++i)
			sensors[i].state = 0;
	}
}
