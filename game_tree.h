// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "game_board.h"
#include "debug.h"

enum ScoreWeight
{
    WIN = 1000,
    DRAW = 0
};

constexpr int score(const PlayerMarker & playerMarker, const ScoreWeight & scoreWeight)
{
    return (playerMarker == X ? +1 : -1) * scoreWeight;
}

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

    int score() const
    {
        if (isGameOver())
        {
            return utilityScore();
        }
        else
        {
            return heuristicScore();
        }
    }

    bool isGameOver() const { return _gameBoard.isGameOver(); }

    int utilityScore() const
    {
        if (_gameBoard.hasWinner())
        {
            return ::score(_gameBoard.winner(), WIN);
        }
        else if (_gameBoard.isDraw())
        {
            return DRAW;
        }
        else
        {
            throw runtime_error { "The utility score can only be calculated when the game board is terminal." };
        }
    }

    int heuristicScore() const
    {
        // TODO Determine heuristic.
        return 0;
    }

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

    GameTree(const GameBoard & currentBoard, const int maxLevel):
        _root { GameNode { currentBoard }  }, _maxLevel { maxLevel }
    {
    }

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

            if (DEBUG<TopLevel>::enabled and not DEBUG<MidLevel>::enabled)
            {
                cout << "DEBUG: Score: " << score << endl;
                cout << "DEBUG: GameNode:" << endl << gameNode << endl << endl;
            }
        }
        return bestPlay;
    }

private:

    static constexpr int MAX_SCORE = score(X, WIN);
    static constexpr int MIN_SCORE = score(O, WIN);

    int minMax(GameNode node, PlayerMarker playerMarker)
    {
        if (DEBUG<MidLevel>::enabled)
        {
            cout << "DEBUG: GameNode:" << endl << node << endl << endl;
        }

        if (node.level() == _maxLevel or node.isGameOver())
        {
            if (DEBUG<BottomLevel>::enabled)
            {
                cout << "DEBUG: Score: " << node.score() << endl << endl;
            }

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
    int _maxLevel;

};
