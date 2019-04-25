// Copyright (c) 2019, William TANG <galaxyking0419@gmail.com>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "reversi.h"

static void my_gets(char *restrict const buffer, size_t buffer_len)
{
	char c;
	size_t bytes_read = 0;
	while ((c = getchar()) != EOF && c != '\n') {
		if (bytes_read < buffer_len - 1) {
			buffer[bytes_read++] = c;
		}
	}
	buffer[bytes_read];
}

static inline uint_fast8_t your_oppenent(uint_fast8_t player)
{
	if (player == 1)
		return 2;
	return 1;
}

static inline bool on_board(uint_fast8_t row, uint_fast8_t column)
{
	return (row >= 0 && row <=7) && (column >=0 && column <= 7);
}

static uint_fast8_t find_mapping_value(char pos)
{
	for (size_t i=0; i<8; i++)
		if (pos == pos_map[i][0])
			return atoi(&pos_map[i][1]);
}

static cord_t position(char *restrict const input)
{
	cord_t invalid_pos = {8, 8};
	if (strchr(valid_column, input[0]) != NULL
		&& strchr(valid_row, input[1]) != NULL) {
			cord_t valid_pos = {atoi(&input[1]) - 1, find_mapping_value(input[0])};
			return valid_pos;
	}
	return invalid_pos;
}

static void print_board(board_t game_board, uint_fast8_t black_score, uint_fast8_t white_score)
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

static void score(board_t game_board, uint_fast8_t *restrict const black_score, uint_fast8_t *restrict const white_score)
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

static bool enclosing(board_t game_board, uint_fast8_t player, cord_t pos, cord_t direction)
{
	uint_fast8_t row = pos.row + direction.row;
	uint_fast8_t column = pos.column + direction.column;

	while (on_board(row, column) 
	&& game_board.board_matrix[row][column] == your_oppenent(player)) {
		row += direction.row;
		column += direction.column;
		if (on_board(row, column) && game_board.board_matrix[row][column] == player)
			return true;
	}
	return false;
}

static uint_fast8_t valid_moves(board_t game_board, uint_fast8_t player, cord_t *restrict valid_cords)
{
	uint_fast8_t mem_ctr = 0;

	for (size_t i=0; i<8; i++) {
		for (size_t j=0; j<8; j++) {
			for (size_t z=0; z<8; z++) {
				cord_t pos = {i, j};
				cord_t dir = {valid_direction[z][0], valid_direction[z][1]};
				if (game_board.board_matrix[i][j] == 0
				&& enclosing(game_board, player, pos, dir)) {
					valid_cords = (cord_t *)realloc(valid_cords, sizeof(cord_t) * (mem_ctr+1));
					(valid_cords+mem_ctr)->row = i;
					(valid_cords+mem_ctr)->column = j;
					mem_ctr++;
				}
			}
		}
	}
	return mem_ctr;
}

static void next_state(board_t *restrict const game_board, uint_fast8_t *restrict const player, cord_t pos)
{
	game_board->board_matrix[pos.row][pos.column] = *player;
	for (size_t i=0; i<8; i++) {
		cord_t dir = {valid_direction[i][0], valid_direction[i][1]};
		if (enclosing(*game_board, *player, pos, dir)) {
			for (size_t j=1; game_board->board_matrix[pos.row + j*dir.row][pos.column + j*dir.column] != *player; j++) {
				game_board->board_matrix[pos.row + j*dir.row][pos.column + j*dir.column] = *player;
			}
		}
	}
	*player = your_oppenent(*player);
	cord_t *restrict moves = (cord_t *)malloc(sizeof(cord_t));
	if (valid_moves(*game_board, *player, moves) == 0) {
		*player = 0;
		return;
	}
}

static cord_t promt_to_place(board_t game_board, uint_fast8_t player)
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

	cord_t *restrict moves = (cord_t *)malloc(sizeof(cord_t));
	cord_t converted_pos = position(pos);
	size_t lenghth = valid_moves(game_board, player, moves);
	for (size_t i=0; i<lenghth; i++)
		if (converted_pos.row == (moves+i)->row 
		&& converted_pos.column == (moves+i)->column)
			return converted_pos;
	puts("Come on, man.");
	return promt_to_place(game_board, player);
}

static void finish_game(uint_fast8_t black_score, uint_fast8_t white_score)
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
	uint_fast8_t player = 1;
	uint_fast8_t black_score = 2;
	uint_fast8_t white_score = 2;
	cord_t current_pos = {8, 8};
	board_t in_game_board = {{
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
		puts("\n");
		print_board(in_game_board, black_score, white_score);
		current_pos = promt_to_place(in_game_board, player);
		next_state(&in_game_board, &player, current_pos);
		score(in_game_board, &black_score, &white_score);
	}
	print_board(in_game_board, black_score, white_score);
	finish_game(black_score, white_score);
}

static cord_t ai_place(board_t game_board, uint_fast8_t level)
{
	uint_fast8_t enclose_index = 0;
	cord_t *restrict moves = (cord_t *)malloc(sizeof(cord_t));
	uint_fast8_t length = valid_moves(game_board, 2, moves);
	if (level != 2) {
		uint_fast8_t enclose;
		if (level == 1)
			enclose = 8;
		else
			enclose = 0;
		
		for (size_t i=0; i<length; i++){
			uint_fast8_t test_player = 2;
			uint_fast8_t black_score = 0;
			uint_fast8_t white_score = 0;
			board_t test_board = game_board;
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

static void run_single_player(uint_fast8_t level)
{
	uint_fast8_t player = 1;
	uint_fast8_t black_score = 2;
	uint_fast8_t white_score = 2;
	board_t in_game_board = {{
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 2, 1, 0, 0, 0},
		{0, 0, 0, 1, 2, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
	}};
	srand(time(NULL));

	while (player != 0) {
		cord_t current_pos = {8, 8};
		print_board(in_game_board, black_score, white_score);
		if (player == 1){
			current_pos = promt_to_place(in_game_board, player);
			puts("\n");
		}
		else {
			puts("\n");
			uint_fast8_t randnum = rand() % MAX_ROBOT_SENTENCE;
			printf("%s\n", sentence_of_robot[randnum]);
			puts("\n");
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
int_fast8_t main(void)
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
		if (strchr(valid_difficulty, level[0]) != NULL) {
			run_single_player(atoi(&level[0]));
		} else {
			puts("Don't mess up with me! Byebye!");
		}
	} else if (choice[0] == '2') {
		run_two_players();
	} else {
		puts("Don't mess up with me! Byebye!");
	}

	puts("Press enter to exit...");
	getchar();
	return 0;
}