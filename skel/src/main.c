#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdbool.h>

#include "dsh-common.h"


/************************************ MACROS ************************************/
#define CONST_1 123              // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
#define CONST_2 (CONST_1 >> 1)   // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX


/************************************ VARS ************************************/
static volatile uint8_t moduleVar;                               // XXXXXXXXXXXXXXXXXXXXX
volatile bool globalVar;                                         // XXXXXXXXXXXXXXXXXXXXX


/************************************ FORWARD DECLARATIONS ************************************/
static void hwInit();

static void waitInterrupt();                                                // XXXXXXXXXX
static uint16_t intFunction(uint8_t param1);                                // XXXXXXXXXX


/************************************ INTERRUPTS ************************************/
EMPTY_INTERRUPT(INT0_vect)               //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

ISR(TIMER1_COMPA_vect, ISR_NAKED) {      //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	//naked interrupt                    //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	PORTB ^= _BV(PB0);                   //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	reti();                              //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
}                                        //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

ISR(ADC_vect) {                          //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	static uint16_t staticLocalVar = 0;  //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	uint8_t localVar = ADCH;             //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	staticLocalVar += localVar;          //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	moduleVar += localVar;               //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
}                                        //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX


/************************************ MAIN CODE ************************************/
int main() {
	hwInit();

//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	static uint16_t staticLocalVar = 0;
	uint8_t localVar = 3;
	bool boolVar = true;

	while(true) {
		waitInterrupt();
		staticLocalVar = intFunction(localVar);
		boolVar = staticLocalVar == 888;
		if(boolVar) {
			_delay_ms(2000);
		}
	}
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
}

uint16_t intFunction(uint8_t param1) {  //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	return ADC * param1;                //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
}                                       //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

void waitInterrupt() {
	sleep_enable();
	sei();
	sleep_cpu();
	sleep_disable();
	cli();
}

/************************************ HWINIT ************************************/
void hwInit() {
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	OCR1A  = 123;
	OCR1B  = 0;
	TCCR1A = _BV(COM1B1) | _BV(COM1B0) | _BV(WGM10) | _BV(WGM11);
	TCCR1B = _BV(CS10) | _BV(WGM12) | _BV(WGM13);

	DDRB = _BV(DDB0) | _BV(DDB1) | _BV(DDB2);

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sei();
//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
}
