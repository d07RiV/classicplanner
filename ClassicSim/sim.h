#pragma once

#include <cstdint>
#include <chrono>
#include <cassert>

#include <memory>
#include <vector>

#include "util.h"

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
class EventHandle;

#include "event.h"

class Sim
{
public:
    timespan_t time() const
    {
        return eventManager.time();
    }

    EventManager eventManager;

    template<typename T>
    EventHandle after( timespan_t delta, const T& fn );
};

template<typename T>
EventHandle Sim::after( timespan_t delta, const T& fn )
{
    class FunctionEvent : public Event
    {
    public:
        FunctionEvent( Sim& sim, timespan_t delta, const T& fn ) :
            Event( sim, delta ),
            fn_( fn )
        {
        }

        const char* name() const override
        {
            return "FunctionEvent";
        }

        void execute() override
        {
            fn_();
        }
    private:
        T fn_;
    };

    return makeEvent( *this, delta, fn );
}
