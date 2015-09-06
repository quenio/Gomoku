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
        cout << "Gamed Started!" << endl << endl << _initialBoard << endl;
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

    shared_ptr<Player> _ai { new AIPlayer { Medium } };
    shared_ptr<Player> _human { new HumanPlayer };
    shared_ptr<Player> _currentPlayer;

    GameBoard _initialBoard, _currentBoard;
    int _playCount;

};


