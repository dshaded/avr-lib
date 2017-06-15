#include <inttypes.h>
#include <stdbool.h>
typedef struct lt_cmd_struct {
	union {
		uint8_t playerId;
		uint8_t code1;
	};
	union {
		struct {
			uint8_t colorCode:2;
			uint8_t damageCode:4;
		};
		uint8_t code2;
	};
} lt_cmd_t;

#define LT_SHOT_BITS 14
#define LT_DUAL_BITS 24
#define LT_DUAL_SIGNATURE 0xe8

#define LT_IS_SHOT(cmd) (cmd.code1 < 0x80)
#define LT_IS_DUAL(cmd) (cmd.code1 >= 0x80)

#define LT_COMMAND_BITS(cmd) (LT_IS_SHOT(cmd) ? LT_SHOT_BITS : LT_DUAL_BITS)

// 4-bit damage code (t4 type)
#define LT_VAL_1   (0x0)
#define LT_VAL_2   (0x1)
#define LT_VAL_4   (0x2)
#define LT_VAL_5   (0x3)
#define LT_VAL_7   (0x4)
#define LT_VAL_10  (0x5)
#define LT_VAL_10  (0x5)
#define LT_VAL_15  (0x6)
#define LT_VAL_17  (0x7)
#define LT_VAL_20  (0x8)
#define LT_VAL_25  (0x9)
#define LT_VAL_30  (0xA)
#define LT_VAL_35  (0xB)
#define LT_VAL_40  (0xC)
#define LT_VAL_50  (0xD)
#define LT_VAL_75  (0xE)
#define LT_VAL_100 (0xF)

// 2-bit damage code (t2 type)
#define LT_RED    (0)
#define LT_BLUE   (1)
#define LT_YELLOW (2)
#define LT_GREEN  (3)

// second byte of LT_CMD_DUAL:LT_C_SIMPLE command
#define LT_KILL               (0x00)
#define LT_PAUSE              (0x01)
#define LT_START              (0x02)
#define LT_RESET_CFG          (0x03)
#define LT_RESPAWN            (0x04)
#define LT_NEW_GAME           (0x05)
#define LT_FULL_AMMO          (0x06)
#define LT_END_GAME           (0x07)
#define LT_TIME_RESET         (0x08)
#define LT_CHANGE_TEAM        (0x09) //LW
#define LT_INIT_PLAYER        (0x0A)
#define LT_BLAST              (0x0B)
#define LT_NEW_GAME_WAIT      (0x0C)
#define LT_FULL_HP            (0x0D)
#define LT_CHANGE_POWER       (0x0E) //LW
#define LT_FULL_ARMOR         (0x0F)
#define LT_DOUBLE_HP          (0x10) //LW
#define LT_POINT_CAPTURED     (0x11) //LW
#define LT_BOMB_DEFUSED       (0x12) //LW
#define LT_RESET_SCORE        (0x14)
#define LT_TEST               (0x15)
#define LT_STUN               (0x16)
#define LT_DISARM             (0x17)
#define LT_DAMAGE_UP          (0x20) //LW
#define LT_RATE_750           (0x21) //LW
#define LT_DOUBLE_HP_ONCE     (0x22) //LW

// first byte of LT_CMD_DUAL
#define LT_C_HEAL             (0x80)
#define LT_C_ADD_ROUNDS       (0x81)
#define LT_C_SIMPLE           (0x83)
#define LT_C_ADD_CLIPS        (0x8A)
#define LT_C_HEAL_T           (0x8B)
#define LT_C_RADIATION        (0xA0) //LW
#define LT_C_ANOMALY          (0xA1) //LW
#define LT_C_PRESET           (0xA8) //LW
#define LT_C_TEAM             (0xA9) //LW
#define LT_C_SOUND            (0xAA) //LW

#define LT_C_HP_RED           (0xB0) //LW
#define LT_C_HP_BLUE          (0xB1) //LW
#define LT_C_HP_YELLOW        (0xB2) //LW
#define LT_C_HP_GREEN         (0xB3) //LW

#define LT_C_TEAM_RESPAWN     (0xB4) //LW
#define LT_C_TEAM_PAUSE       (0xB5) //LW
#define LT_C_TEAM_KILL        (0xB6) //LW
#define LT_C_TEAM_FULL_AMMO   (0xB7) //LW

#define LT_C_PLAYER_RESPAWN   (0xB8) //LW
#define LT_C_PLAYER_PAUSE     (0xB9) //LW
#define LT_C_PLAYER_KILL      (0xBA) //LW
#define LT_C_PLAYER_FULL_AMMO (0xBB) //LW

