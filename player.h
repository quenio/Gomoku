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

enum PlayerSkill { Novice = 1, Medium = 2, Expert = 3, Master = 4 };

class AIPlayer: public Player
{
public:
    AIPlayer(const PlayerSkill & skill): Player { "Exterminator",  X }, _skill { skill }
    {
    }

    GameBoard play(GameBoard & gameBoard)
    {
        checkAndSetFocus(gameBoard.lastPlayedPosition());

        GameTree gameTree { gameBoard, focus, _skill };

        const GamePosition bestPosition = gameTree.bestPositionFor(_marker);

        cout << "Position Played: " << bestPosition << endl << endl;

        return gameBoard.play(bestPosition, _marker);
    }

private:

    void checkAndSetFocus(const GamePosition & lastPlayedPosition)
    {
        if (not lastPlayedPosition.in(focus) or lastPlayedPosition.onFrontierOf(focus))
        {
            int startLine = lastPlayedPosition.line() - FOCUS_LENGTH / 2;
            int startColumn = lastPlayedPosition.column() - FOCUS_LENGTH / 2;
            focus = GameArea { startLine, startColumn, startLine + FOCUS_LENGTH, startColumn + FOCUS_LENGTH };
            cout << "NEW FOCUS: " << focus << endl;
        }
    }

    const PlayerSkill _skill;
    GameArea focus { CENTRAL_AREA };
};

class HumanPlayer: public Player
{
public:

    HumanPlayer(): Player { "Charlie Brown", O }
    {
    }

    GameBoard play(GameBoard & gameBoard)
    {
        GamePosition position;
        bool positionInvalid = true, showScore = false;

        while (positionInvalid or showScore)
        {
            cout << "Your turn: ";

            string address;
            cin >> address;
            cout << endl;

            showScore = address[0] == 's';
            if (showScore)
            {
                GameNode node { gameBoard };
                node.scoreFor(X);
                cout << endl << endl;
            }
            else
            {
                int line = address.size() == 3 ? lineFromChar(address[1]) * 10 + lineFromChar(address[2]) : lineFromChar(address[1]);
                int column = columnFromChar(address[0]);

                position = GamePosition { line - 1, column };
                positionInvalid = not position.valid() or not gameBoard.emptyIn(position);

                if (positionInvalid)
                {
                    cout << "Position invalid: " << position << endl << endl;
                }
            }
        }

        return gameBoard.play(position, _marker);
    }

private:

    int lineFromChar(char c) const { return c - '0'; }

    int columnFromChar(char c) const { return c - 'A'; }

};

