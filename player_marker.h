// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "base.h"

enum PlayerMarker {
    X, O
};

inline PlayerMarker opponentOf(const PlayerMarker & playerMaker)
{
    return playerMaker == X ? O : X;
}

inline bool maxTurn(const PlayerMarker & playerMaker)
{
    return playerMaker == X;
}


inline ostream & operator << (ostream &os, const PlayerMarker &marker)
{
    os << (marker == X ? "X" : "O");
    return os;
}