//14-bit shot command
#define LT_CMD_SHOT(player_id_n127, color_t2, power_t4) { \
	.playerId = (player_id_n127 & 0x7F), \
	.colorCode = (color_t2), \
	.damageCode = (power_t4) \
}

// generic 2-byte command
#define LT_CMD_DUAL(code_1, code_2) { .code1 = (code_1), .code2 = (code_2) }

// ready to use 2-byte command macros.
// n127      8bit int             0 - 127
// n100      8bit int             0 - 100
// n16       8bit int             0 - 15
// t4        4-bit damage code    0 - 15
// t2        2-bit color code     0 - 3
// signed    8-bit signed      -128 - 127
#define LT_CMD_HEAL(hp_n100)                    LT_CMD_DUAL( LT_C_HEAL             , hp_n100   )
#define LT_CMD_ADD_ROUNDS(ammo_n100)            LT_CMD_DUAL( LT_C_ADD_ROUNDS       , ammo_n100 )
#define LT_CMD_SIMPLE(code)                     LT_CMD_DUAL( LT_C_SIMPLE           , code      )
#define LT_CMD_ADD_CLIPS(clips_n16)             LT_CMD_DUAL( LT_C_ADD_CLIPS        , clips_n16 )
#define LT_CMD_HEAL_T(hp_t4)                    LT_CMD_DUAL( LT_C_HEAL_T           , hp_t4     )
#define LT_CMD_RADIATION(hp_n100)               LT_CMD_DUAL( LT_C_RADIATION        , hp_n100   )
#define LT_CMD_ANOMALY(hp_n100)                 LT_CMD_DUAL( LT_C_ANOMALY          , hp_n100   )
#define LT_CMD_PRESET(id_n16)                   LT_CMD_DUAL( LT_C_PRESET           , id_n16    )
#define LT_CMD_TEAM(color_t2)                   LT_CMD_DUAL( LT_C_TEAM             , color_t2  )
#define LT_CMD_SOUND(id)                        LT_CMD_DUAL( LT_C_SOUND            , id        )

#define LT_CMD_HP_RED(hp_signed)                LT_CMD_DUAL( LT_C_HP_RED           , hp_signed )
#define LT_CMD_HP_BLUE(hp_signed)               LT_CMD_DUAL( LT_C_HP_BLUE          , hp_signed )
#define LT_CMD_HP_YELLOW(hp_signed)             LT_CMD_DUAL( LT_C_HP_YELLOW        , hp_signed )
#define LT_CMD_HP_GREEN(hp_signed)              LT_CMD_DUAL( LT_C_HP_GREEN         , hp_signed )

#define LT_CMD_TEAM_RESPAWN(color_t2)           LT_CMD_DUAL( LT_C_TEAM_RESPAWN     , color_t2 )
#define LT_CMD_TEAM_PAUSE(color_t2)             LT_CMD_DUAL( LT_C_TEAM_PAUSE       , color_t2 )
#define LT_CMD_TEAM_KILL(color_t2)              LT_CMD_DUAL( LT_C_TEAM_KILL        , color_t2 )
#define LT_CMD_TEAM_FULL_AMMO(color_t2)         LT_CMD_DUAL( LT_C_TEAM_FULL_AMMO   , color_t2 )

#define LT_CMD_PLAYER_RESPAWN(player_id_n127)   LT_CMD_DUAL( LT_C_PLAYER_RESPAWN   , player_id_n127 )
#define LT_CMD_PLAYER_PAUSE(player_id_n127)     LT_CMD_DUAL( LT_C_PLAYER_PAUSE     , player_id_n127 )
#define LT_CMD_PLAYER_KILL(player_id_n127)      LT_CMD_DUAL( LT_C_PLAYER_KILL      , player_id_n127 )
#define LT_CMD_PLAYER_FULL_AMMO(player_id_n127) LT_CMD_DUAL( LT_C_PLAYER_FULL_AMMO , player_id_n127 )



// Emit miles-tag command via preconfigured 56kHz pwm.
void ltEmitCommand(
	const lt_cmd_t cmd,
	volatile uint8_t* pwmRegister,
	const uint8_t pwmOnLevel,
	const uint8_t pwmOffLevel
);

//MUST be called exactly once per 100us.
//Returns true when lt command was fully received and stored intto result.
bool onIrSensTimerInterrupt(bool isCarrierPresent, volatile lt_cmd_t* result);
