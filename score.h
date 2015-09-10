// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "integer_math.h"
#include "player_marker.h"

typedef long Score;

constexpr Score scoreOf(const PlayerMarker & playerMarker, const Score & base, const int & seqCount)
{
    return (playerMarker == X ? +1 : -1) * ipow(base, seqCount);
}

constexpr Score fullScoreOf(const PlayerMarker & playerMarker, const Score & base, const int & seqCount)
{
    int score = 0;
    int count = seqCount;

    while (count > 0)
    {
        score += scoreOf(playerMarker, base, count--);
    }

    return score;
}

constexpr Score DRAW = 0; // The game board has come to a draw.
constexpr Score CLOSER_TO_CENTER = 1; // Positions will score higher closer to the center.
constexpr Score EMPTY_POSITION = 2; // An empty position is just a possibility.
constexpr Score SINGLE_MARK = 10; // A single mark on the game board.
constexpr Score BLOCKED = 4; // A blocked line should neutralize the effect of the opponent's sequence.

constexpr Score MAX_SCORE = fullScoreOf(X, SINGLE_MARK, WINNING_COUNT);
constexpr Score MIN_SCORE = fullScoreOf(O, SINGLE_MARK, WINNING_COUNT);

