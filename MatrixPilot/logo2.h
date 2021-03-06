// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.

// Defines the logo language

#ifndef LOGO2_H
#define LOGO2_H

#include "logo2_tables.h"

typedef enum
{
    LOGO_LANG_TYPE_INST = 0,        // List of avaialble istructions / functions
    LOGO_LANG_TYPE_VAR,             // Reference to UDB variable
    LOGO_LANG_TYPE_INST_INTERRUPT,  // List of available interrupts
    LOGO_LANG_TYPE_GLOBAL,          // Gloal variables for logo
    LOGO_LANG_TYPE_TURTLE,          // List of available turtles
} LOGO_LANG_TYPE_e;

// Type for containing a description of a logo language item and how it is used
typedef struct tag_logo_lang_def
{
    const char name[23] ;                           // Language name
    uint16_t index                :  16 ;
    uint16_t stack_depth          :  8 ;
    uint16_t lang_type            :  4 ;
    uint16_t can_call_live        :  1 ;        // Can it be called live or not
    uint16_t param_count          :  3 ;        // If a function, how many parameters it takes
} tag_logo_lang_def_t ;

extern const tag_logo_lang_def_t logo_instr_definition_list[];
extern const uint16_t count_of_instr_def_list;

extern const tag_logo_lang_def_t logo_vars_definition_list[];
extern const uint16_t count_of_vars_def_list;

extern const tag_logo_lang_def_t logo_int_definitions_list[];
extern const uint16_t count_of_int_def_list;

extern const tag_logo_lang_def_t logo_global_definitions_list[];
extern const uint16_t count_of_global_def_list;

extern const tag_logo_lang_def_t logo_turtle_definitions_list[];
extern const uint16_t count_of_turtle_def_list;


struct logoInstructionDef
{
	uint16_t cmd		:  8 ;
	uint16_t do_fly		:  1 ;
	uint16_t use_param	:  1 ;
	uint16_t subcmd		:  6 ;
	int16_t arg					: 16 ;
	int16_t arg2				: 16 ;
	int16_t arg3				: 16 ;
	int16_t arg4				: 16 ;
} ;

// RAM based instruction buffer for running macros and similar.
#define LOGO_INSTRUCTION_BUFFER_SIZE	16
extern struct logoInstructionDef logoInstructionBuffer[LOGO_INSTRUCTION_BUFFER_SIZE];
extern uint16_t instrBufferFillCount;


typedef struct tag_logo_flightplan_ref
{
    const char name[23];
    const uint8_t index;
    const struct logoInstructionDef* pflightPlan;
} logo_flightplan_ref_t;

extern const logo_flightplan_ref_t logo_flightplan_ref_list[];
extern const uint16_t count_of_flightplan_ref_list;

// Note that any instruction with an odd subcmd is a FLY command.
// Interpretation stops on a FLY command until the plane arrives at that
// location, similar to a waypoint.  This includes PEN_DOWN.
// When the pen is up, FLY commands do not stop the interpreter.  So when
// the pen goes back down, we FLY to wherever the turtle has moved to
// while the pen was up.  We also skip flying when the CAMERA turtle is
// the active turtle.



//typedef enum 
//{
//	LOGO_CMD_HIGH = LOGO_LOW_CMD_MAX,
//	LOGO_HIGH_CMD_MAX,
//} eLOGO_HIGH_COMMANDS;
//

// Define the Low-level Commands
//							   cmd,fly,param,sub,x
#define _REPEAT(n, pr)			{LOGO_CMD_REPEAT,		0,	pr,	0,	n},
#define _END					{LOGO_CMD_END,			0,	0,	1,	0},
#define _ELSE					{LOGO_CMD_ELSE,			0,	0,	3,	0},
#define _TO(fn)					{LOGO_CMD_TO,			0,	0,	2,	fn},

#define _DO(fn, x, pr)			{LOGO_CMD_DO,			0,	pr,	fn, x},
#define _EXEC(fn, x, pr)		{LOGO_CMD_EXEC,			0,	pr,	fn, x},

#define _FD(x, fl, pr)			{LOGO_CMD_FD,			fl,	pr,	0,	x},

#define _RT(x, pr)				{LOGO_CMD_RT,			0,	pr, 0,	x},
#define _SET_ANGLE(x, pr)		{LOGO_CMD_SET_ANGLE,	0,	pr, 1,	x},
#define _USE_CURRENT_ANGLE		{LOGO_CMD_USE_CURRENT_ANGLE,	0,	0,	2,	0},
#define _USE_ANGLE_TO_GOAL		{LOGO_CMD_USE_ANGLE_TO_GOAL,	0,	0,	3,	0},

