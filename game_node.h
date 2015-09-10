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
        if (_gameBoard.winner() == X)
        {
            return MAX_SCORE;
        }
        else if (_gameBoard.winner() == O)
        {
            return MIN_SCORE;
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

    Score heuristicScore(const PlayerMarker & marker) const
    {
        if (DEBUG<HeuristicLevel>::enabled)
        {
            cout << "Heuristic of " << marker << " - " << _playedPosition << endl;
        }

        Score score = DRAW;

        // Horizontal
        for (int line = 0; line < LINE_COUNT; line++)
        {
            auto start = GamePosition { line, 0 };

            score += directionScore(start, East, marker);
        }

        // Vertical
        for (int column = 0; column < COLUMN_COUNT; column++)
        {
            auto start = GamePosition { 0, column };

            score += directionScore(start, South, marker);
        }

        // Diagonal - Northeast - Superior
        for (int line = WINNING_COUNT - 1; line < LINE_COUNT; line++)
        {
            auto start = GamePosition { line, 0 };

            score += directionScore(start, Northeast, marker);
        }

        // Diagonal - Northeast - Inferior
        for (int column = 1; column < COLUMN_COUNT - WINNING_COUNT; column++)
        {
            auto start = GamePosition { LINE_COUNT - 1, column };

            score += directionScore(start, Northeast, marker);
        }

        // Diagonal - Southeast - Superior
        for (int column = 0; column < COLUMN_COUNT - WINNING_COUNT; column++)
        {
            auto start = GamePosition { 0, column };

            score += directionScore(start, Southeast, marker);
        }

        // Diagonal - Southeast - Inferior
        for (int line = 1; line < LINE_COUNT - WINNING_COUNT; line++)
        {
            auto start = GamePosition { line, 0 };

            score += directionScore(start, Southeast, marker);
        }

        if (DEBUG<HeuristicLevel>::enabled)
        {
            cout << "Heuristic Score: " << score << " (" << MAX_SCORE - abs(score) << " - " << MAX_SCORE << ")" << endl;
        }

        if (score > MAX_SCORE)
        {
            throw runtime_error { "No sequences may have score higher than winning score." };
        }

        return score;
    }

    Score directionScore(const GamePosition & start, const Direction & direction, const PlayerMarker & marker) const
    {
        Score score = DRAW;

        score += markerScore(start, direction, marker);
        score += markerScore(start, direction, opponentOf(marker));
        score += mixedScore(start, direction, marker);
        score += mixedScore(start, direction, opponentOf(marker));

        if (DEBUG<HeuristicDetailedLevel>::enabled)
        {
            cout << "directionScore - " << start << " - " << direction << " - " << marker << endl;
        }

        cout << "directionScore: " << score << " - " << start << " - " << direction << " - " << marker << endl;

        return score;
    }

    Score markerScore(GamePosition start, const Direction & direction, const PlayerMarker & marker) const
    {
        Score score = DRAW;

        while (start.valid())
        {
            GamePosition end;
            score += markerScore(start, direction, marker, end);
            start = end;
        }

        return score;
    }

    // TODO Remove duplication between markerScore() and mixedScore()
    Score markerScore(const GamePosition & start, const Direction & direction, const PlayerMarker & marker, GamePosition & end) const
    {
        if (DEBUG<HeuristicDetailedLevel>::enabled)
        {
            cout << "markerScore - " << start << " - " << direction << " - " << marker << endl;
        }

        int markerCount = 0;
        int blockedCount = 0;
        int emptyCount = 0;

        GamePosition current = findPosition(start, direction, marker);

        Score score = DRAW;
        if (_gameBoard.markedIn(current, marker))
        {
            markerCount++;
            score += scoreOf(marker, SINGLE_MARK - current.distanceTo(CENTER), markerCount); // Positions will score higher closer to the center.
        }
        else
        {
            return DRAW;
        }

        if (DEBUG<HeuristicDetailedLevel>::enabled)
        {
            cout << "Score: " << score << " of " << current << endl;
        }

        int step = 1;
        int seqCount = 1;
        GamePosition previous, base = current;

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
            current = base.neighbor(direction, step);

            if (step > 0)
            {
                end = current;
            }

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
                score += scoreOf(marker, SINGLE_MARK, ++markerCount); // Full score; position already marked.
                step = step > 0 ? step + 1 : step - 1; // Proceed on the same direction.
                seqCount++;
            }
            else // blocked on this direction - marked positions not found
            {
                // A blocked line should be worth less than a free one.
                if (_gameBoard.emptyIn(current))
                {
                    score += scoreOf(marker, EMPTY_POSITION, (++emptyCount + markerCount));
                    seqCount++;
                }
                else
                {
                    score += scoreOf(opponentOf(marker), BLOCKED, (++blockedCount + markerCount));
                }

                if (step <= 1)
                {
                    // Already blocked on the immediate neighbor, or on the opposite direction; giving up on this direction.
                    current = INVALID_POSITION;
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

        if (step < -2)
        {
            score = DRAW; // A good chunk of this sequence was in the opposite direction (avoid double-count)
        }

        if (DEBUG<HeuristicDetailedLevel>::enabled)
        {
            cout << "Final score: " << score << " of " << current << endl << endl;
        }

        return score;
    }

    Score mixedScore(GamePosition start, const Direction & direction, const PlayerMarker & marker) const
    {
        Score score = DRAW;

        while (start.valid())
        {
            GamePosition end;
            score += mixedScore(start, direction, marker, end);
            start = end;
        }

        return score;
    }

    Score mixedScore(const GamePosition & start, const Direction & direction, const PlayerMarker & marker, GamePosition & end) const
    {
        if (DEBUG<HeuristicDetailedLevel>::enabled)
        {
            cout << "mixedScore - " << start << " - " << direction << " - " << marker << endl;
        }

        int markerCount = 0;
        int emptyCount = 0;
        int blockedCount = 0;

        GamePosition current = findPosition(start, direction, marker);

        Score score = DRAW;
        if (_gameBoard.markedIn(current, marker))
        {
            markerCount++;
            score += scoreOf(marker, SINGLE_MARK - current.distanceTo(CENTER), markerCount); // Positions will score higher closer to the center.
        }
        else
        {
            return DRAW;
        }

        if (DEBUG<HeuristicDetailedLevel>::enabled)
        {
            cout << "Score: " << score << " of " << current << endl;
        }

        int step = 1;
        int seqCount = 1;
        GamePosition previous, base = current;

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
            current = base.neighbor(direction, step);

            if (step > 0)
            {
                end = current;
            }

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
                if (_gameBoard.emptyIn(previous))
                {
                    // Do not count two subsequent empty spaces.
                    current = INVALID_POSITION;
                }

                score += scoreOf(marker, EMPTY_POSITION, (++emptyCount + markerCount)); // half-score; just a possibility at this point.
                step = step > 0 ? step + 1 : step - 1; // Proceed on the same direction.
                seqCount++;
            }
            else if (_gameBoard.markedIn(current, marker))
            {
                score += scoreOf(marker, SINGLE_MARK, ++markerCount); // Full score; position already marked.
                step = step > 0 ? step + 1 : step - 1; // Proceed on the same direction.
                seqCount++;
            }
            else // blocked on this direction
            {
                // A blocked line should be worth less than an open one.
                score += scoreOf(opponentOf(marker), BLOCKED, (++blockedCount + markerCount));

                if (step <= 1)
                {
                    // Already blocked on the immediate neighbor, or on the opposite direction; giving up on this direction.
                    current = INVALID_POSITION;
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

        if (seqCount != WINNING_COUNT or step < -2)
        {
            // There are not enough positions available on this direction to win the game,
            // or a great chunk of it was using the opposite direction (avoid double-count)
            score = DRAW;
        }

        if (DEBUG<HeuristicDetailedLevel>::enabled)
        {
            cout << "Final score: " << score << " of " << current << endl << endl;
        }

        return score;
    }

    GamePosition findPosition(const GamePosition & start, const Direction & direction, const PlayerMarker & marker) const
    {
        GamePosition current = start;

        while (current.valid() and not _gameBoard.markedIn(current, marker))
        {
            current = current.neighbor(direction, 1);
        }

        return current;
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

