// Copyright (c) 2015 Quenio Cesar Machado dos Santos. All rights reserved.

#pragma once

template <typename T>
constexpr T ipow(T base, T exp)
{
    T result = 1;

    while (exp != 0)
    {
        if ((exp & 1) == 1)
        {
            result *= base;
        }

        exp >>= 1;
        base *= base;
    }

    return result;
}

template <typename T>
constexpr T isqrt(T n)
{
    T b = 0;

    while (n >= 0)
    {
        n = n - b;
        b = b + 1;
        n = n - b;
    }

    return b - 1;
}
