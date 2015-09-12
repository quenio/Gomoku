// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "player.h"

class Game
{
public:

    void start()
    {
        displayGameStarted();
        chooseSkillLevel();
        choosePlayerToStart();
        displayInitialBoard();
        while (inProgress())
        {
            doPlay();
            displayCurrentPlay();
            alternatePlayer();
        }
        displayFinalResult();
    }

private:

    void resetGame(shared_ptr<Player> initialPlayer)
    {
        if (initialPlayer == nullptr)
        {
            throw runtime_error { "Initial player not defined." };
        }

        _currentPlayer = initialPlayer;
        _currentBoard = _initialBoard;
        _playCount = 0;
    }

    bool inProgress()
    {
        return not _currentBoard.isGameOver();
    }

    void doPlay()
    {
        _currentBoard = _currentPlayer->play(_currentBoard);
        _playCount++;
    }

    void alternatePlayer()
    {
        _currentPlayer = (_currentPlayer == _ai ? _human : _ai);
    }

    void displayGameStarted()
    {
        cout << "Gamed Started!" << endl << endl;
    }

    void displayInitialBoard()
    {
        cout << _initialBoard << endl;
    }

    void displayCurrentPlay()
    {
        cout << "Play No.: " << _playCount << " (" << _currentPlayer->name() << ")" << endl << endl << _currentBoard << endl;
    }

    void displayFinalResult()
    {
        if (_currentBoard.isDraw())
        {
            cout << "Wow!!! What a draw!!!";
        }
        else if (_currentBoard.winner() == X)
        {
            cout << "YESS!!! The Exterminator wins again!!!" << endl ;
        }
        else
        {
            cout << "Congrats!!! You WON!!!" << endl;
        }
    }

    void chooseSkillLevel()
    {
        cout << "1 - Novice (depth = 1 on MinMax search)" << endl;
        cout << "2 - Medium (depth = 2)" << endl;
        cout << "3 - Expert (depth = 3)" << endl;
        cout << "4 - Master (depth = 4)" << endl << endl;

        int skillLevel = 0;
        while (skillLevel < Novice or skillLevel > Master)
        {
            cout << "Choose the skill level: ";
            cin >> skillLevel;
            cout << endl;

            if (skillLevel >= Novice and skillLevel <= Master)
            {
                _ai = shared_ptr<Player> { new AIPlayer { PlayerSkill(skillLevel) } };
            }
            else
            {
                cout << "Invalid skill level: " << skillLevel << endl << endl;
            }
        }
    }

    void choosePlayerToStart()
    {
        cout << "1 - Computer" << endl;
        cout << "2 - User" << endl << endl;

        int player = 0;
        while (player < 1 or player > 2)
        {
            cout << "Choose the player to start: ";
            cin >> player;
            cout << endl;

            if (player == 1)
            {
                resetGame(_ai);
            }
            else if (player == 2)
            {
                resetGame(_human);
            }
            else
            {
                cout << "Invalid player: " << player << endl << endl;
            }
        }
    }

    shared_ptr<Player> _ai { new AIPlayer { Novice } };
    shared_ptr<Player> _human { new HumanPlayer };
    shared_ptr<Player> _currentPlayer;

    GameBoard _initialBoard, _currentBoard;
    int _playCount;

};


