// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

class GamePlay
{
public:

    GamePlay(int line = -1, int column = -1): _line { line }, _column { column }
    {
    }

    int line() const { return _line; }
    int column() const { return _column; }

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
