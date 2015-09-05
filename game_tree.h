// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "game_board.h"
#include "debug.h"

enum Score
{
    DRAW = 0, // The game board has come to a draw.
    EMPTY_POSITION = 50, // An empty position is just a possibility.
    SINGLE_MARK = 100, // A single mark on the game board.
    WIN = 500, // The game board has a victory.
};

constexpr int score(const PlayerMarker & playerMarker, const int & score)
{
    return (playerMarker == X ? +1 : -1) * score;
}

class GameNode
{
public:

    GameNode(const GameBoard & gameBoard): _gameBoard { gameBoard }
    {
    }

    GameNode(const GamePosition & playedPosition, const GameBoard & gameBoard, int level):
        _playedPosition { playedPosition }, _gameBoard { gameBoard }, _level { level }
    {
    }

    vector<GameNode> childrenFor(const PlayerMarker & playerMarker)
    {
        vector<GameNode> result;
        for (const auto & nextPosition : _gameBoard.emptyPositions())
        {
            GameBoard newGameBoard { _gameBoard.play(nextPosition, playerMarker) };
            result.push_back(GameNode { nextPosition, newGameBoard, _level + 1 });
        }
        return result;
    }

    GamePosition playedPosition() const { return _playedPosition; }

    bool isGameOver() const { return _gameBoard.isGameOver(); }

    int scoreFor(PlayerMarker playerMarker) const
    {
        if (isGameOver())
        {
            return utilityScore();
        }
        else
        {
            return ::score(playerMarker, heuristicScore());
        }
    }

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
        int max = 0;
        int extra = 0;

        for (int direction = North; direction <= Northwest; direction++)
        {
            for (int playerMarker = X; playerMarker <= O; playerMarker++)
            {
                int score = directionScore(Direction(direction), PlayerMarker(playerMarker));

                if (score == max)
                {
                    extra++;
                }
                else if (score > max)
                {
                    max = score;
                    extra = 0;
                }
            }
        }

        return max + extra;
    }

    int directionScore(const Direction & direction, const PlayerMarker & marker) const
    {
        int score = Score::SINGLE_MARK; // Considers the node's played position as marked by the param marker.
        int step = 1;
        int interval = 1;

        GamePosition previous, current = _playedPosition;
        while (current.valid() && interval < WINNING_COUNT)
        {
            previous = current;
            current = current.neighbor(direction, step);

            if (not current.valid() or not _gameBoard.markedIn(current, marker)) // blocked on this direction
            {
                if (step == 1)
                {
                    // Since already blocked on the immediate neighbor, giving up on this direction.
                    current = INVALID_POSITION;
                }
                else if (step > 1)
                {
                    // Trying out on the opposite direction.
                    step = -1;
                    current = _playedPosition;
                }
                score--; // A blocked line should be worth less than a free one.
            }
            else if (_gameBoard.markedIn(current, marker))
            {
                score += Score::SINGLE_MARK; // Full score; position already marked.
                step++; // Proceed on the same direction.
                interval++;
            }
            else if (_gameBoard.emptyIn(current))
            {
                score += Score::EMPTY_POSITION; // half-score; since this position is just a possibility at this point.
                step++; // Proceed on the same direction.
                interval++;
            }

            if (previous != _playedPosition and _gameBoard.positionsMatch(previous, current))
            {
                score++; // Intermittent lines score higher because they can be overlooked by the adversary.
            }
        }

        if (interval == WINNING_COUNT)
        {
            return score; // This direction has some potential.
        }
        else
        {
            return 0; // There are not enough positions available on this direction to win the game.
        }
    }

    int level() const { return _level; }

    friend ostream & operator << (ostream &os, const GameNode &gameNode);

private:

    const GamePosition _playedPosition;
    const GameBoard _gameBoard;
    const int _level = 0;

};

ostream & operator << (ostream &os, const GameNode &gameNode)
{
    os << gameNode._gameBoard << "Play: " << gameNode._playedPosition << " - Level: " << gameNode._level;
    return os;
}

class GameTree {
public:

    GameTree(const GameBoard & currentBoard, const int deepestLevel):
        _root { GameNode { currentBoard }  }, _deepestLevel { deepestLevel }
    {
    }

    GamePosition bestPositionFor(const PlayerMarker & playerMarker)
    {
        int maxScore = MIN_SCORE;
        GamePosition bestPosition;

        for (const auto &gameNode : _root.childrenFor(playerMarker))
        {
            int score = minMax(gameNode, playerMarker);
            if (score > maxScore)
            {
                maxScore = score;
                bestPosition = gameNode.playedPosition();
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

        return bestPosition;
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
