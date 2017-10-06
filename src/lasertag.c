#include "lasertag.h"
#include <avr/interrupt.h>


static void ltMicrotimeWait(uint16_t targetMicrotime, uint16_t (*microtime) ()) {
	uint16_t start = microtime();
	uint16_t delay = targetMicrotime - start;
	while(microtime() - start < delay);
}

void ltEmitCommand(
	const lt_cmd_t cmd,
	volatile uint8_t* pwmRegister,
	const uint8_t pwmOnLevel,
	const uint8_t pwmOffLevel,
	uint16_t (*microtime) ()
) {
	uint8_t bytes[] = {cmd.code1, cmd.code2, LT_DUAL_SIGNATURE};
	uint8_t bitCount = LT_COMMAND_BITS(cmd);

	uint16_t target = microtime();
	*pwmRegister = pwmOnLevel;

	ltMicrotimeWait(target += 2400, microtime);
	*pwmRegister = pwmOffLevel;

	for(uint8_t i = 0; bitCount; i++) {
		uint8_t currentByte = bytes[i];
		for(uint8_t mask = 0x80; mask && bitCount; mask >>= 1, bitCount--) {
			ltMicrotimeWait(target += 600, microtime);
			*pwmRegister = pwmOnLevel;
			target += (currentByte & mask) ? 1200 : 600;
			ltMicrotimeWait(target, microtime);
			*pwmRegister = pwmOffLevel;
		}
	}
}

static uint8_t masks[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x002, 0x01};
static uint8_t receiveBuffer[4];//byte 0 is curent bit offset
static uint8_t byteIndex;
static uint16_t prevMicroTime;
static bool isShot;
static volatile lt_cmd_t receivedCmd;

void ltGetReceivedCommand(volatile lt_cmd_t* command) {
	cli();
	*command = receivedCmd;
	sei();
}

bool ltOnIrPCI(bool carrierPresent, uint16_t microtime) {
	uint16_t delta16 = prevMicroTime;
	prevMicroTime = microtime;
	delta16 = prevMicroTime - delta16;

	//todo - optimize if-branching

	if(carrierPresent) { //check silence time
		if(delta16 < 400 || delta16 >= 800) { //not good enough for 600us pause
			byteIndex = 0;
		}
		return false;
	}

	if(delta16 < 400 || delta16 >= 2900) { //not good enough for 600us 0 or 2400us header
		byteIndex = 0;
		return false;
	}

	if(((uint8_t)(delta16 >> 8)) >= 0x08) { //2048~2900us - header detected - init data
		byteIndex = 1;
		receiveBuffer[0] = 0;
		receiveBuffer[1] = 0;
		receiveBuffer[2] = 0;
		receiveBuffer[3] = 0;
		return false;
	}

	if(byteIndex == 0) { //no header detected - ignore any signals
		return false;
	}

	uint8_t bitIndex = receiveBuffer[0];
	if(delta16 >= 900) { //900~2048us - received 1; 400~900us - received 0
		receiveBuffer[byteIndex] |= masks[bitIndex];
	}

	if(bitIndex == 0) {
		if(byteIndex == 1) {
			isShot = !receiveBuffer[1];
		}
	} else if(bitIndex == 5) {
		if(byteIndex == 2 && isShot) {
			byteIndex = 0;
			receivedCmd.code1 = receiveBuffer[1];
			receivedCmd.code2 = receiveBuffer[2];
			return true;
		}
	} else if(bitIndex == 7) {
		if(byteIndex == 3) {
			byteIndex = 0;
			if(receiveBuffer[3] == LT_DUAL_SIGNATURE) {
				receivedCmd.code1 = receiveBuffer[1];
				receivedCmd.code2 = receiveBuffer[2];
				return true;
			} else {
				return false;
			}
		} else {
			receiveBuffer[0] = 0;
			byteIndex++;
			return false;
		}
	}
	receiveBuffer[0] = bitIndex + 1;
	return false;
}
