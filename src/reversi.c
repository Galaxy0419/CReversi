// Copyright (c) 2019, William TANG <galaxyking0419@gmail.com>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include "reversi.h"

#ifdef WIN
	#include <windows.h>
#else
	#include <unistd.h>
#endif

static inline size_t input(char **string, char *restrict const prompt, size_t init_size, size_t step_size)
{
	char c;
	size_t length = 0;
	*string = (char *)malloc(sizeof(char) * init_size);
	printf("%s", prompt);
	while ((c = getchar()) != EOF && c != '\n') {
		if (length == init_size) {
			*string = (char *)realloc(*string, sizeof(init_size+=step_size));
			if (! *string) {
				perror("Memory ERROR");
				exit(EXIT_FAILURE);
			}
		}
		(*string)[length++] = c;
	}
	(*string)[length] = '\0';
	*string = (char *)realloc(*string, sizeof(char) * length);
	return length;
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
	for (size_t i=0; i<8; i++) {
		if (pos == pos_map[i][0])
			return atoi(&pos_map[i][1]);
	}
	puts("Unexpect ERROR in find_mapping_value()");
	exit(2);
}

static cord_t position(char *restrict const input)
{
	if (strchr(valid_column, input[0]) != NULL && strchr(valid_row, input[1]) != NULL) {
		return (cord_t){atoi(input+1) - 1, find_mapping_value(input[0])};
	}
	return (cord_t){8, 8};
}

static void print_board(board_t game_board, uint_fast8_t black_score, uint_fast8_t white_score)
{
	
	for (size_t i=0; i<8; i++) {
		printf("   %ld |", i+1);
		for (size_t j=0; j<8; j++) {
			if (game_board.board_matrix[i][j] == 1)
				printf(" B");
			else if (game_board.board_matrix[i][j] == 2)
				printf(" W");
			else
				printf(" -");
		}
		if (i == 3)
			printf("    Black Score: %d", black_score);
		else if (i == 5)
			printf("    White Score: %d", white_score);
		printf("\n");
	}
	printf("   --------------------\n");
	printf("       a b c d e f g h\n");
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

static void *restrict const valid_moves(void *restrict const para)
{
	for (size_t i=0; i<8; i++) {
		for (size_t j=0; j<8; j++) {
			if (((v_mov_t *)para)->game_board.board_matrix[i][j] == 0) {
				for (size_t z=0; z<8; z++) {
					cord_t pos = {i, j};
					cord_t dir = {valid_direction[z][0], valid_direction[z][1]};
					if (enclosing(((v_mov_t *)para)->game_board, ((v_mov_t *)para)->player, pos, dir)) {
						((v_mov_t *)para)->valid_cords = (cord_t *)realloc(((v_mov_t *)para)->valid_cords, sizeof(cord_t) * (*(((v_mov_t *)para)->length)+1));
						if (! ((v_mov_t *)para)->valid_cords) {
							perror("Memory error");
							exit(1);
						}
						((((v_mov_t *)para)->valid_cords) + *(((v_mov_t *)para)->length))->row = i;
						((((v_mov_t *)para)->valid_cords) + *(((v_mov_t *)para)->length))->column = j;
						(*(((v_mov_t *)para)->length))++;
					}
				}
			}
		}
	}
	return NULL;
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
	cord_t *restrict moves = (cord_t *)malloc(sizeof(cord_t));
	if (! moves) {
		perror("Memory error");
		exit(1);
	}

	size_t length = 0;
	v_mov_t para = {*player, *game_board, moves, &length};
	valid_moves((void *)&para);
	if (length == 0) {
		*player = 0;
	} else {
		*player = your_oppenent(*player);
	}
	free(moves);
}

static cord_t prompt_to_place(board_t game_board, uint_fast8_t player)
{
	char *pos;
	char cplayer;
	bool is_join = false;
	size_t length = 0;
	cord_t converted_pos = {8, 8};
	cord_t *moves = (cord_t *)malloc(sizeof(cord_t));
	if (! moves) {
		perror("Memory error");
		exit(1);
	}
	v_mov_t para = {player, game_board, moves, &length};

	pthread_t v_mov_thread;
	pthread_create(&v_mov_thread, NULL, valid_moves, &para);

	if (player == 1)
		cplayer = 'B';
	else
		cplayer = 'W';

	while (converted_pos.row == 8) {
		printf("\n (%c) Enter the position to place your disk: ", cplayer);
		size_t read = input(&pos, "", 4, 4);
		if (pos[0] == 'q') {
			puts("Thanks for playing C Reversi.");
			exit(0);
		}
		converted_pos = position(pos);

		if (read != 2 || converted_pos.row == 8) {
			puts("(-_-) Come on, you little naughty boy.");
			converted_pos.row = 8;
			continue;
		}

		if (! is_join) {
			pthread_join(v_mov_thread, NULL);
			is_join = true;
		}

		for (size_t i = 0; i < length; i++) {
			if (converted_pos.row == ((para.valid_cords)+i)->row 
			&& converted_pos.column == ((para.valid_cords)+i)->column) {
				free(moves);
				return converted_pos;
			}
		}
		puts("(-_-) Come on, you little naughty boy.");
		converted_pos.row = 8;
	}
	puts("Unexpected ERROR in promt_to_place()");
	exit(3);
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

	puts("Press enter to exit...");
	getchar();
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
		current_pos = prompt_to_place(in_game_board, player);
		next_state(&in_game_board, &player, current_pos);
		score(in_game_board, &black_score, &white_score);
	}
	puts("\n");
	print_board(in_game_board, black_score, white_score);
	finish_game(black_score, white_score);
}

static cord_t ai_place(board_t game_board, uint_fast8_t level)
{
	size_t length = 0;
	uint_fast8_t enclose_index = 0;
	cord_t return_value;
	cord_t *restrict moves = (cord_t *)malloc(sizeof(cord_t));
	if (! moves) {
		perror("Memory error");
		exit(1);
	}

	v_mov_t para = {2, game_board, moves, &length};

	pthread_t v_mov_thread;
	pthread_create(&v_mov_thread, NULL, valid_moves, (void *)&para);

	if (level != 2) {
		uint_fast8_t enclose;
		if (level == 1)
			enclose = 8;
		else
			enclose = 0;
		
		pthread_join(v_mov_thread, NULL);

		for (size_t i = 0; i < length; i++){
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
	return_value = *(para.valid_cords+enclose_index);
	free(moves);
	return return_value;
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
		if (player == 1) {
			current_pos = prompt_to_place(in_game_board, player);
			puts("\n");
		} else {
			puts("\n");
			uint_fast8_t randnum = rand() % MAX_ROBOT_SENTENCE;
			printf("Mr. Win: %s\n", sentence_of_robot[randnum]);
			puts("\n");
#ifdef WIN
			Sleep(4000);
#else
			sleep(4);
#endif
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
	char *choice;
	puts("Welcome to C Reversi!");
	puts("1. Single Player");
	puts("2. Two Players");
	size_t read_len = input(&choice, "Please choose your game mode: ", 1, 4);
	if (read_len == 1) {
		if (choice[0] == '1') {
			puts("1. Noob");
			puts("2. Average");
			puts("3. Asian");
			read_len = input(&choice, "Please choose a difficulty: ", 1, 4);
			if (read_len == 1 && atoi(choice) > 0 && atoi(choice) < 4) {
				run_single_player(atoi(choice));
			}
		} else if (choice[0] == '2') {
			free(choice);
			run_two_players();
		}
	}
	puts("Don't mess up with me! Byebye!");
	return 0;
}