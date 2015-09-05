// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "game_area.h"

enum Direction { North, Northeast, East, Southeast, South, Southwest, West, Northwest };

class GamePosition
{
public:

    GamePosition(int line = -1, int column = -1): _line { line }, _column { column }
    {
    }

    int line() const { return _line; }
    int column() const { return _column; }

    bool valid() const
    {
        return _line >= 0 and _line < LINE_COUNT and _column >= 0 and _column < COLUMN_COUNT;
    }

    bool in(const GameArea & area) const
    {
        return _line >= area.startLine() and _line <= area.endLine() and
               _column >= area.startColumn() and _column <= area.endLine();
    }

    GamePosition neighbor(const Direction & direction, const int & step) const
    {
        int newLine = _line, newColumn = _column;

        switch (direction)
        {
            case North:
                newLine -= step;
                break;

            case Northeast:
                newLine -= step;
                newColumn += step;
                break;

            case East:
                newColumn += step;
                break;

            case Southeast:
                newLine += step;
                newColumn += step;
                break;

            case South:
                newLine += step;
                break;

            case Southwest:
                newLine += step;
                newColumn -= step;
                break;

            case West:
                newColumn -= step;
                break;

            case Northwest:
                newLine -= step;
                newColumn -= step;
                break;
        }

        return GamePosition { newLine, newColumn };
    }

    bool operator == (const GamePosition & position) const
    {
        return _line == position._line and _column == position._column;
    }

    bool operator != (const GamePosition & position) const
    {
        return _line != position._line or _column != position._column;
    }

    friend ostream & operator << (ostream &os, const GamePosition &position);

private:

    int _line;
    int _column;

};

static const GamePosition INVALID_POSITION;

inline ostream & operator << (ostream &os, const GamePosition &position)
{
    os << "(" << position._line << "," << position._column;

    if (position.valid())
    {
        os << ":" << char('A' + position._column) << (position._line + 1) <<  ")";
    }
    else
    {
        os << ")";
    }

    return os;
}

inline ostream & operator << (ostream &os, const Direction &direction)
{
    switch (direction)
    {
        case North:
            os << "North";
            break;

        case Northeast:
            os << "Northeast";
            break;

        case East:
            os << "East";
            break;

        case Southeast:
            os << "Southeast";
            break;

        case South:
            os << "South";
            break;

        case Southwest:
            os << "Southwest";
            break;

        case West:
            os << "West";
            break;

        case Northwest:
            os << "Northwest";
            break;
    }

    return os;
}
