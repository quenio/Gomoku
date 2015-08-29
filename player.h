// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "game_board.h"
#include "game_tree.h"

class Player
{
public:

    Player(const string & name, const PlayerMarker & marker) : _name { name },  _marker { marker }
    {
    }

    virtual ~Player() {}

    string name() { return _name; }

    virtual GameBoard play(GameBoard & gameBoard) = 0;

    friend bool operator == (Player & lhs, Player & rhs);

protected:

    const string _name;
    const PlayerMarker _marker;

};

bool operator == (Player & lhs, Player & rhs)
{
    return lhs._marker == rhs._marker;
}

class AIPlayer: public Player
{
public:
    AIPlayer(): Player { "Exterminator",  X }
    {
    }

    GameBoard play(GameBoard& gameBoard)
    {
        GameTree gameTree(gameBoard);
        GamePlay bestPlay = gameTree.bestPlayFor(_marker);
        return gameBoard.play(bestPlay, _marker);
    }

};

class HumanPlayer: public Player
{
public:
    HumanPlayer(): Player { "Charlie Brown", O }
    {
    }

    GameBoard play(GameBoard &gameBoard)
    {
        cout << "Your turn." << endl;

        int line;
        cout << "Line: ";
        cin >> line;

        int column;
        cout << "Column: ";
        cin >> column;

        cout << endl;

        return gameBoard.play(GamePlay { line, column }, _marker);
    }

};

