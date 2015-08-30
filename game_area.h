// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

class GameArea
{
public:

    GameArea(int startLine, int startColumn, int endLine, int endColumn):
        _startLine { startLine }, _startColumn { startColumn },
        _endLine { endLine }, _endColumn { endColumn }
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

private:

    const int _startLine, _startColumn, _endLine, _endColumn;

};

static const int LINE_COUNT = 15;
static const int COLUMN_COUNT = 15;

static const GameArea FULL_BOARD { 0, 0, LINE_COUNT - 1, COLUMN_COUNT - 1 };
static const GameArea CENTRAL_AREA { 6, 5 };

