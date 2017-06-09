#include "lasertag.h"
#include <util/delay.h>

void ltEmitCommand(volatile uint8_t* pwmRegister, const uint8_t* cmdPtr, const uint8_t pwmOnLevel, const uint8_t pwmOffLevel) {
	*pwmRegister = pwmOnLevel;
	_delay_ms(2.4);
	*pwmRegister = pwmOffLevel;
	_delay_ms(0.6);

	uint8_t bitCount = *cmdPtr++;
	while(bitCount) {
		uint8_t currentByte = *cmdPtr++;
		for(uint8_t mask = 0x80; mask && bitCount; mask >>= 1, bitCount--) {
			*pwmRegister = pwmOnLevel;
			if(currentByte & mask) {
				_delay_ms(1.2);
			} else {
				_delay_ms(0.6);
			}
			*pwmRegister = pwmOffLevel;
			_delay_ms(0.6);
		}
	}
}
