// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "player.h"

class Game
{
public:

    shared_ptr<Player> ai() { return _ai; }

    shared_ptr<Player> human() { return _human; }

    void startWithPlayer(shared_ptr<Player> initialPlayer)
    {
        resetGame(initialPlayer);
        displayGameStarted();
        chooseSkillLevel();
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
        cout << "Choose the skill level: " << endl;

        int skillLevel;
        cin >> skillLevel;
        cout << endl;

        _ai = shared_ptr<Player> { new AIPlayer { PlayerSkill(skillLevel) } };
    }

    shared_ptr<Player> _ai { new AIPlayer { Novice } };
    shared_ptr<Player> _human { new HumanPlayer };
    shared_ptr<Player> _currentPlayer;

    GameBoard _initialBoard, _currentBoard;
    int _playCount;

};


