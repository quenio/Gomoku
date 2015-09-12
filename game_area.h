// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

static constexpr int LINE_COUNT = 15;
static constexpr int COLUMN_COUNT = 15;

class GameArea
{
public:

    GameArea(int startLine, int startColumn, int endLine, int endColumn):
        _startLine { imax(0, startLine) }, _startColumn { imax(0, startColumn) },
        _endLine { imin(endLine, LINE_COUNT - 1) }, _endColumn { imin(endColumn, COLUMN_COUNT - 1) }
    {
    }

    GameArea(int start, int length):
            _startLine { start }, _startColumn { start },
            _endLine { start + length - 1 }, _endColumn { start + length - 1 }
    {
    }

    int startLine() const { return _startLine; }
    int startColumn() const { return _startColumn; }
    int endLine() const { return _endLine; }
    int endColumn() const { return _endColumn; }

    int width() const { return endColumn() - startColumn() + 1; }
    int height() const { return endLine() - startLine() + 1; }

    int slotCount() const { return width() * height(); }

    friend ostream & operator << (ostream & os, const GameArea & area);

private:

    int _startLine, _startColumn, _endLine, _endColumn;

};

static constexpr int WINNING_COUNT = 5;
static constexpr int FOCUS_LENGTH = 6;

static const GameArea FULL_BOARD { 0, 0, LINE_COUNT - 1, COLUMN_COUNT - 1 };
static const GameArea CENTRAL_AREA { 3, FOCUS_LENGTH };

inline ostream & operator << (ostream & os, const GameArea & area)
{
    os << "(" << area._startLine << "," << area._startColumn << "," << area._endLine << "," << area._endColumn << ")";

    return os;
}
