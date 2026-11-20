#define _CRT_SECURE_NO_WARNINGS
#include "render.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
static DWORD orig_console_mode = 0;
static UINT orig_output_cp = 0;
static int console_saved = 0;
#endif

#define CELL_WIDTH 9
#define BOARD_INNER_WIDTH (BOARD_SIZE * CELL_WIDTH + (BOARD_SIZE + 1))

typedef struct {
    int bg;
    int fg;
    int bold;
} TileStyle;

static TileStyle get_tile_style(int value) {
    switch (value) {
        case 0:    return (TileStyle){240, 245, 0};
        case 2:    return (TileStyle){230, 236, 1};
        case 4:    return (TileStyle){223, 236, 1};
        case 8:    return (TileStyle){209, 231, 1};
        case 16:   return (TileStyle){208, 231, 1};
        case 32:   return (TileStyle){203, 231, 1};
        case 64:   return (TileStyle){196, 231, 1};
        case 128:  return (TileStyle){221, 236, 1};
        case 256:  return (TileStyle){220, 236, 1};
        case 512:  return (TileStyle){214, 236, 1};
        case 1024: return (TileStyle){178, 231, 1};
        case 2048: return (TileStyle){226, 232, 1};
        default:   return (TileStyle){93,  231, 1};
    }
}

void render_init(void) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD mode = 0;
        if (GetConsoleMode(hOut, &mode)) {
            orig_console_mode = mode;
            orig_output_cp = GetConsoleOutputCP();
            console_saved = 1;
            SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
            SetConsoleOutputCP(CP_UTF8);
        }
    }
#endif
    /* Hide cursor and clear screen */
    printf("\033[?25l\033[2J\033[H");
    fflush(stdout);
}

void render_cleanup(void) {
    printf("\033[0m\033[?25h\n");
    fflush(stdout);
#ifdef _WIN32
    if (console_saved) {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            SetConsoleMode(hOut, orig_console_mode);
        }
        SetConsoleOutputCP(orig_output_cp);
        console_saved = 0;
    }
#endif
}

static void print_horizontal_border(const char *left, const char *right) {
    printf("  \033[38;5;248m%s", left);
    for (int i = 0; i < BOARD_INNER_WIDTH; ++i) {
        printf("═");
    }
    printf("%s\033[0m\033[K\n", right);
}

static void print_board_spacer(void) {
    printf("  \033[38;5;248m║\033[48;5;237m%*s\033[0m\033[38;5;248m║\033[0m\033[K\n",
           BOARD_INNER_WIDTH, "");
}

static void print_tile_slice(int value, int slice_row, bool is_new) {
    TileStyle st = get_tile_style(value);
    if (st.bold) {
        printf("\033[1;38;5;%d;48;5;%dm", st.fg, st.bg);
    } else {
        printf("\033[0;38;5;%d;48;5;%dm", st.fg, st.bg);
    }

    if (slice_row == 0) {
        if (is_new && value != 0) {
            printf("       * ");
        } else {
            printf("%*s", CELL_WIDTH, "");
        }
    } else if (slice_row == 1) {
        if (value == 0) {
            printf("    ·    ");
        } else {
            char buf[16];
            snprintf(buf, sizeof(buf), "%d", value);
            int len = (int)strlen(buf);
            int pad_total = CELL_WIDTH - len;
            int pad_left = pad_total / 2;
            int pad_right = pad_total - pad_left;
            if (pad_left < 0) pad_left = 0;
            if (pad_right < 0) pad_right = 0;
            printf("%*s%s%*s", pad_left, "", buf, pad_right, "");
        }
    } else {
        printf("%*s", CELL_WIDTH, "");
    }

    printf("\033[0m");
}

void render_game(const GameState *state, const char *status_msg) {
    /* Move cursor to top-left without full-screen flicker */
    printf("\033[H\n");

    /* Header */
    printf("  \033[1;38;5;220m   2 0 4 8   \033[0m"
           "\033[38;5;245mTerminal Edition (C)\033[0m\033[K\n\n");

    /* Scoreboard */
    printf("  \033[1;38;5;255;48;5;238m SKOR: %-7d \033[0m ", state->score);
    printf("\033[1;38;5;226;48;5;238m EN IYI: %-7d \033[0m ", state->best_score);
    if (state->last_gain > 0) {
        printf("\033[1;38;5;82m+%d\033[0m", state->last_gain);
    }
    printf("\033[K\n");

    printf("  \033[38;5;247mHamle: %-5d  |  En Buyuk: %-5d  |  Geri Al: %d/%d\033[0m\033[K\n\n",
           state->moves, game_max_tile(state), state->history_count, MAX_UNDO_HISTORY);

    /* Board top border */
    print_horizontal_border("╔", "╗");

    for (int r = 0; r < BOARD_SIZE; ++r) {
        print_board_spacer();
        for (int slice = 0; slice < 3; ++slice) {
            printf("  \033[38;5;248m║\033[48;5;237m ");
            for (int c = 0; c < BOARD_SIZE; ++c) {
                bool is_new = (r == state->last_spawn_row && c == state->last_spawn_col);
                print_tile_slice(state->grid[r][c], slice, is_new);
                printf("\033[48;5;237m ");
            }
            printf("\033[0m\033[38;5;248m║\033[0m\033[K\n");
        }
    }

    print_board_spacer();
    print_horizontal_border("╚", "╝");

    /* Status / Win / Game Over banner */
    printf("\033[K\n");
    if (state->won && !state->keep_playing) {
        printf("  \033[1;38;5;232;48;5;226m ★ TEBRIKLER! 2048'E ULASTIN! ★ \033[0m\033[K\n");
        printf("  \033[38;5;228m[C] Devam Et (Sonsuz Mod)   [R] Yeni Oyun   [Q] Cikis\033[0m\033[K\n");
    } else if (state->game_over) {
        printf("  \033[1;38;5;231;48;5;196m ✖ OYUN BITTI! Yapilacak hamle kalmadi. \033[0m\033[K\n");
        printf("  \033[38;5;210m[U] Son Hamleyi Geri Al   [R] Yeni Oyun   [Q] Cikis\033[0m\033[K\n");
    } else if (status_msg && status_msg[0] != '\0') {
        printf("  \033[38;5;117m> %s\033[0m\033[K\n", status_msg);
        printf("\033[K\n");
    } else {
        printf("  \033[38;5;245mAyni sayilari birlestir ve 2048'e ulas!\033[0m\033[K\n");
        printf("\033[K\n");
    }

    /* Controls legend */
    printf("  \033[38;5;250m[↑↓←→ / WASD]\033[38;5;244m Kaydir  "
           "\033[38;5;250m[U/Z]\033[38;5;244m Geri Al  "
           "\033[38;5;250m[R]\033[38;5;244m Yeniden  "
           "\033[38;5;250m[Q]\033[38;5;244m Cikis\033[0m\033[K\n");

    /* Clear any leftover lines below */
    printf("\033[J");
    fflush(stdout);
}
