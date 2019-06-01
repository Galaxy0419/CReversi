// Copyright (c) 2019, William TANG <galaxyking0419@gmail.com>
#ifndef __REVERSI_H__
#define __REVERSI_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#define MAX_ROBOT_SENTENCE 3

typedef struct board {
	uint_fast8_t board_matrix[8][8];
} board_t;

typedef struct cord {
	int_fast8_t row;
	int_fast8_t column;
} cord_t;

typedef struct v_mov {
	uint_fast8_t player;
	board_t game_board;
	cord_t *restrict valid_cords;
	size_t *restrict const length;
} v_mov_t;

const char valid_column[9] = "abcdefgh";
const char valid_row[9] = "12345678";

const char pos_map[8][2] = {
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

const char sentence_of_robot[MAX_ROBOT_SENTENCE][32] = {
	"Let me think for a while...",
	"Aha, smart move.",
	"I'm going to win!"
};


#ifdef __cplusplus
}
#endif

#endif