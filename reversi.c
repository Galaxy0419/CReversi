#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "reversi.h"

static void my_gets(char *buffer, size_t buffer_len)
{
    memset(buffer, 0, buffer_len);

    int c;
    int bytes_read = 0;
    while (EOF != (c = fgetc(stdin)) && '\n' != c) {
        if (bytes_read < buffer_len - 1) {
            buffer[bytes_read++] = (char)c;
        }
    }
}

static inline int your_oppenent(int player)
{
	if (player == 1)
		return 2;
	return 1;
}

static inline bool on_board(int row, int column)
{
	return (row >= 0 && row <=7) && (column >=0 && column <= 7);
}

static int find_mapping_value(char pos)
{
	for (size_t i=0; i<8; i++)
		if (pos == pos_map[i][0])
			return atoi(&pos_map[i][1]);
}

static cord position(char* input)
{
	cord invalid_pos = {-1, -1};
	if (strchr(valid_column, input[0]) != NULL
		&& strchr(valid_row, input[1]) != NULL) {
			cord valid_pos = {atoi(&input[1]) - 1, find_mapping_value(input[0])};
			return valid_pos;
	}
	return invalid_pos;
}

static board new_board()
{
	board game_board;
	int board[8][8] = {
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 2, 1, 0, 0, 0},
		{0, 0, 0, 1, 2, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
	};
	memcpy(game_board.board_matrix, board, sizeof(board));
	return game_board;
}

static void print_board(board game_board, int black_score, int white_score)
{
	
	for (size_t i=0; i<8; i++) {
		printf(" %d |", i+1);
		for (size_t j=0; j<8; j++) {
			if (game_board.board_matrix[i][j] == 1)
				printf(" B");
			else if (game_board.board_matrix[i][j] == 2)
				printf(" W");
			else
				printf(" -");
		}
		if (i == 3)
			printf("\tBlack Score: %d", black_score);
		else if (i == 5)
			printf("\tWhite Score: %d", white_score);
		printf("\n");
	}
	printf(" --------------------\n");
	printf("     a b c d e f g h\n");
}

static void score(board game_board, int* black_score, int* white_score)
{
	*black_score = 0;
	*white_score = 0;
	for (size_t i=0; i<8; i++) {
		for (size_t j=0; j<8; j++) {
			if (game_board.board_matrix[i][j] == 1)
				*black_score += 1;
			else if (game_board.board_matrix[i][j] == 2)
				*white_score += 1;
		}
	}
}

static bool enclosing(board game_board, int player, cord pos, cord direction)
{
	int row = pos.row + direction.row;
	int column = pos.column + direction.column;

	while (on_board(row, column) 
	&& game_board.board_matrix[row][column] == your_oppenent(player)) {
		if (on_board(row+direction.row, column+direction.column) 
		&& game_board.board_matrix[row+direction.row][column+direction.column] == player)
			return true;
		row += direction.row;
		column += direction.column;
	}
	return false;
}

static moves valid_moves(board game_board, int player)
{
	int index_counter = 0;
	int dummy[2] = {-1, -1};
	moves valid;
	for (size_t i=0; i<32; i++)
	{
		memcpy(&valid.move[i], &dummy, sizeof(dummy));
	}

	for (size_t i=0; i<8; i++) {
		for (size_t j=0; j<8; j++) {
			for (size_t z=0; z<8; z++) {
				cord pos = {i, j};
				cord dir = {valid_direction[z][0], valid_direction[z][1]};
				if (game_board.board_matrix[i][j] == 0
				&& enclosing(game_board, player, pos, dir)) {
					valid.move[index_counter][0] = i;
					valid.move[index_counter][1] = j;
					index_counter++;
				}
			}
		}
	}
	return valid;
}

