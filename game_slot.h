// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

#include "player_marker.h"

class GameSlot
{
public:

    bool hasPlayerMarker(const PlayerMarker & playerMarker) const
    {
        if (_empty)
        {
            return false;
        }
        else
        {
            return _playerMarker == playerMarker;
        }
    }

    bool isEmpty() const
    {
        return _empty;
    }

    PlayerMarker playerMarker() const
    {
        if (isEmpty())
        {
            throw runtime_error { "Game slot is empty." };
        }
        else
        {
            return _playerMarker;
        }
    }

    friend class GameBoard;
    friend bool operator == (const GameSlot & lhs, const GameSlot & rhs);

private:

    void mark(const PlayerMarker &playerMarker)
    {
        if (_empty)
        {
            _playerMarker = playerMarker;
            _empty = false;
        }
        else
        {
            throw "Game slot already marked";
        }
    }

    bool _empty = true;
    PlayerMarker _playerMarker;

};

inline bool operator == (const GameSlot & lhs, const GameSlot & rhs)
{
    if (lhs._empty || rhs._empty)
    {
        return false; // only marked _slots may match.
    }
    else
    {
        return lhs._playerMarker == rhs._playerMarker;
    }
}

inline ostream & operator << (ostream &os, const GameSlot &gameSlot)
{
    if (gameSlot.isEmpty()) {
        os << "[ ]";
    }
    else if (gameSlot.hasPlayerMarker(PlayerMarker::X))
    {
        os << "[X]";
    }
    else if (gameSlot.hasPlayerMarker(PlayerMarker::O))
    {
        os << "[O]";
    }
    return os;
}

