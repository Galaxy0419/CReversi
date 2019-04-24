// Copyright (c) 2019, William TANG <galaxyking0419@gmail.com>
#ifndef _REVERSI_H
#define _REVERSI_H

#include <stdint.h>

typedef struct board {
	int8_t board_matrix[8][8];
}board_t;

typedef struct cord {
	int8_t row;
	int8_t column;
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
#endif