// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "integer_math.h"
#include "player_marker.h"

typedef long Score;

constexpr Score scoreOf(const Score & base, const int & seqCount)
{
    return ipow(base, seqCount);
}

constexpr Score scoreOf(const PlayerMarker & playerMarker, const Score & score)
{
    return (playerMarker == X ? +1 : -1) * score;
}

constexpr Score fullScoreOf(const Score & base, const int & seqCount)
{
    int score = 0;
    int count = seqCount;

    while (count > 0)
    {
        score += scoreOf(base, count--);
    }

    return score;
}

constexpr Score DRAW = 0; // The game board has come to a draw.
constexpr Score CLOSER_TO_CENTER = 1; // Positions will score higher closer to the center.
constexpr Score EMPTY_BLOCKED = 2; // A blocked line should be worth less than a free one.
constexpr Score FILLED_BLOCKED = 3; // A truly blocked line should be worth less than a free one.
constexpr Score EMPTY_POSITION = 5; // An empty position is just a possibility.
constexpr Score SINGLE_MARK = 4 * EMPTY_POSITION; // A single mark on the game board.
constexpr Score WIN = fullScoreOf(SINGLE_MARK, WINNING_COUNT); // The game board has a victory.

constexpr int DIRECTION_COUNT = 8;

constexpr Score MAX_SCORE = scoreOf(X, DIRECTION_COUNT * WIN);
constexpr Score MIN_SCORE = scoreOf(O, DIRECTION_COUNT * WIN);

