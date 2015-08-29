// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "game_board.h"

class GameNode
{
public:

    GameNode(const GameBoard & gameBoard): _gameBoard { gameBoard }
    {
    }

    GameNode(const GamePlay & gamePlay, const GameBoard & gameBoard, int level):
        _gamePlay { gamePlay }, _gameBoard { gameBoard }, _level { level }
    {
    }

    vector<GameNode> childrenFor(const PlayerMarker & playerMarker)
    {
        vector<GameNode> result;
        for (const auto &newPlay : _gameBoard.allLegalPlays())
        {
            GameBoard newGameBoard { _gameBoard.play(newPlay, playerMarker) };
            result.push_back(GameNode { newPlay, newGameBoard, _level + 1 });
        }
        return result;
    }

    GamePlay play() const { return _gamePlay; }

    int score() const { return _gameBoard.score(); }

    bool isGameOver() const { return _gameBoard.isGameOver(); }

    int level() const { return _level; }

    friend ostream & operator << (ostream &os, const GameNode &gameNode);

private:

    const GamePlay _gamePlay;
    const GameBoard _gameBoard;
    const int _level = 0;

};

ostream & operator << (ostream &os, const GameNode &gameNode)
{
    os << gameNode._gameBoard << "Play: " << gameNode._gamePlay << " - Level: " << gameNode._level;
    return os;
}

class GameTree {
public:

    GameTree(GameBoard currentBoard): _root { GameNode { currentBoard }  } {}

    GamePlay bestPlayFor(const PlayerMarker & playerMarker)
    {
        int maxScore = MIN_SCORE;
        GamePlay bestPlay(-1, -1);
        for (const auto &gameNode : _root.childrenFor(playerMarker))
        {
            int score = minMax(gameNode, adversaryOf(playerMarker));
            if (score > maxScore)
            {
                maxScore = score;
                bestPlay = gameNode.play();
            }

            if (DEBUG)
            {
                cout << "DEBUG: Score: " << score << endl;
                cout << "DEBUG: GameNode:" << endl << gameNode << endl << endl;
            }
        }
        return bestPlay;
    }

private:

    static constexpr int MAX_SCORE = +1000;
    static constexpr int MIN_SCORE = -1000;

    static constexpr int MAX_DEPTH = 10;

    int minMax(GameNode node, PlayerMarker playerMarker)
    {
        if (DEBUG)
        {
            if (node.level() > MAX_DEPTH)
            {
                throw runtime_error { "Game tree reached max depth." };
            }

            cout << "DEBUG: GameNode:" << endl << node << endl << endl;
        }

        if (node.isGameOver())
        {
            return node.score();
        }

        const vector<GameNode> children = node.childrenFor(playerMarker);

        if (maxTurn(playerMarker))
        {
            return max(children, adversaryOf(playerMarker));
        }
        else
        {
            return min(children, adversaryOf(playerMarker));
        }
    }

    int max(vector<GameNode> children, PlayerMarker playerMarker)
    {
        int maxScore = MIN_SCORE;
        for (const auto &gameNode : children)
        {
            int score = minMax(gameNode, playerMarker);
            if (score > maxScore)
            {
                maxScore = score;
            }
        }
        return maxScore;
    }

    int min(vector<GameNode> children, PlayerMarker playerMarker)
    {
        int minScore = MAX_SCORE;
        for (const auto &gameNode : children)
        {
            int score = minMax(gameNode, playerMarker);
            if (score < minScore)
            {
                minScore = score;
            }
        }
        return minScore;
    }

    GameNode _root;

};
