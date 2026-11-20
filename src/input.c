#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif

#include "input.h"

#ifdef _WIN32
#include <conio.h>

void input_init(void) {
    /* _getch() requires no extra setup on Windows */
}

void input_cleanup(void) {
    /* Nothing to restore */
}

InputAction input_read_action(void) {
    int ch = _getch();

    if (ch == 0 || ch == 224) {
        int ext = _getch();
        switch (ext) {
            case 72: return ACT_UP;
            case 80: return ACT_DOWN;
            case 75: return ACT_LEFT;
            case 77: return ACT_RIGHT;
            default: return ACT_NONE;
        }
    }

    switch (ch) {
        case 'w': case 'W': return ACT_UP;
        case 's': case 'S': return ACT_DOWN;
        case 'a': case 'A': return ACT_LEFT;
        case 'd': case 'D': return ACT_RIGHT;
        case 'u': case 'U':
        case 'z': case 'Z': return ACT_UNDO;
        case 'r': case 'R': return ACT_RESTART;
        case 'c': case 'C': return ACT_CONTINUE;
        case 'q': case 'Q':
        case 27:            return ACT_QUIT;
        default:            return ACT_NONE;
    }
}

#else

#include <stdio.h>
#include <termios.h>
#include <unistd.h>

static struct termios orig_termios;
static int termios_saved = 0;

void input_init(void) {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == 0) {
        termios_saved = 1;
        struct termios raw = orig_termios;
        raw.c_lflag &= (tcflag_t)~(ICANON | ECHO);
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }
}

void input_cleanup(void) {
    if (termios_saved) {
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
        termios_saved = 0;
    }
}

InputAction input_read_action(void) {
    unsigned char ch = 0;
    if (read(STDIN_FILENO, &ch, 1) != 1) {
        return ACT_QUIT;
    }

    if (ch == 27) {
        unsigned char seq[2];
        struct termios curr;
        if (tcgetattr(STDIN_FILENO, &curr) == 0) {
            struct termios nonblock = curr;
            nonblock.c_cc[VMIN] = 0;
            nonblock.c_cc[VTIME] = 1;
            tcsetattr(STDIN_FILENO, TCSANOW, &nonblock);

            ssize_t n1 = read(STDIN_FILENO, &seq[0], 1);
            ssize_t n2 = (n1 == 1) ? read(STDIN_FILENO, &seq[1], 1) : 0;

            tcsetattr(STDIN_FILENO, TCSANOW, &curr);

            if (n1 == 1 && n2 == 1 && seq[0] == '[') {
                switch (seq[1]) {
                    case 'A': return ACT_UP;
                    case 'B': return ACT_DOWN;
                    case 'D': return ACT_LEFT;
                    case 'C': return ACT_RIGHT;
                    default:  return ACT_NONE;
                }
            }
            if (n1 == 0) {
                return ACT_QUIT;
            }
        }
        return ACT_NONE;
    }

    switch (ch) {
        case 'w': case 'W': return ACT_UP;
        case 's': case 'S': return ACT_DOWN;
        case 'a': case 'A': return ACT_LEFT;
        case 'd': case 'D': return ACT_RIGHT;
        case 'u': case 'U':
        case 'z': case 'Z': return ACT_UNDO;
        case 'r': case 'R': return ACT_RESTART;
        case 'c': case 'C': return ACT_CONTINUE;
        case 'q': case 'Q': return ACT_QUIT;
        default:            return ACT_NONE;
    }
}

#endif
