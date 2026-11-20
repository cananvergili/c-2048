#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>

#define DEFAULT_HIGHSCORE_FILE ".2048_highscore"

int storage_load_best_score(const char *filepath);
bool storage_save_best_score(const char *filepath, int best_score);

#endif /* STORAGE_H */