#define _MV_X(x, fl, pr)		{LOGO_CMD_MV_X,		fl,	pr,	0,	x},
#define _SET_X(x, fl, pr)		{LOGO_CMD_SET_X,	fl,	pr,	1,	x},
#define _MV_Y(y, fl, pr)		{LOGO_CMD_MV_Y,		fl,	pr,	2,	y},
#define _SET_Y(y, fl, pr)		{LOGO_CMD_SET_Y,	fl,	pr,	3,	y},
#define _MV_Z(z, fl, pr)		{LOGO_CMD_MV_Z,		fl,	pr,	4,	z},
#define _SET_Z(z, fl, pr)		{LOGO_CMD_SET_Z,	fl,	pr,	5,	z},
#define _USE_CURRENT_POS(fl)	{LOGO_CMD_USE_CURRENT_POS, fl,	0,	6,	0},
#define _HOME(fl)				{LOGO_CMD_HOME,		fl,	0,	7,	0},

#define _SET_ABS_VAL_HIGH(x)	{LOGO_CMD_SET_ABS_VAL_HIGH,	0,	0,	8,	x}, // Set the high and then low words for X and
#define _SET_ABS_X_LOW(x)		{LOGO_CMD_SET_ABS_X_LOW,	0,	0,	9,	x}, // then Y, as 4 consecutive instructions.
#define _SET_ABS_Y_LOW(y, fl)	{LOGO_CMD_SET_ABS_Y_LOW,	fl,	0,	10,	y}, // (as VAL_HIGH, X_LOW, VAL_HIGH, Y_LOW)

#define _SET_ABS_X_Y(x,y)		{LOGO_CMD_SET_ABS_X_Y,		fl,	0,	11,	\
																((((uint32_t)(x))>>16)&0xFFFF), (((uint32_t)(x))&0xFFFF), \
																((((uint32_t)(y))>>16)&0xFFFF), (((uint32_t)(y))&0xFFFF)},

#define _FLAG_ON(f)				{LOGO_CMD_FLAG_ON,		0,	0,	0,	f},
#define _FLAG_OFF(f)			{LOGO_CMD_FLAG_OFF,		0,	0,	1,	f},
#define _FLAG_TOGGLE(f)			{LOGO_CMD_FLAG_TOGGLE,	0,	0,	2,	f},

#define _PEN_UP					{LOGO_CMD_PEN_UP,		0,	0,	0,	0},
#define _PEN_DOWN				{LOGO_CMD_PEN_DOWN,		1,	0,	1,	0},
#define _PEN_TOGGLE				{LOGO_CMD_PEN_TOGGLE,	0,	0,	2,	0},

#define _SET_TURTLE(x)			{LOGO_CMD_SET_TURTLE,	0,	0,	0,	x},

#define _PARAM_SET(x)			{LOGO_CMD_PARAM_SET,	0,	0,	0,	x},
#define _PARAM_ADD(x)			{LOGO_CMD_PARAM_ADD,	0,	0,	1,	x},
#define _PARAM_MUL(x)			{LOGO_CMD_PARAM_MUL,	0,	0,	2,	x},
#define _PARAM_DIV(x)			{LOGO_CMD_PARAM_DIV,	0,	0,	3,	x},

#define _SPEED_INCREASE(s, pr)	{LOGO_CMD_SPEED_INCREASE,	0,	pr,	0,	s},
#define _SET_SPEED(s, pr)		{LOGO_CMD_SET_SPEED,		0,	pr,	1,	s},

#define _SET_INTERRUPT(fn)		{LOGO_CMD_SET_INTERRUPT,	0,	0,	1,	fn},
#define _CLEAR_INTERRUPT		{LOGO_CMD_CLEAR_INTERRUPT,	0,	0,	0,	0},

#define _LOAD_TO_PARAM(val)		{LOGO_CMD_LOAD_TO_PARAM,	0,	0,	val,0},

#define _IF_EQ(val, x, pr)		{LOGO_CMD_IF_EQ,	0,	pr,	val,x},
#define _IF_NE(val, x, pr)		{LOGO_CMD_IF_NE,	0,	pr,	val,x},
#define _IF_GT(val, x, pr)		{LOGO_CMD_IF_GT,	0,	pr,	val,x},
#define _IF_LT(val, x, pr)		{LOGO_CMD_IF_LT,	0,	pr,	val,x},
#define _IF_GE(val, x, pr)		{LOGO_CMD_IF_GE,	0,	pr,	val,x},
#define _IF_LE(val, x, pr)		{LOGO_CMD_IF_LE,	0,	pr,	val,x},


// Define the High-level Commands
#define FD(x)				_FD(x, 1, 0)
//#define FD(x)				{LOGO_CMD_FD,			1,	0,	0,	x},


#define BK(x)				_FD(-x, 1, 0)
#define FD_PARAM			_FD(1, 1, 1)
#define BK_PARAM			_FD(-1, 1, 1)
//#define BK_PARAM			{LOGO_CMD_BK_PARAM,		1,	1,	0,	1},

#define RT(x)				_RT(x, 0)
#define LT(x)				_RT(-x, 0)
#define SET_ANGLE(x)		_SET_ANGLE(x, 0)
#define RT_PARAM			_RT(1, 1)
#define LT_PARAM			_RT(-1, 1)
#define SET_ANGLE_PARAM		_SET_ANGLE(0, 1)
#define USE_CURRENT_ANGLE	_USE_CURRENT_ANGLE
#define USE_ANGLE_TO_GOAL	_USE_ANGLE_TO_GOAL

