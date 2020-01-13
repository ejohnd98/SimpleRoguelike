#pragma once
enum class Command {
	NONE,
	MOVE_UP, //Actor commands
	MOVE_DOWN,
	MOVE_RIGHT,
	MOVE_LEFT,
	WAIT,
	NEXT_MAP, //Prop commands
	PREV_MAP,
	USE_DOOR,
	PLAYER_DIED, //System commands
	WON_THE_GAME 
};