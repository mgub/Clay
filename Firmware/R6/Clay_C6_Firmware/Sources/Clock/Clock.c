/*
 * Clock.cpp
 *
 *  Created on: Jan 26, 2016
 *      Author: thebh
 */
////includes
#include "PE_Types.h"
#include "Clock.h"

////global vars
bool tick_50us;
bool tick_1ms;
bool tick_50ms;
bool tick_250ms;
bool tick_500ms;
bool tick_3000ms;

////local vars
uint32_t PowerOnTime_ms;
uint8_t tick_count;

////local function prototypes

////global function implementations
uint32_t Millis() {
	return PowerOnTime_ms;
}

void Wait(uint32_t wait_ms) {
	uint32_t Start = PowerOnTime_ms;
	while ((PowerOnTime_ms - Start) <= wait_ms)
		;
}

bool Clock_Enable() {
}

bool Clock_Start() {
	PowerOnTime_ms = 0;
	tick_count = 0;
}

bool Clock_Stop() {
	//todo: disable/deinit
}

bool Clock_Pause() {
	//todo: disable
}

void Resume() {
	//todo: re-enable
	//reset tick?
}

void Tick() {

	tick_50us = TRUE;

	if (++tick_count >= 20) {
		tick_count = 0;
		tick_1ms = TRUE;
		++PowerOnTime_ms;

		if (!(PowerOnTime_ms % 50)) {
			tick_50ms = TRUE;
		}

		if (!(PowerOnTime_ms % 250)) {
			tick_250ms = TRUE;
		}

		if (!(PowerOnTime_ms % 500)) {
			tick_500ms = TRUE;
		}

		if (!(PowerOnTime_ms % 3000)) {
			tick_3000ms = TRUE;
		}
	}

}

////local function implementations