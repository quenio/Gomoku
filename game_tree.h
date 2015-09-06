// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "game_board.h"
#include "debug.h"
#include "score.h"

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

    Score scoreFor(PlayerMarker playerMarker) const
    {
        if (isGameOver())
        {
            return utilityScore();
        }
        else
        {
            return scoreOf(playerMarker, heuristicScore());
        }
    }

    Score utilityScore() const
    {
        if (_gameBoard.hasWinner())
        {
            return scoreOf(_gameBoard.winner(), MAX_SCORE);
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

    Score heuristicScore() const
    {
        if (DEBUG<HeuristicLevel>::enabled)
        {
            cout << "Heuristic of " << _playedPosition << endl << endl;
        }

        Score score = DRAW;
        for (int direction = North; direction <= Northwest; direction++)
        {
            const Score scoreX = directionScore(Direction(direction), PlayerMarker::X);
            const Score scoreO = directionScore(Direction(direction), PlayerMarker::O);

            score += imax(scoreX, scoreO);
        }

        if (DEBUG<HeuristicLevel>::enabled)
        {
            cout << "Heuristic Score: " << score << " (" << (WIN - score) << " - " << WIN << ")" << endl << endl;
        }

        if (score > MAX_SCORE)
        {
            throw runtime_error { "No sequences may have score higher than winning score." };
        }

        return score;
    }

    Score directionScore(const Direction & direction, const PlayerMarker & marker) const
    {
        int step = 1;
        int seqCount = 1;
        int markerCount = 1; // Considers the marker on the first position.
        int emptyCount = 0;
        int blockedCount = 0;

        // Positions will score higher closer to the center.
        Score score = scoreOf(SINGLE_MARK - _playedPosition.distanceTo(CENTER), markerCount);

        if (DEBUG<HeuristicDetailedLevel>::enabled)
        {
            cout << "Direction: " << direction << " - " << marker << endl;
        }

        GamePosition previous, current = _playedPosition;
        while (current.valid() and seqCount < WINNING_COUNT)
        {
            if (step >= WINNING_COUNT)
            {
                if (DEBUG<HeuristicDetailedLevel>::enabled)
                {
                    cout << "Step: " << step << endl;
                }

                throw runtime_error { "Heuristhic function should not go farther than 5 steps." };
            }

            previous = current;
            current = _playedPosition.neighbor(direction, step);

            if (previous == current)
            {
                if (DEBUG<HeuristicLevel>::enabled)
                {
                    cout << "Step: " << step << endl;
                }

                throw runtime_error { "Unable to find neighbor position." };
            }

            if (_gameBoard.emptyIn(current))
            {
                score += scoreOf(EMPTY_POSITION, ++emptyCount); // half-score; just a possibility at this point.
                step = step > 0 ? step + 1 : step - 1; // Proceed on the same direction.
                seqCount++;
            }
            else if (_gameBoard.markedIn(current, marker))
            {
                score += scoreOf(SINGLE_MARK, ++markerCount); // Full score; position already marked.
                step = step > 0 ? step + 1 : step - 1; // Proceed on the same direction.
                seqCount++;
            }
            else // blocked on this direction
            {
                // A blocked line should be worth less than a free one.
                score -= scoreOf(BLOCKED, ++blockedCount);

                if (step <= 1)
                {
                    // Already blocked on the immediate neighbor, or on the opposite direction; giving up on this direction.
                    previous = current = INVALID_POSITION;
                }
                else if (step > 1)
                {
                    // Trying out on the opposite direction.
                    step = -1;
                }
            }

            if (DEBUG<HeuristicDetailedLevel>::enabled)
            {
                cout << "Score: " << score << " of " << current << endl;
            }

        }

        if (DEBUG<HeuristicDetailedLevel>::enabled)
        {
            cout << endl;
        }

        if (seqCount == WINNING_COUNT and step >= -2)
        {
            return score; // This direction has some potential.
        }
        else
        {
            // There are not enough positions available on this direction to win the game,
            // or a great chunk of it was using the opposite direction (avoid double-count)
            return 0;
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
        Score maxScore = MIN_SCORE;
        GamePosition bestPosition;

        for (const auto &gameNode : _root.childrenFor(playerMarker))
        {
            Score score = minMax(gameNode, playerMarker);
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

    // TODO Basic implementation of min-max - part 2 will implement alpha/beta
    Score minMax(GameNode node, PlayerMarker playerMarker)
    {
        if (DEBUG<MidLevel>::enabled)
        {
            cout << "DEBUG: GameNode:" << endl << node << endl << endl;
        }

        if (node.level() == _deepestLevel or node.isGameOver())
        {
            const Score score = node.scoreFor(playerMarker);

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

    Score max(vector<GameNode> children, PlayerMarker playerMarker)
    {
        Score maxScore = MIN_SCORE;
        for (const auto &gameNode : children)
        {
            Score score = minMax(gameNode, playerMarker);
            if (score > maxScore)
            {
                maxScore = score;
            }
        }
        return maxScore;
    }

    Score min(vector<GameNode> children, PlayerMarker playerMarker)
    {
        Score minScore = MAX_SCORE;
        for (const auto &gameNode : children)
        {
            Score score = minMax(gameNode, playerMarker);
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
