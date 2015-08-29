// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

// Debugging categories:
class TopLevel;
class MidLevel;
class BottomLevel;

template <class C> struct DEBUG
{
    static const bool enabled = false;
};

template <> struct DEBUG<TopLevel>
{
    static const bool enabled = true;
};

template <> struct DEBUG<MidLevel>
{
    static const bool enabled = false;
};

template <> struct DEBUG<BottomLevel>
{
    static const bool enabled = false;
};



