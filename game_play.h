// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "game_area.h"

class GamePlay
{
public:

    GamePlay(int line = -1, int column = -1): _line { line }, _column { column }
    {
    }

    int line() const { return _line; }
    int column() const { return _column; }

    bool in(const GameArea & area) const
    {
        return _line >= area.startLine() and _line <= area.endLine() and
               _column >= area.startColumn() and _column <= area.endLine();
    }

    friend ostream & operator << (ostream &os, const GamePlay &gamePlay);

private:

    int _line;
    int _column;

};

inline ostream & operator << (ostream &os, const GamePlay &gamePlay)
{
    os << "(" << gamePlay._line << "," << gamePlay._column << ")";
    return os;
}
