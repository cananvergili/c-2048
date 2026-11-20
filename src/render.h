#ifndef RENDER_H
#define RENDER_H

#include "game.h"

void render_init(void);
void render_cleanup(void);
void render_game(const GameState *state, const char *status_msg);

#endif /* RENDER_H */
