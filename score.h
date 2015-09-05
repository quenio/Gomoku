// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "integer_math.h"
#include "player_marker.h"

typedef long Score;

constexpr Score scoreOf(const Score & base, const Score & seqCount)
{
    return ipow(base, seqCount * 2); // Multiplying by 2 so that there is enough room between victory and several 4-count sequences.
}

constexpr Score scoreOf(const PlayerMarker & playerMarker, const Score & score)
{
    return (playerMarker == X ? +1 : -1) * score;
}

constexpr Score EMPTY_POSITION = 5; // An empty position is just a possibility.
constexpr Score SINGLE_MARK = 10; // A single mark on the game board.
constexpr Score INTERMITTENT = 2; // Intermittent lines score higher because they can be overlooked by the adversary.
constexpr Score BLOCKED = 2; // A blocked line should be worth less than a free one.
constexpr Score CLOSER_TO_CENTER = 1; // Positions will score higher closer to the center.
constexpr Score DRAW = 0; // The game board has come to a draw.
constexpr Score WIN = scoreOf(SINGLE_MARK, WINNING_COUNT); // The game board has a victory.

constexpr Score MAX_SCORE = scoreOf(X, WIN);
constexpr Score MIN_SCORE = scoreOf(O, WIN);

