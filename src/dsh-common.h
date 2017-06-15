
#define IS_LOW(port, bit) (!((port) & _BV(bit)))
#define IS_HIGH(port, bit) (!IS_LOW(port, bit))

#define SET_BIT(port, bit, state) do { \
	if(state) { \
		port |= _BV(bit); \
	} else  { \
		port &= ~_BV(bit); \
	} \
} while(0)


