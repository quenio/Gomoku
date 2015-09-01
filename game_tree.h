// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "game_board.h"
#include "debug.h"

enum ScoreWeight
{
    WIN = 1000, // The game board has a victory.
    CLEAR_FIELD = 10, // The 5x5 area of the game board inspected has all slots available.
    DRAW = 0 // The game board has come to a draw.
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
        for (const auto &newPlay : _gameBoard.validPlays())
        {
            GameBoard newGameBoard { _gameBoard.play(newPlay, playerMarker) };
            result.push_back(GameNode { newPlay, newGameBoard, _level + 1 });
        }
        return result;
    }

    GamePlay gamePlay() const { return _gamePlay; }

    // TODO Modify in part 2 to implement envisioned algorithm that takes into account the heuristics.
    int scoreFor(PlayerMarker playerMarker) const
    {
        if (isGameOver())
        {
            return utilityScore();
        }
        else
        {
            return heuristicScore(playerMarker);
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

    // Naive, initial implementation of heuristics.
    int heuristicScore(const PlayerMarker & playerMarker) const
    {
        if (DEBUG<HeuristicLevel>::enabled)
        {
            cout << "Heuristic Player Marker: " << playerMarker << endl;
        }

        return ::score(playerMarker, heuristicWeight());
    }

    ScoreWeight heuristicWeight() const
    {
        if (DEBUG<HeuristicLevel>::enabled)
        {
            cout << "Valid Play Count: " << int(_gameBoard.validPlays(CENTRAL_AREA).size()) << endl;
            cout << "Slot Count: " << CENTRAL_AREA.slotCount() << endl << endl;
        }

        if (_gameBoard.isClearInAreaForPlay(CENTRAL_AREA, _gamePlay))
        {
            return CLEAR_FIELD;
        }
        else
        {
            return DRAW;
        }
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

    GameTree(const GameBoard & currentBoard, const int deepestLevel):
        _root { GameNode { currentBoard }  }, _deepestLevel { deepestLevel }
    {
    }

    GamePlay bestPlayFor(const PlayerMarker & playerMarker)
    {
        int maxScore = MIN_SCORE;
        GamePlay bestPlay(-1, -1);
        for (const auto &gameNode : _root.childrenFor(playerMarker))
        {
            int score = minMax(gameNode, playerMarker);
            if (score > maxScore)
            {
                maxScore = score;
                bestPlay = gameNode.gamePlay();
            }

            if (DEBUG<TopLevel>::enabled and not DEBUG<MidLevel>::enabled)
            {
                if (score != 0)
                {
                    cout << "DEBUG: Score: " << score << endl;
                    cout << "DEBUG: GameNode:" << endl << gameNode << endl << endl;
                }
            }
        }
        return bestPlay;
    }

private:

    static constexpr int MAX_SCORE = score(X, WIN);
    static constexpr int MIN_SCORE = score(O, WIN);

    // TODO Basic implementation of min-max - part 2 will implement alpha/beta
    int minMax(GameNode node, PlayerMarker playerMarker)
    {
        if (DEBUG<MidLevel>::enabled)
        {
            cout << "DEBUG: GameNode:" << endl << node << endl << endl;
        }

        if (node.level() == _deepestLevel or node.isGameOver())
        {
            const int score = node.scoreFor(playerMarker);

            if (DEBUG<BottomLevel>::enabled)
            {
                cout << "DEBUG: Score: " << score << endl << endl;
            }

            return score;
        }

        const PlayerMarker adversary = adversaryOf(playerMarker);
        const vector<GameNode> children = node.childrenFor(adversary);

        if (maxTurn(adversary))
        {
            return max(children, adversary);
        }
        else
        {
            return min(children, adversary);
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
    int _deepestLevel;

};
