// Copyright (c) 2019, William TANG <galaxyking0419@gmail.com>
#ifndef _REVERSI_H
#define _REVERSI_H

#include <stdint.h>

#define MAX_ROBOT_SENTENCE 3

typedef struct board {
	uint_fast8_t board_matrix[8][8];
}board_t;

typedef struct cord {
	int_fast8_t row;
	int_fast8_t column;
}cord_t;

const char* valid_difficulty = "123";
const char* valid_column = "abcdefgh";
const char* valid_row = "12345678";

const char pos_map[8][8] = {
	{'a', '0'},
	{'b', '1'},
	{'c', '2'},
	{'d', '3'},
	{'e', '4'},
	{'f', '5'},
	{'g', '6'},
	{'h', '7'}
};

const int valid_direction[8][2] = {
	{1, 0},
	{-1, 0},
	{0, 1},
	{0, -1},
	{-1, 1},
	{1, -1},
	{1, 1},
	{-1, -1}
};

const char sentence_of_robot[3][64] = {
	"Let me think for a while...",
	"Aha, smart move.",
	"I'm going to win!"
};

#endif