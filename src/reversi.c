// Copyright (c) 2019, William TANG <galaxyking0419@gmail.com>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "reversi.h"

static void my_gets(char *restrict buffer, size_t buffer_len)
{
	char c;
	int bytes_read = 0;
	while ((c = getchar()) != EOF && c != '\n') {
		if (bytes_read < buffer_len - 1) {
			buffer[bytes_read++] = c;
		}
	}
	buffer[bytes_read];
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

static cord position(char *restrict input)
{
	cord invalid_pos = {-1, -1};
	if (strchr(valid_column, input[0]) != NULL
		&& strchr(valid_row, input[1]) != NULL) {
			cord valid_pos = {atoi(&input[1]) - 1, find_mapping_value(input[0])};
			return valid_pos;
	}
	return invalid_pos;
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

static void score(board game_board, int *restrict black_score, int *restrict white_score)
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

static int valid_moves(board game_board, int player, cord *restrict valid_cords)
{
	int mem_ctr = 0;

	for (size_t i=0; i<8; i++) {
		for (size_t j=0; j<8; j++) {
			for (size_t z=0; z<8; z++) {
				cord pos = {i, j};
				cord dir = {valid_direction[z][0], valid_direction[z][1]};
				if (game_board.board_matrix[i][j] == 0
				&& enclosing(game_board, player, pos, dir)) {
					valid_cords = (cord *)realloc(valid_cords, sizeof(cord) * (mem_ctr+1));
					(valid_cords+mem_ctr)->row = i;
					(valid_cords+mem_ctr)->column = j;
					mem_ctr++;
				}
			}
		}
	}
	return mem_ctr;
}

static void next_state(board *restrict game_board, int *restrict player, cord pos)
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
	*player = your_oppenent(*player);
	cord *restrict moves = (cord *)malloc(sizeof(cord));
	if (valid_moves(*game_board, *player, moves) == 0) {
		*player = 0;
		return;
	}
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

	cord *restrict moves = (cord *)malloc(sizeof(cord));
	cord converted_pos = position(pos);
	int lenghth = valid_moves(game_board, player, moves);
	for (size_t i=0; i<lenghth; i++)
		if (converted_pos.row == (moves+i)->row 
		&& converted_pos.column == (moves+i)->column)
			return converted_pos;
	puts("Come on, man.");
	return promt_to_place(game_board, player);
}

static void finish_game(int black_score, int white_score)
{
	puts("\n");
	puts("The game has finished!\n");
	printf("Black Score: %d\n", black_score);
	printf("White Score: %d\n\n", white_score);

	if (black_score > white_score)
		puts("Black Wins!");
	else if (black_score == white_score)
		puts("Draw!");
	else
		puts("White Wins!");
	sleep(4);
	exit(0);
}

static void run_two_players(void)
{
	int player = 1;
	int black_score = 2;
	int white_score = 2;
	cord current_pos = {-1, -1};
	board in_game_board = {{
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 2, 1, 0, 0, 0},
		{0, 0, 0, 1, 2, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
	}};

	while (player != 0) {
		print_board(in_game_board, black_score, white_score);
		current_pos = promt_to_place(in_game_board, player);
		next_state(&in_game_board, &player, current_pos);
		score(in_game_board, &black_score, &white_score);
		puts("\n");
	}
	print_board(in_game_board, black_score, white_score);
	finish_game(black_score, white_score);
}

static cord ai_place(board game_board, int level)
{
	int enclose_index = 0;
	cord *restrict moves = (cord *)malloc(sizeof(cord));
	int length = valid_moves(game_board, 2, moves);
	if (level != 2) {
		int enclose;
		if (level == 1)
			enclose = 8;
		else
			enclose = 0;
		
		for (size_t i=0; i<length; i++){
			int test_player = 2;
			int black_score = 0;
			int white_score = 0;
			board test_board = game_board;
			next_state(&test_board, &test_player, *(moves+i));
			score(test_board, &black_score, &white_score);
			if (level == 3 && white_score > enclose) {
				enclose = white_score;
				enclose_index = i;
			} else if (level == 1 && white_score < enclose) {
				enclose = white_score;
				enclose_index = i;
			}	
		}
	} else {
		enclose_index = rand() % length;
	}
	return *(moves+enclose_index);
}

static void run_single_player(int level)
{
	int player = 1;
	int black_score = 2;
	int white_score = 2;
	board in_game_board = {{
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 2, 1, 0, 0, 0},
		{0, 0, 0, 1, 2, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
	}};

	while (player != 0) {
		cord current_pos = {-1, -1};
		print_board(in_game_board, black_score, white_score);
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
			current_pos = ai_place(in_game_board, level);
		}
		next_state(&in_game_board, &player, current_pos);
		score(in_game_board, &black_score, &white_score);
	}
	print_board(in_game_board, black_score, white_score);
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
	if (choice[0] == '1') {
		char level[2];
		puts("1. Noob");
		puts("2. Average");
		puts("3. Asian");
		printf("Please choose a difficulty: ");
		my_gets(level, sizeof(level));
		if (strchr(valid_difficulty, level[0]) == NULL) {
			puts("Don't mess up with me! Byebye!");
			exit(1);
		}
		run_single_player(atoi(&level[0]));
	}
	else if (choice[0] == '2'){
		run_two_players();
	}
	else{
		puts("Don't mess up with me! Byebye!");
	}
	return 0;
}