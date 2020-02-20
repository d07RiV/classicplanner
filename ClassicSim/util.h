#pragma once

inline constexpr unsigned long nextPowerOfTwo( unsigned long v )
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;

    return v;
}

inline constexpr unsigned long logPowerOfTwo( unsigned long v )
{
    unsigned long r = ( v & 0xAAAAAAAA ) != 0;
    r |= ( ( v & 0xFFFF0000 ) != 0 ) << 4;
    r |= ( ( v & 0xFF00FF00 ) != 0 ) << 3;
    r |= ( ( v & 0xF0F0F0F0 ) != 0 ) << 2;
    r |= ( ( v & 0xCCCCCCCC ) != 0 ) << 1;
    return r;
}
