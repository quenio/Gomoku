// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "game_slot.h"
#include "game_position.h"

class GameBoard
{
public:

    GamePosition lastPlayedPosition() const { return _lastPlayedPosition; }

    bool isGameOver() const
    {
        return hasWinner() or emptyPositions().size() == 0;
    }

    bool hasWinner() const
    {
        PlayerMarker playerMarker;

        return victoryFound(playerMarker);
    }

    bool isDraw() const
    {
        return isGameOver() and not hasWinner();
    }

    PlayerMarker winner() const
    {
        PlayerMarker playerMarker;

        if (victoryFound(playerMarker)) return playerMarker;

        throw runtime_error { "Game has no winner yet." };
    }

    bool victoryFound(PlayerMarker & marker) const
    {
        // Horizontal
        for (int line = 0; line < LINE_COUNT; line++)
        {
            auto start = GamePosition { line, 0 };

            if (victoryFound(start, East, marker))
            {
                return true;
            }
        }

        // Vertical
        for (int column = 0; column < COLUMN_COUNT; column++)
        {
            auto start = GamePosition { 0, column };

            if (victoryFound(start, South, marker))
            {
                return true;
            }
        }

        // Diagonal - Northeast - Superior
        for (int line = WINNING_COUNT - 1; line < LINE_COUNT; line++)
        {
            auto start = GamePosition { line, 0 };

            if (victoryFound(start, Northeast, marker))
            {
                return true;
            }
        }

        // Diagonal - Northeast - Inferior
        for (int column = 1; column < COLUMN_COUNT - WINNING_COUNT; column++)
        {
            auto start = GamePosition { LINE_COUNT - 1, column };

            if (victoryFound(start, Northeast, marker))
            {
                return true;
            }
        }

        // Diagonal - Southeast - Superior
        for (int column = 0; column < COLUMN_COUNT - WINNING_COUNT; column++)
        {
            auto start = GamePosition { 0, column };

            if (victoryFound(start, Southeast, marker))
            {
                return true;
            }
        }

        // Diagonal - Southeast - Inferior
        for (int line = 1; line < LINE_COUNT - WINNING_COUNT; line++)
        {
            auto start = GamePosition { line, 0 };

            if (victoryFound(start, Southeast, marker))
            {
                return true;
            }
        }

        return false;
    }

    bool victoryFound(const GamePosition & start, const Direction & direction, PlayerMarker & playerMarker) const
    {
        auto current = start;

        while (current.valid())
        {
            while (emptyIn(current))
            {
                current = current.neighbor(direction);
            }

            int count = 1;

            while (current.valid())
            {
                auto previous = current;

                current = current.neighbor(direction);

                if (positionsMatch(previous, current))
                {
                    if (markerInPosition(previous) != markerInPosition(current))
                    {
                        throw runtime_error { "Markers should match at this point. " };
                    }

                    if (++count == WINNING_COUNT)
                    {
                        playerMarker = markerInPosition(current);
                        return true;
                    }
                }
                else
                {
                    break;
                }
            }
        }

        return false;
    }

    PlayerMarker markerInPosition(const GamePosition & position) const
    {
        if (emptyIn(position))
        {
            throw runtime_error { "No player marker available in this position." };
        }

        return _slots[position.line()][position.column()]._playerMarker;
    }

    GameBoard play(const GamePosition & position, const PlayerMarker & playerMarker) const
    {
        checkRangeOf(position);

        GameBoard newGameBoard { *this };

        newGameBoard._slots[position.line()][position.column()].mark(playerMarker);

        newGameBoard._lastPlayedPosition = position;

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
        if (position.valid())
        {
            return _slots[position.line()][ position.column()].markedBy(playerMarker);
        }
        else
        {
            return false;
        }
    }

    bool emptyIn(const GamePosition & position) const
    {
        if (position.valid())
        {
            return _slots[position.line()][position.column()].empty();
        }
        else
        {
            return false;
        }
    }

    bool positionsMatch(const GamePosition & left, const GamePosition & right) const
    {
        if (left.valid() and right.valid() and left != right)
        {
            const GameSlot leftSlot = _slots[left.line()][left.column()];
            const GameSlot rightSlot = _slots[right.line()][right.column()];

            if (leftSlot.empty() and rightSlot.empty())
            {
                return true; // Should match if both are empty.
            }
            else
            {
                return leftSlot == rightSlot;
            }
        }
        else
        {
            return false;
        }
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
    GamePosition _lastPlayedPosition;

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
