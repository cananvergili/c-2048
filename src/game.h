#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stddef.h>

#define BOARD_SIZE 4
#define MAX_UNDO_HISTORY 50
#define WIN_TILE 2048

typedef enum {
    DIR_UP = 0,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction;

typedef struct {
    int grid[BOARD_SIZE][BOARD_SIZE];
    int score;
    int moves;
} BoardSnapshot;

typedef struct {
    int grid[BOARD_SIZE][BOARD_SIZE];
    int score;
    int best_score;
    int last_gain;
    int moves;
    int last_spawn_row;
    int last_spawn_col;
    bool won;
    bool keep_playing;
    bool game_over;

    BoardSnapshot history[MAX_UNDO_HISTORY];
    int history_count;
} GameState;

void game_init(GameState *state, int best_score);
bool game_spawn_tile(GameState *state);
bool game_move(GameState *state, Direction dir);
bool game_undo(GameState *state);
bool game_can_move(const GameState *state);
int game_max_tile(const GameState *state);
int game_empty_count(const GameState *state);

#endif /* GAME_H */
