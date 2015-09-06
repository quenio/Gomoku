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

enum PlayerSkill { Novice = 1, Medium = 2, Expert = 3, Master = 2000 };

class AIPlayer: public Player
{
public:
    AIPlayer(const PlayerSkill & skill): Player { "Exterminator",  X }, _skill { skill }
    {
    }

    GameBoard play(GameBoard & gameBoard)
    {
        GameTree gameTree { gameBoard, _skill };

        const GamePosition bestPosition = gameTree.bestPositionFor(_marker);

        cout << "Position Played: " << bestPosition << endl << endl;

        return gameBoard.play(bestPosition, _marker);
    }

private:

    const PlayerSkill _skill;
};

class HumanPlayer: public Player
{
public:

    HumanPlayer(): Player { "Charlie Brown", O }
    {
    }

    GameBoard play(GameBoard &gameBoard)
    {
        cout << "Your turn: ";

        string address;
        cin >> address;
        cout << endl;


        int line = address.size() == 3 ? lineFromChar(address[1]) * 10 + lineFromChar(address[2]) : lineFromChar(address[1]);
        int column = columnFromChar(address[0]);

        return gameBoard.play(GamePosition { line - 1, column }, _marker);
    }

private:

    int lineFromChar(char c) const { return c - '0'; }

    int columnFromChar(char c) const { return c - 'A'; }

};