static void next_state(board *game_board, int* player, cord pos)
{
	game_board->board_matrix[pos.row][pos.column] = *player;
	for (size_t i=0; i<8; i++) {
		cord dir = {valid_direction[i][0], valid_direction[i][1]};
		if (enclosing(*game_board, *player, pos, dir)) {
			for (size_t j=1; game_board->board_matrix[pos.row + j*dir.row][pos.column + j*dir.column] != *player; j++) {
				game_board->board_matrix[pos.row + j*dir.row][pos.column + j*dir.column] = *player;
			}
		}
	}
	if (valid_moves(*game_board, *player).move[0][0] == -1 &&
	valid_moves(*game_board, your_oppenent(*player)).move[0][0] == -1) {
		*player = 0;
		return;
	}
	*player = your_oppenent(*player);
}

static cord promt_to_place(board game_board, int player)
{
	char pos[3];
	char cplayer;
	if (player == 1)
		cplayer = 'B';
	else
		cplayer = 'W';

	printf("(%c) Enter the position to place your disk: ", cplayer);
	my_gets(pos, sizeof(pos));
	if (pos[0] == 'q') {
		puts("Thanks for playing C Reversi.");
		exit(0);
	}

	cord converted_pos = position(pos);
	moves v_movs = valid_moves(game_board, player);
	for (size_t i=0; v_movs.move[i][0] != -1; i++)
		if (converted_pos.row == v_movs.move[i][0] 
		&& converted_pos.column == v_movs.move[i][1])
			return converted_pos;
	puts("Come on, man.");
	return promt_to_place(game_board, player);
}

static void finish_game(int black_score, int white_score)
{
	puts("The game has finished!");
	printf("Black Score: %d\n", black_score);
	printf("White Score: %d\n", white_score);

	if (black_score > white_score)
		puts("Black Wins!");
	else if (black_score == white_score)
		puts("Draw!");
	else
		puts("White Wins!");
	
	exit(0);
}

static void run_two_players()
{
	int player = 1;
	int black_score = 0;
	int white_score = 0;
	cord current_pos = {-1, -1};
	board in_game_board = new_board();

	while (player != 0) {
		score(in_game_board, &black_score, &white_score);
		print_board(in_game_board, black_score, white_score);
		current_pos = promt_to_place(in_game_board, player);
		next_state(&in_game_board, &player, current_pos);
		puts("\n");
	}
	finish_game(black_score, white_score);
}

static cord ai_place(board game_board)
{
	int max_enclose = 0;
	int max_enclose_index = 0;
	moves v_mov = valid_moves(game_board, 2);
	for (size_t i=0; v_mov.move[i][0] != -1; i++){
		int test_player = 2;
		int black_score = 0;
		int white_score = 0;
		cord current_pos = {v_mov.move[i][0], v_mov.move[i][1]};
		board test_board = game_board;
		next_state(&test_board, &test_player, current_pos);
		score(test_board, &black_score, &white_score);
		if (white_score > max_enclose)
			max_enclose_index = i;
	}
	cord max_score_pos = {v_mov.move[max_enclose_index][0], v_mov.move[max_enclose_index][0]};
	return max_score_pos;
}

static void run_single_player()
{
	int player = 1;
	int black_score = 0;
	int white_score = 0;
	board in_game_board = new_board();

	while (player != 0) {
		score(in_game_board, &black_score, &white_score);
		print_board(in_game_board, black_score, white_score);
		cord current_pos = {-1, -1};
		if (player == 1){
			current_pos = promt_to_place(in_game_board, player);
		}
		else {
			puts("\n");
			int randnum = rand() % 3;
			switch (randnum) {
				case(0):
					puts("Robot: Let me think for while...");
					break;
				case(1):
					puts("Robot: Aha, smart move.");
					break;
				case(2):
					puts("Robot: I'm going to win!");
					break;
			}
			sleep(4);
			current_pos = ai_place(in_game_board);
		}
		next_state(&in_game_board, &player, current_pos);
	}
	finish_game(black_score, white_score);
}

// main function
int main(void)
{
	char choice[2];
	puts("Welcome to C Reversi!");
	puts("1. Single Player");
	puts("2. Two Players");
	printf("Please choose your game mode: ");
	my_gets(choice, sizeof(choice));
	if (choice[0] == '1')
		run_single_player();
	else if (choice[0] == '2')
		run_two_players();
	else
		puts("Okay, seems you don't want to play...");
	return 0;
}