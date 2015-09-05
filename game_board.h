// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "game_slot.h"
#include "game_position.h"

class GameBoard
{
public:

    bool isGameOver() const
    {
        return hasWinner() or emptyPositions().size() == 0;
    }

    bool hasWinner() const
    {
        PlayerMarker playerMarker;

        return lineVictory(playerMarker) or columnVictory(playerMarker);
    }

    bool isDraw() const
    {
        return isGameOver() and not hasWinner();
    }

    PlayerMarker winner() const
    {
        PlayerMarker playerMarker;

        if (lineVictory(playerMarker)) return playerMarker;
        if (columnVictory(playerMarker)) return playerMarker;

        throw runtime_error { "Game has no winner yet." };
    }

    bool lineVictory(PlayerMarker & playerMarker) const
    {
        for (int line = 0; line < LINE_COUNT; line++)
        {
            int count = 1;
            for (int column = 0; column < COLUMN_COUNT - 1; column++)
            {
                count = (_slots[line][column] == _slots[line][column + 1] ? count + 1 : 0);
                if (count == WINNING_COUNT)
                {
                    playerMarker = _slots[line][column]._playerMarker;
                    return true;
                }
            }
        }
        return false;
    }

    bool columnVictory(PlayerMarker & playerMarker) const
    {
        for (int column = 0; column < COLUMN_COUNT; column++)
        {
            int count = 0;
            for (int line = 0; line < LINE_COUNT - 1; line++)
            {
                count = (_slots[line][column] == _slots[line + 1][column] ? count + 1 : 0);
                if (count == WINNING_COUNT)
                {
                    playerMarker = _slots[line][column]._playerMarker;
                    return true;
                }
            }
        }
        return false;
    }

    // TODO Implement diagonalVictory()

    GameBoard play(const GamePosition & position, const PlayerMarker & playerMarker) const
    {
        checkRangeOf(position);

        GameBoard newGameBoard { *this };

        newGameBoard._slots[position.line()][position.column()].mark(playerMarker);

        return newGameBoard;
    }

    vector<GamePosition> emptyPositions(const GameArea &area = FULL_BOARD) const
    {
        vector<GamePosition> positions;

        for (int line = area.startLine(); line <= area.endLine(); line++)
        {
            for (int column = area.startColumn(); column <= area.endColumn(); column++)
            {
                if (_slots[line][column].empty())
                {
                    positions.push_back(GamePosition { line, column });
                }
            }
        }

        return positions;
    }

    bool markedIn(const GamePosition & position, const PlayerMarker & playerMarker) const
    {
        checkRangeOf(position);

        return _slots[position.line(), position.column()]->markedBy(playerMarker);
    }

    bool emptyIn(const GamePosition & position) const
    {
        checkRangeOf(position);

        return _slots[position.line(), position.column()]->empty();
    }

    bool positionsMatch(const GamePosition & left, const GamePosition &right) const
    {
        checkRangeOf(left);
        checkRangeOf(right);

        const GameSlot leftSlot = _slots[left.line()][left.column()];
        const GameSlot rightSlot = _slots[right.line()][right.column()];

        return leftSlot == rightSlot;
    }

    bool isClearInAreaForPlay(const GameArea & area, const GamePosition & position) const
    {
        return position.in(area) and int(emptyPositions(area).size()) == (area.slotCount() - 1);
    }

    friend ostream & operator << (ostream &os, const GameBoard &gameBoard);

private:

    void checkRangeOf(const GamePosition &position) const
    {
        if (position.line() < 0 or position.line() >= LINE_COUNT)
        {
            throw runtime_error { "Line number out of range" };
        }

        if (position.column() < 0 or position.column() >= COLUMN_COUNT)
        {
            throw runtime_error { "Column number out of range" };
        }
    }

    GameSlot _slots[LINE_COUNT][COLUMN_COUNT];

};

ostream & operator << (ostream &os, const GameBoard &gameBoard)
{
    // Display column letters:
    os << "   ";
    for (int column = 0; column < COLUMN_COUNT; column++)
    {
        os << char('A' + column) << " ";
    }
    os << endl;

    // Display actual board:
    for (int line = 0; line < LINE_COUNT; line++)
    {
        os.width(2);
        os << line + 1 << ' ';
        os.width(0);

        for (int column = 0; column < COLUMN_COUNT; column++)
        {
            os << gameBoard._slots[line][column] << " ";
        }

        os << endl;
    }

    return os;
}
