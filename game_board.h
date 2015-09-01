// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "game_slot.h"
#include "game_play.h"

class GameBoard
{
public:

    static const int WINNING_COUNT = 5;

    bool isGameOver() const
    {
        return hasWinner() or validPlays().size() == 0;
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