#define EAST(x)				_MV_X(x, 1, 0)
#define WEST(x)				_MV_X(-x, 1, 0)
#define SET_X_POS(x)		_SET_X(x, 1, 0)
#define EAST_PARAM			_MV_X(1, 1, 1)
#define WEST_PARAM			_MV_X(-1, 1, 1)
#define SET_X_POS_PARAM		_SET_X(1, 1, 1)
#define USE_CURRENT_POS		_USE_CURRENT_POS(1)

#define NORTH(y)			_MV_Y(y, 1, 0)
#define SOUTH(y)			_MV_Y(-y, 1, 0)
#define SET_Y_POS(y)		_SET_Y(y, 1, 0)
#define NORTH_PARAM			_MV_Y(1, 1, 1)
#define SOUTH_PARAM			_MV_Y(-1, 1, 1)
#define SET_Y_POS_PARAM		_SET_Y(1, 1, 1)

#define ALT_UP(z)			_MV_Z(z, 0, 0)
#define ALT_DOWN(z)			_MV_Z(-z, 0, 0)
#define SET_ALT(z)			_SET_Z(z, 0, 0)
#define ALT_UP_PARAM		_MV_Z(1, 0, 1)
#define ALT_DOWN_PARAM		_MV_Z(-1, 0, 1)
#define SET_ALT_PARAM		_SET_Z(1, 0, 1)

#define SPEED_INCREASE(x)	_SPEED_INCREASE(x, 0)
#define SPEED_DECREASE(x)	_SPEED_INCREASE(-x, 0)
#define SET_SPEED(x)		_SET_SPEED(x, 0)
#define SPEED_INCREASE_PARAM _SPEED_INCREASE(1, 1)
#define SPEED_DECREASE_PARAM _SPEED_INCREASE(-1, 1)
#define SET_SPEED_PARAM		_SET_SPEED(0, 1)

#define FLAG_ON(f)			_FLAG_ON(f)
#define FLAG_OFF(f)			_FLAG_OFF(f)
#define FLAG_TOGGLE(f)		_FLAG_TOGGLE(f)

#define PEN_UP				_PEN_UP
#define PEN_DOWN			_PEN_DOWN
#define PEN_TOGGLE			_PEN_TOGGLE

#define SET_TURTLE(x)		_SET_TURTLE(x)

#define REPEAT(n)			_REPEAT(n, 0)
#define REPEAT_PARAM		_REPEAT(1, 1)
#define REPEAT_FOREVER		_REPEAT(-1, 0)
#define END					_END
#define ELSE				_ELSE

#define TO(func)			_TO(func)

#define DO(func)			_DO(func, 0, 0)
#define DO_ARG(func, arg)	_DO(func, arg, 0)
#define DO_PARAM(func)		_DO(func, 1, 1)

#define EXEC(func)			_EXEC(func, 0, 0)
#define EXEC_ARG(func, arg)	_EXEC(func, arg, 0)
#define EXEC_PARAM(func)	_EXEC(func, 1, 1)

#define PARAM_SET(x)		_PARAM_SET(x)
#define PARAM_ADD(x)		_PARAM_ADD(x)
#define PARAM_SUB(x)		_PARAM_ADD(-x)
#define PARAM_MUL(x)		_PARAM_MUL(x)
#define PARAM_DIV(x)		_PARAM_DIV(x)

#define SET_INTERRUPT(fn)	_SET_INTERRUPT(fn)
#define CLEAR_INTERRUPT		_CLEAR_INTERRUPT

#define LOAD_TO_PARAM(val)	_LOAD_TO_PARAM(val)

#define IF_EQ(val, x)		_IF_EQ(val, x, 0)
#define IF_NE(val, x)		_IF_NE(val, x, 0)
#define IF_GT(val, x)		_IF_GT(val, x, 0)
#define IF_LT(val, x)		_IF_LT(val, x, 0)
#define IF_GE(val, x)		_IF_GE(val, x, 0)
#define IF_LE(val, x)		_IF_LE(val, x, 0)
#define IF_EQ_PARAM(val)	_IF_EQ(val, 1, 1)
#define IF_NE_PARAM(val)	_IF_NE(val, 1, 1)
#define IF_GT_PARAM(val)	_IF_GT(val, 1, 1)
#define IF_LT_PARAM(val)	_IF_LT(val, 1, 1)
#define IF_GE_PARAM(val)	_IF_GE(val, 1, 1)
#define IF_LE_PARAM(val)	_IF_LE(val, 1, 1)

#define SET_POS(x, y)		_SET_X(x, 0, 0) _SET_Y(y, 1, 0)
#define SET_ABS_POS(x, y)	_SET_ABS_X_Y(x, y)
#define HOME				_HOME(1)


#include "flightplan-logo2.h"


#define NUM_INSTRUCTIONS (( sizeof instructions ) / sizeof ( struct logoInstructionDef ))
#define NUM_RTL_INSTRUCTIONS (( sizeof rtlInstructions ) / sizeof ( struct logoInstructionDef ))


#endif //#define LOGO2_H
