// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "game_slot.h"
#include "game_play.h"

class GameBoard
{
public:

    bool isGameOver() const
    {
        return hasWinner() or validPlays().size() == 0;
    }

    bool hasWinner() const
    {
        for (int line = 0; line < LINE_COUNT; line++) {
            if (isLineMatch(line)) {
                return true;
            }
        }

        for (int column = 0; column < COLUMN_COUNT; column++) {
            if (isColumnMatch(column)) {
                return true;
            }
        }

        if (isDiagonalMatch()) {
            return true;
        }

        return false;
    }

    bool isDraw() const
    {
        return isGameOver() and not hasWinner();
    }


    PlayerMarker winner() const
    {
        for (int line = 0; line < LINE_COUNT; line++) {
            if (isLineMatch(line)) {
                return _slots[line][0].playerMarker();
            }
        }

        for (int column = 0; column < COLUMN_COUNT; column++) {
            if (isColumnMatch(column)) {
                return _slots[0][column].playerMarker();
            }
        }

        if (isDiagonalMatch()) {
            return _slots[1][1].playerMarker();
        }

        throw runtime_error { "Game has no winner yet." };
    }

    // TODO Change implementation from TicTacToe to check Gomoku's 5-slot lines
    bool isLineMatch(int line) const {
        return (_slots[line][0] == _slots[line][1]) and (_slots[line][1] == _slots[line][2]);
    }

    // TODO Change implementation from TicTacToe to check Gomoku's 5-slot lines
    bool isColumnMatch(int column) const {
        return (_slots[0][column] == _slots[1][column]) and (_slots[1][column] == _slots[2][column]);
    }

    // TODO Change implementation from TicTacToe to check Gomoku's 5-slot lines
    bool isDiagonalMatch() const {
        return ((_slots[0][0] == _slots[1][1]) and (_slots[1][1] == _slots[2][2])) or
               ((_slots[2][0] == _slots[1][1]) and (_slots[1][1] == _slots[0][2]));
    }

    GameBoard play(GamePlay newPlay, const PlayerMarker &playerMarker) const
    {
        if (newPlay.line() < 0 or newPlay.line() >= LINE_COUNT)
        {
            throw out_of_range { "Line number out of range" };
        }

        if (newPlay.column() < 0 or newPlay.column() >= COLUMN_COUNT)
        {
            throw out_of_range { "Column number out of range" };
        }

        GameBoard newGameBoard { *this };

        newGameBoard._slots[newPlay.line()][newPlay.column()].mark(playerMarker);

        return newGameBoard;
    }

    vector<GamePlay> validPlays(const GameArea & area = FULL_BOARD) const
    {
        vector<GamePlay> legalPlays;

        for (int line = area.startLine(); line <= area.endLine(); line++) {
            for (int column = area.startColumn(); column <= area.endColumn(); column++) {
                if (_slots[line][column].isEmpty())
                {
                    legalPlays.push_back(GamePlay(line, column));
                }
            }
        }

        return legalPlays;
    }

    bool isClearInAreaForPlay(const GameArea & area, const GamePlay & gamePlay) const
    {
        return gamePlay.in(area) and int(validPlays(area).size()) == (area.slotCount() - 1);
    }

    friend ostream & operator << (ostream &os, const GameBoard &gameBoard);

private:

    GameSlot _slots[LINE_COUNT][COLUMN_COUNT];

};

ostream & operator << (ostream &os, const GameBoard &gameBoard)
{
    for (int line = 0; line < LINE_COUNT; line++)
    {
        for (int column = 0; column < COLUMN_COUNT; column++)
        {
            os << gameBoard._slots[line][column] << " ";
        }
        os << endl;
    }
    return os;
}
