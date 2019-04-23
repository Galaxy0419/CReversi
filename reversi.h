#ifndef _REVERSI_H
#define _REVERSI_H

typedef struct board {
	int board_matrix[8][8];
}board;

typedef struct moves {
	int move[32][2];
}moves;

typedef struct cord {
	int row;
	int column;
}cord;

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