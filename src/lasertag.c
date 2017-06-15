#include "lasertag.h"
#include "dsh-common.h"
#include <util/delay.h>


void ltEmitCommand(
	const lt_cmd_t cmd,
	volatile uint8_t* pwmRegister,
	const uint8_t pwmOnLevel,
	const uint8_t pwmOffLevel
) {
	uint8_t bytes[] = {cmd.code1, cmd.code2, LT_DUAL_SIGNATURE};
	uint8_t bitCount = LT_COMMAND_BITS(cmd);

	*pwmRegister = pwmOnLevel;
	_delay_ms(2.4);
	*pwmRegister = pwmOffLevel;

	for(uint8_t i = 0; bitCount; i++) {
		uint8_t currentByte = bytes[i];
		for(uint8_t mask = 0x80; mask && bitCount; mask >>= 1, bitCount--) {
			_delay_ms(0.6);
			*pwmRegister = pwmOnLevel;
			if(currentByte & mask) {
				_delay_ms(1.2);
			} else {
				_delay_ms(0.6);
			}
			*pwmRegister = pwmOffLevel;
		}
	}
}


#define RING_SIZE 36
#define WAIT_HDR 0
#define WAIT_HIGH 1
#define WAIT_LOW 2
#define WAIT_DONE 3

#define ICP_COUNT(from, count) \
	( ring[ringIndex < from         ? ringIndex + RING_SIZE - from         : ringIndex - from        ] \
	- ring[ringIndex < from + count ? ringIndex + RING_SIZE - from - count : ringIndex - from - count] )

#define ICP_COUNT_0(count) \
	( ring[ringIndex] \
	- ring[ringIndex < count ? ringIndex + RING_SIZE - count : ringIndex - count] )

bool onIrSensTimerInterrupt(bool isCarrierPresent, volatile lt_cmd_t* result) {
	static uint8_t ring[RING_SIZE];
	static uint8_t ringIndex = 0;

	static uint8_t state = WAIT_HDR;
	static uint8_t waitCount = 0;

	static uint8_t data[3] = {0, 0, 0};
	static uint8_t bits = 0;

	uint8_t prevIndex = ringIndex;
	ringIndex = ringIndex >= (RING_SIZE - 1) ? 0 : ringIndex + 1;
	ring[ringIndex] = ring[prevIndex] + (isCarrierPresent ? 1 : 0);

	if(state == WAIT_HDR) {
		if(	   ICP_COUNT_0(  4 ) == 1
			&& ICP_COUNT_0(  3 ) == 0
			&& ICP_COUNT(3 , 20) >= 16
			&& ICP_COUNT(18, 6 ) >= 3
			&& ICP_COUNT(21, 14) <= 12
		) {
			state = WAIT_HIGH;
			waitCount = 0;
		}
	} else if(state == WAIT_HIGH) {
		if(ICP_COUNT_0(5) >= 3) {
			state = WAIT_LOW;
			waitCount = 0;
		} else if(waitCount > 9) {
			state = WAIT_DONE;
		} else {
			waitCount++;
		}
	} else if(state == WAIT_LOW) {
		if(ICP_COUNT_0(3) == 0) {
			if(waitCount >= 9) {
				data[bits >> 3] |= 0x80 >> (bits & 0x07);
			}
			bits++;
			state = WAIT_HIGH;
			waitCount = 0;
		} else if(waitCount > 17) {
			state = WAIT_DONE;
		} else {
			waitCount++;
		}
	}

	bool retval = false;
	if(state == WAIT_DONE || bits >= LT_DUAL_BITS) {
		if(	   (bits == LT_SHOT_BITS && data[0] <  0x80)
			|| (bits == LT_DUAL_BITS && data[0] >= 0x80 && data[2] == LT_DUAL_SIGNATURE)
		) {
			result->code1 = data[0];
			result->code2 = data[1];
			retval = true;
		}
		state = WAIT_HDR;
		data[0] = 0;
		data[1] = 0;
		data[2] = 0;
		bits = 0;
	}


	return retval;
}

