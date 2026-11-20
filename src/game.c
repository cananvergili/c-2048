#include "game.h"

#include <stdlib.h>
#include <string.h>

static void push_snapshot(GameState *state) {
    if (state->history_count >= MAX_UNDO_HISTORY) {
        memmove(&state->history[0], &state->history[1],
                sizeof(BoardSnapshot) * (MAX_UNDO_HISTORY - 1));
        state->history_count = MAX_UNDO_HISTORY - 1;
    }

    BoardSnapshot *snap = &state->history[state->history_count++];
    memcpy(snap->grid, state->grid, sizeof(state->grid));
    snap->score = state->score;
    snap->moves = state->moves;
}

void game_init(GameState *state, int best_score) {
    memset(state, 0, sizeof(*state));
    state->best_score = best_score;
    state->last_spawn_row = -1;
    state->last_spawn_col = -1;

    game_spawn_tile(state);
    game_spawn_tile(state);
}

int game_empty_count(const GameState *state) {
    int count = 0;
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            if (state->grid[r][c] == 0) {
                ++count;
            }
        }
    }
    return count;
}

bool game_spawn_tile(GameState *state) {
    int empty_cells[BOARD_SIZE * BOARD_SIZE][2];
    int count = 0;

    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            if (state->grid[r][c] == 0) {
                empty_cells[count][0] = r;
                empty_cells[count][1] = c;
                ++count;
            }
        }
    }

    if (count == 0) {
        return false;
    }

    int idx = rand() % count;
    int r = empty_cells[idx][0];
    int c = empty_cells[idx][1];

    /* 90% chance of 2, 10% chance of 4 */
    state->grid[r][c] = ((rand() % 10) == 0) ? 4 : 2;
    state->last_spawn_row = r;
    state->last_spawn_col = c;
    return true;
}

static bool slide_and_merge_line(int line[BOARD_SIZE], int *score_gain) {
    int compacted[BOARD_SIZE] = {0};
    int write_idx = 0;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (line[i] != 0) {
            compacted[write_idx++] = line[i];
        }
    }

    int result[BOARD_SIZE] = {0};
    int out_idx = 0;
    int i = 0;
    while (i < write_idx) {
        if (i + 1 < write_idx && compacted[i] == compacted[i + 1]) {
            int merged = compacted[i] * 2;
            result[out_idx++] = merged;
            *score_gain += merged;
            i += 2;
        } else {
            result[out_idx++] = compacted[i];
            i += 1;
        }
    }

    bool changed = false;
    for (int k = 0; k < BOARD_SIZE; ++k) {
        if (line[k] != result[k]) {
            changed = true;
            line[k] = result[k];
        }
    }
    return changed;
}

bool game_move(GameState *state, Direction dir) {
    if (state->game_over) {
        return false;
    }

    int prev_grid[BOARD_SIZE][BOARD_SIZE];
    memcpy(prev_grid, state->grid, sizeof(prev_grid));
    int prev_score = state->score;
    int prev_moves = state->moves;

    bool moved = false;
    int gain = 0;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        int line[BOARD_SIZE];

        for (int j = 0; j < BOARD_SIZE; ++j) {
            switch (dir) {
                case DIR_LEFT:
                    line[j] = state->grid[i][j];
                    break;
                case DIR_RIGHT:
                    line[j] = state->grid[i][BOARD_SIZE - 1 - j];
                    break;
                case DIR_UP:
                    line[j] = state->grid[j][i];
                    break;
                case DIR_DOWN:
                    line[j] = state->grid[BOARD_SIZE - 1 - j][i];
                    break;
            }
        }

        if (slide_and_merge_line(line, &gain)) {
            moved = true;
            for (int j = 0; j < BOARD_SIZE; ++j) {
                switch (dir) {
                    case DIR_LEFT:
                        state->grid[i][j] = line[j];
                        break;
                    case DIR_RIGHT:
                        state->grid[i][BOARD_SIZE - 1 - j] = line[j];
                        break;
                    case DIR_UP:
                        state->grid[j][i] = line[j];
                        break;
                    case DIR_DOWN:
                        state->grid[BOARD_SIZE - 1 - j][i] = line[j];
                        break;
                }
            }
        }
    }

    if (!moved) {
        return false;
    }

    /* Save previous state to undo stack */
    int current_grid[BOARD_SIZE][BOARD_SIZE];
    memcpy(current_grid, state->grid, sizeof(current_grid));
    memcpy(state->grid, prev_grid, sizeof(prev_grid));
    state->score = prev_score;
    state->moves = prev_moves;
    push_snapshot(state);
    memcpy(state->grid, current_grid, sizeof(current_grid));

    state->score += gain;
    state->last_gain = gain;
    state->moves += 1;

    if (state->score > state->best_score) {
        state->best_score = state->score;
    }

    game_spawn_tile(state);

    if (!state->won && game_max_tile(state) >= WIN_TILE) {
        state->won = true;
    }

    state->game_over = !game_can_move(state);
    return true;
}

bool game_undo(GameState *state) {
    if (state->history_count <= 0) {
        return false;
    }

    const BoardSnapshot *snap = &state->history[--state->history_count];
    memcpy(state->grid, snap->grid, sizeof(state->grid));
    state->score = snap->score;
    state->moves = snap->moves;
    state->last_gain = 0;
    state->last_spawn_row = -1;
    state->last_spawn_col = -1;
    state->game_over = false;

    if (game_max_tile(state) < WIN_TILE) {
        state->won = false;
        state->keep_playing = false;
    }

    return true;
}

bool game_can_move(const GameState *state) {
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            int val = state->grid[r][c];
            if (val == 0) {
                return true;
            }
            if (r + 1 < BOARD_SIZE && state->grid[r + 1][c] == val) {
                return true;
            }
            if (c + 1 < BOARD_SIZE && state->grid[r][c + 1] == val) {
                return true;
            }
        }
    }
    return false;
}

int game_max_tile(const GameState *state) {
    int max_val = 0;
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            if (state->grid[r][c] > max_val) {
                max_val = state->grid[r][c];
            }
        }
    }
    return max_val;
}
