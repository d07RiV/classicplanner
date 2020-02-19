#pragma once

class Event : private noncopyable
{
public:
    Event( Sim& sim );
    Event( Sim& sim, timespan_t delta ) : Event( sim )
    {
        schedule( delta );
    }

    virtual ~Event() {}

    void schedule( timespan_t delta );
    void reschedule( timespan_t delta );

    virtual void execute() = 0;
    virtual const char* name() const
    {
        return "Event";
    }

protected:
    Sim& sim_;

    static uint64_t uid_counter_;
    uint64_t uid_;
    timespan_t time_;
    int scheduled_ = -1;

    template<typename Event, typename... Args>
    friend Event* make_event( Sim& sim, Args&... args );

    static void* operator new( std::size_t size, Sim& sim )
    {
        return sim.events.allocate_event( size );
    }
};

template<class T = Event>
class EventHandle
{
public:

private:
    Event* event_;
};

class EventManager
{
public:
    void* allocate_event( size_t size );

private:
    Sim& sim_;

    std::vector<Event*> heap_;
};
