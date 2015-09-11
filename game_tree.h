// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "game_node.h"

class GameTree {
public:

    GameTree(const GameBoard & currentBoard, const GameArea & focus, const int deepestLevel):
        _root { GameNode { currentBoard }  }, _focus { focus }, _deepestLevel { deepestLevel }
    {
    }

    GamePosition bestPositionFor(const PlayerMarker & playerMarker)
    {
        const auto children = _root.childrenFor(playerMarker, _focus);
        auto bestPosition = children.front().playedPosition();
        Score maxScore = MIN_SCORE;

        for (const auto & gameNode : children)
        {
            if (DEBUG<TopLevel>::enabled)
            {
                cout << "GameNode: in: " << gameNode << endl;
            }

            const Score score = minMax(gameNode, playerMarker, maxScore, MAX_SCORE);

            if (score > maxScore)
            {
                maxScore = score;
                bestPosition = gameNode.playedPosition();
            }

            if (DEBUG<TopLevel>::enabled)
            {
                cout << "GameNode: out: " << gameNode;
                cout << " (Score: " << score << "; max: " << maxScore << ")" << endl << endl;
            }
        }

        if (DEBUG<TopLevel>::enabled)
        {
            cout << "AI Played: " << bestPosition << " (max: " << maxScore << ")" << endl << endl;
        }

        return bestPosition;
    }

private:

    Score minMax(GameNode node, PlayerMarker playerMarker, Score alpha, Score beta)
    {
        if (DEBUG<MidLevel>::enabled)
        {
            cout << "DEBUG: GameNode:" << endl << node << endl << endl;
        }

        if (node.level() == _deepestLevel or node.isGameOver())
        {
            const Score score = node.scoreFor(playerMarker);

            if (DEBUG<MidLevel>::enabled)
            {
                cout << "DEBUG: Score: " << score << " (" << alpha << "," << beta << ")" << endl << endl;
            }

            return score;
        }

        const PlayerMarker opponent = opponentOf(playerMarker);
        const vector<GameNode> children = node.childrenFor(opponent, _focus);

        if (DEBUG<BottomLevel>::enabled)
        {
            cout << "minMax: in: " << node << endl;
        }

        Score score;
        if (maxTurn(opponent))
        {
            score = max(children, opponent, alpha, beta);
        }
        else
        {
            score = min(children, opponent, alpha, beta);
        }

        if (DEBUG<BottomLevel>::enabled)
        {
            cout << "minMax: out: " << node << " - score: " << score << endl;
        }

        return score;
    }

    Score max(vector<GameNode> children, PlayerMarker playerMarker, Score alpha, Score beta)
    {
        if (DEBUG<BottomLevel>::enabled)
        {
            cout << "max: in (" << playerMarker << ": " << alpha << "," << beta << ")" << endl;
        }

        for (const auto & gameNode : children)
        {
            const Score score = minMax(gameNode, playerMarker, alpha, beta);

            if (score > alpha)
            {
                alpha = score; // a better best move for computer
            }

            if (alpha >= beta)
            {
                if (DEBUG<BottomLevel>::enabled)
                {
                    cout << "max: break" << endl;
                }

                break;
            }
        }

        if (DEBUG<BottomLevel>::enabled)
        {
            cout << "max: out (" << playerMarker << ": " << alpha << "," << beta << ")" << endl;
        }

        return alpha;
    }

    Score min(vector<GameNode> children, PlayerMarker playerMarker, Score alpha, Score beta)
    {
        if (DEBUG<BottomLevel>::enabled)
        {
            cout << "min: in (" << playerMarker << ": " << alpha << "," << beta << ")" << endl;
        }

        for (const auto & gameNode : children)
        {
            const Score score = minMax(gameNode, playerMarker, alpha, beta);

            if (score < beta)
            {
                beta = score;  // a better best move for opponent
            }

            if (alpha >= beta)
            {
                if (DEBUG<BottomLevel>::enabled)
                {
                    cout << "min: break" << endl;
                }

                break;
            }
        }

        if (DEBUG<BottomLevel>::enabled)
        {
            cout << "min: out (" << playerMarker << ": " << alpha << "," << beta << ")" << endl;
        }

        return beta;
    }

    GameNode _root;
    GameArea _focus;
    int _deepestLevel;
};
