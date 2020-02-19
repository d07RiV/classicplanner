#pragma once

#include <cstdint>
#include <chrono>

using timespan_t = std::chrono::duration<int64_t>;

class noncopyable
{
protected:
    noncopyable() = default;
    noncopyable( noncopyable&& ) = default;
    noncopyable& operator=( noncopyable&& ) = default;

    noncopyable( const noncopyable& ) = delete;
    noncopyable& operator=( const noncopyable& ) = delete;
};

class nonmoveable : private noncopyable
{
protected:
    nonmoveable() = default;
    nonmoveable( nonmoveable&& ) = delete;
    nonmoveable& operator=( nonmoveable&& ) = delete;
};

class Sim;
class EventManager;
class Event;

#include "event.h"

class Sim
{
public:
    EventManager events;
};
