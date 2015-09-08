// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "debug.h"
#include "score.h"
#include "game_board.h"

class GameNode
{
public:

    GameNode(const GameBoard & gameBoard, int level =  0, int distanceToParent = 0):
        _playedPosition { gameBoard.lastPlayedPosition() },
        _gameBoard { gameBoard },
        _level { level },
        _distanceToParent { distanceToParent }
    {
        if (not _playedPosition.valid())
        {
            _playedPosition = CENTER; // If the game board has not been played yet, we start from the center.
        }
    }

    vector<GameNode> childrenFor(const PlayerMarker & playerMarker)
    {
        vector<GameNode> result;

        for (const auto & nextPosition : _gameBoard.emptyPositions())
        {
            result.push_back(GameNode
                                 {
                                     GameBoard { _gameBoard.play(nextPosition, playerMarker) },
                                     _level + 1,
                                     _playedPosition.distanceTo(nextPosition)
                                 });
        }

        sort(result.begin(), result.end(), [](const GameNode & left, const GameNode & right)
        {
            return left._distanceToParent < right._distanceToParent;
        });

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
            return heuristicScore(playerMarker);
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

    Score heuristicScore(const PlayerMarker & playerMarker) const
    {
        if (DEBUG<HeuristicLevel>::enabled)
        {
            cout << "Heuristic of " << playerMarker << " - " << _playedPosition << endl;
        }

        Score score = DRAW;
        for (int i = North; i <= Northwest; i++)
        {
            const auto direction = Direction(i);

            score += markerScore(direction, playerMarker);
            score -= markerScore(direction, opponentOf(playerMarker));
            score += mixedScore(direction, playerMarker);
            score -= mixedScore(direction, opponentOf(playerMarker));
        }

        if (DEBUG<HeuristicLevel>::enabled)
        {
            cout << "Heuristic Score: " << score << " (" << (WIN - score) << " - " << WIN << ")" << endl;
        }

        if (score > MAX_SCORE)
        {
            throw runtime_error { "No sequences may have score higher than winning score." };
        }

        return scoreOf(playerMarker, score);
    }

    // TODO Remove duplication between markerScore() and mixedScore()
    Score markerScore(const Direction &direction, const PlayerMarker &marker) const
    {
        int step = 1;
        int seqCount = 1;
        int markerCount = 1; // Considers the marker on the first position.
        int blockedCount = 0;
        int emptyCount = 0;

        Score score = scoreOf(SINGLE_MARK, markerCount);

        if (DEBUG<HeuristicDetailedLevel>::enabled)
        {
            cout << "markerScore - Direction: " << direction << " - " << marker << endl;
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

            if (_gameBoard.markedIn(current, marker))
            {
                score += scoreOf(SINGLE_MARK, ++markerCount); // Full score; position already marked.
                step = step > 0 ? step + 1 : step - 1; // Proceed on the same direction.
                seqCount++;
            }
            else // blocked on this direction - marked positions not found
            {
                // A blocked line should be worth less than a free one.
                if (_gameBoard.emptyIn(current))
                {
                    score -= scoreOf(EMPTY_BLOCKED, (++emptyCount + markerCount));
                }
                else
                {
                    score -= scoreOf(FILLED_BLOCKED, (++blockedCount + markerCount));
                }

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

        if (step >= -2)
        {
            return scoreOf(marker, score); // This direction has some potential.
        }
        else
        {
            return 0; // A good chunk of this sequence was in the opposite direction (avoid double-count)
        }
    }

    Score mixedScore(const Direction &direction, const PlayerMarker &marker) const
    {
        int step = 1;
        int seqCount = 1;
        int markerCount = 0;
        int emptyCount = 0;
        int blockedCount = 0;

        Score score = 0;
        GamePosition previous, current = _playedPosition;

        if (_gameBoard.markedIn(current, marker))
        {
            scoreOf(SINGLE_MARK - _playedPosition.distanceTo(CENTER), markerCount); // Positions will score higher closer to the center.
            markerCount++;
        }

        if (DEBUG<HeuristicDetailedLevel>::enabled)
        {
            cout << "mixedScore - Direction: " << direction << " - " << marker << endl;
        }

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
                score += scoreOf(EMPTY_POSITION, (++emptyCount + markerCount)); // half-score; just a possibility at this point.
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
                score -= scoreOf(FILLED_BLOCKED, (++blockedCount + markerCount));

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
            return scoreOf(score, marker); // This direction has some potential.
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

    GamePosition _playedPosition;
    GameBoard _gameBoard;
    int _level;
    int _distanceToParent;

};

ostream & operator << (ostream &os, const GameNode &gameNode)
{
    if (not DEBUG<BottomLevel>::enabled and not DEBUG<HeuristicLevel>::enabled)
    {
        os << endl << gameNode._gameBoard << endl;
    }

    os << "Play: " << gameNode._playedPosition << " - Level: " << gameNode._level;

    return os;
}

