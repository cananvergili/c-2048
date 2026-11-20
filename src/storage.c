#define _CRT_SECURE_NO_WARNINGS
#include "storage.h"

#include <stdio.h>

int storage_load_best_score(const char *filepath) {
    if (!filepath) {
        return 0;
    }

    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        return 0;
    }

    int score = 0;
    if (fscanf(fp, "%d", &score) != 1 || score < 0) {
        score = 0;
    }

    fclose(fp);
    return score;
}

bool storage_save_best_score(const char *filepath, int best_score) {
    if (!filepath || best_score < 0) {
        return false;
    }

    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        return false;
    }

    int written = fprintf(fp, "%d\n", best_score);
    fclose(fp);
    return written > 0;
}
