#include "../src/game.h"
#include "../src/storage.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void set_grid(GameState *state, const int values[BOARD_SIZE][BOARD_SIZE]) {
    memset(state, 0, sizeof(*state));
    memcpy(state->grid, values, sizeof(state->grid));
    state->last_spawn_row = -1;
    state->last_spawn_col = -1;
}

static void test_init_spawns_two_tiles(void) {
    GameState s;
    srand(42);
    game_init(&s, 100);

    assert(s.best_score == 100);
    assert(s.score == 0);
    assert(s.moves == 0);
    assert(game_empty_count(&s) == BOARD_SIZE * BOARD_SIZE - 2);
    assert(!s.game_over);
    assert(!s.won);
}

static void test_merge_single_pass_rule(void) {
    /* [2, 2, 4, 0] moved left must become [4, 4, 0, 0] (NOT [8, 0, 0, 0]) */
    GameState s;
    const int grid[BOARD_SIZE][BOARD_SIZE] = {
        {2, 2, 4, 0},
        {2, 2, 2, 2},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    };
    srand(1);
    set_grid(&s, grid);

    assert(game_move(&s, DIR_LEFT));
    assert(s.grid[0][0] == 4);
    assert(s.grid[0][1] == 4);
    assert(s.grid[1][0] == 4);
    assert(s.grid[1][1] == 4);
    assert(s.score == 12);
    assert(s.last_gain == 12);
    assert(s.moves == 1);
}

static void test_invalid_move_does_not_change_state(void) {
    GameState s;
    const int grid[BOARD_SIZE][BOARD_SIZE] = {
        {2, 4, 8, 16},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    };
    set_grid(&s, grid);

    assert(!game_move(&s, DIR_LEFT));
    assert(!game_move(&s, DIR_UP));
    assert(s.moves == 0);
    assert(s.history_count == 0);
    assert(game_empty_count(&s) == 12);
}

static void test_undo_restores_previous_state(void) {
    GameState s;
    const int grid[BOARD_SIZE][BOARD_SIZE] = {
        {4, 4, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    };
    srand(7);
    set_grid(&s, grid);

    assert(game_move(&s, DIR_LEFT));
    assert(s.score == 8);
    assert(s.moves == 1);
    assert(s.history_count == 1);

    assert(game_undo(&s));
    assert(s.grid[0][0] == 4);
    assert(s.grid[0][1] == 4);
    assert(s.score == 0);
    assert(s.moves == 0);
    assert(s.history_count == 0);
    assert(!game_undo(&s));
}

static void test_win_detection(void) {
    GameState s;
    const int grid[BOARD_SIZE][BOARD_SIZE] = {
        {1024, 1024, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    };
    srand(3);
    set_grid(&s, grid);

    assert(game_move(&s, DIR_LEFT));
    assert(s.won);
    assert(game_max_tile(&s) == 2048);
    assert(s.score == 2048);
}

static void test_game_over_detection(void) {
    GameState s;
    const int stuck[BOARD_SIZE][BOARD_SIZE] = {
        {2,  4,  8,  16},
        {32, 64, 128, 256},
        {2,  4,  8,  16},
        {32, 64, 128, 256}
    };
    set_grid(&s, stuck);
    assert(!game_can_move(&s));

    const int mergeable[BOARD_SIZE][BOARD_SIZE] = {
        {2,  4,  8,  16},
        {32, 64, 128, 256},
        {2,  4,  8,  256},
        {32, 64, 128, 512}
    };
    set_grid(&s, mergeable);
    assert(game_can_move(&s));
}

static void test_storage_roundtrip(void) {
    const char *tmp_file = ".test_2048_score.tmp";
    assert(storage_save_best_score(tmp_file, 12345));
    assert(storage_load_best_score(tmp_file) == 12345);
    remove(tmp_file);
    assert(storage_load_best_score(tmp_file) == 0);
}

int main(void) {
    test_init_spawns_two_tiles();
    test_merge_single_pass_rule();
    test_invalid_move_does_not_change_state();
    test_undo_restores_previous_state();
    test_win_detection();
    test_game_over_detection();
    test_storage_roundtrip();

    printf("All 7 unit tests passed!\n");
    return 0;
}
