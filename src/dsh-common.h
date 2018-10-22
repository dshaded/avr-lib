
#define IS_LOW(value, bit) (!((value) & (1 << (bit))))
#define IS_HIGH(value, bit) (!IS_LOW(value, bit))

#define SET_BIT(port, bit, state) do { \
	if(state) { \
		port |= 1 << (bit); \
	} else  { \
		port &= ~ (1 << (bit)); \
	} \
} while(0)


#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))


#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, NAME, ...) NAME
#define BV(...) GET_MACRO(__VA_ARGS__, _BV8, _BV7, _BV6, _BV5, _BV4, _BV3, _BV2, _BV1)(__VA_ARGS__)

#define _BV1(_1)                             ( 1 << (_1) )
#define _BV2(_1, _2)                         ( _BV1(_1) | _BV1(_2) )
#define _BV3(_1, _2, _3)                     ( _BV1(_1) | _BV1(_2) | _BV1(_3) )
#define _BV4(_1, _2, _3, _4)                 ( _BV1(_1) | _BV1(_2) | _BV1(_3) | _BV1(_4) )
#define _BV5(_1, _2, _3, _4, _5)             ( _BV1(_1) | _BV1(_2) | _BV1(_3) | _BV1(_4) | _BV1(_5) )
#define _BV6(_1, _2, _3, _4, _5, _6)         ( _BV1(_1) | _BV1(_2) | _BV1(_3) | _BV1(_4) | _BV1(_5) | _BV1(_6) )
#define _BV7(_1, _2, _3, _4, _5, _6, _7)     ( _BV1(_1) | _BV1(_2) | _BV1(_3) | _BV1(_4) | _BV1(_5) | _BV1(_6) | _BV1(_7) )
#define _BV8(_1, _2, _3, _4, _5, _6, _7, _8) ( _BV1(_1) | _BV1(_2) | _BV1(_3) | _BV1(_4) | _BV1(_5) | _BV1(_6) | _BV1(_7) | _BV1(_8) )
