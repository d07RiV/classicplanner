#pragma once

#include <cstdint>
#include <chrono>
#include <cassert>

#include <memory>
#include <vector>
#include <functional>

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
class Actor;

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
    EventHandle after( timespan_t delta, T&& fn );

    template<typename T>
    void batch( T&& fn );

private:
    timespan_t batchSize_;
    std::vector<std::function<void()>> nextBatch_;
};

template<typename T>
EventHandle Sim::after( timespan_t delta, T&& fn )
{
    class FunctionEvent : public Event
    {
    public:
        FunctionEvent( Sim& sim, timespan_t delta, T&& fn ) :
            Event( sim, delta ),
            fn_( std::forward<T>( fn ) )
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
        std::decay_t<T> fn_;
    };

    return makeEvent( *this, delta, std::forward<T>( fn ) );
}

template<typename T>
void Sim::batch( T&& fn )
{
    if ( batchSize_ )
    {
        nextBatch_.emplace_back( std::forward<T>( fn ) );
    }
    else
    {
        after( timespan_t::zero(), std::forward<T>( fn ) );
    }
}
