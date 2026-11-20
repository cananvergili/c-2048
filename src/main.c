#include "game.h"
#include "input.h"
#include "render.h"
#include "storage.h"

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    srand((unsigned int)time(NULL));

    int best_score = storage_load_best_score(DEFAULT_HIGHSCORE_FILE);
    GameState state;
    game_init(&state, best_score);

    render_init();
    input_init();

    const char *status_msg = "Hos geldin! Yon tuslari veya WASD ile oyna.";
    bool running = true;

    while (running) {
        render_game(&state, status_msg);
        status_msg = "";

        InputAction action = input_read_action();

        if (state.won && !state.keep_playing) {
            if (action == ACT_CONTINUE) {
                state.keep_playing = true;
                status_msg = "Sonsuz mod aktif! Daha yuksek skorlara devam.";
                continue;
            } else if (action == ACT_RESTART) {
                game_init(&state, state.best_score);
                status_msg = "Yeni oyun baslatildi.";
                continue;
            } else if (action == ACT_UNDO) {
                if (game_undo(&state)) {
                    status_msg = "Son hamle geri alindi.";
                }
                continue;
            } else if (action == ACT_QUIT) {
                running = false;
                continue;
            } else {
                continue;
            }
        }

        switch (action) {
            case ACT_UP:
            case ACT_DOWN:
            case ACT_LEFT:
            case ACT_RIGHT: {
                Direction dir = DIR_UP;
                if (action == ACT_DOWN) dir = DIR_DOWN;
                else if (action == ACT_LEFT) dir = DIR_LEFT;
                else if (action == ACT_RIGHT) dir = DIR_RIGHT;

                int prev_best = state.best_score;
                if (game_move(&state, dir)) {
                    if (state.best_score > prev_best) {
                        storage_save_best_score(DEFAULT_HIGHSCORE_FILE, state.best_score);
                    }
                } else if (!state.game_over) {
                    status_msg = "Bu yone hareket edilemiyor.";
                }
                break;
            }

            case ACT_UNDO:
                if (game_undo(&state)) {
                    status_msg = "Son hamle geri alindi.";
                } else {
                    status_msg = "Geri alinacak hamle yok.";
                }
                break;

            case ACT_RESTART:
                game_init(&state, state.best_score);
                status_msg = "Oyun sifirlandi.";
                break;

            case ACT_QUIT:
                running = false;
                break;

            case ACT_CONTINUE:
            case ACT_NONE:
            default:
                break;
        }
    }

    storage_save_best_score(DEFAULT_HIGHSCORE_FILE, state.best_score);
    input_cleanup();
    render_cleanup();
    return 0;
}
