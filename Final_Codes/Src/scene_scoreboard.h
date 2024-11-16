#ifndef SCENE_SCOREBOARD_H
#define SCENE_SCOREBOARD_H
#include "game.h"
#include "shared.h"
#include "scene_menu.h"

typedef struct Score {
    char name[13];
    int point;
} Score;

bool justFinished;
int justScore;

Scene scene_scoreboard_create(void);

#endif