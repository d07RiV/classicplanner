#pragma once

// Base event class
// Every event instance must be scheduled in the derived constructor by calling schedule() or the appropriate Event constructor
// Event instance will be destroyed after the event is executed or canceled and the pointer might be recycled for future use
// As such, events are referenced using EventHandle's from the outside, which checks for event validity before 
class Event : private noncopyable
{
public:
    Event( Sim& sim );
    Event( Sim& sim, timespan_t delta ) : Event( sim )
    {
        schedule_( delta );
    }

    virtual ~Event() {}

    void reschedule( timespan_t delta );
    // this will delete the event, invalidating the handle!
    void cancel();

    virtual void execute() = 0;
    virtual const char* name() const
    {
        return "Event";
    }

    uint64_t uid() const
    {
        return uid_;
    }

    Sim& sim() const
    {
        return sim_;
    }

    timespan_t time() const
    {
        return time_;
    }

    bool scheduled() const
    {
        return scheduled_ != std::numeric_limits<size_t>::max();
    }

private:
    Sim& sim_;
    static uint64_t uidCounter_;
    uint64_t uid_;

    friend class EventManager;
    size_t scheduled_ = std::numeric_limits<size_t>::max();
    timespan_t time_;

    template<typename T, typename... Args>
    friend EventHandle makeEvent( Sim& sim, Args&... args );

    static void* operator new( std::size_t size, Sim& sim );
    // placement delete is only called when an exception occurs in event constructor, so it's okay to have a bit of a leak here
    static void operator delete( void*, Sim& sim ) {}
    static void operator delete( void* ) {}
    static void* operator new( std::size_t ) = delete;

protected:
    void schedule_( timespan_t delta );
};

template<typename T, typename... Args>
inline EventHandle makeEvent( Sim& sim, Args&... args )
{
    static_assert( std::is_base_of<Event, T>::value, "Event type not derived from Event" );
    auto ptr = new( sim ) T( sim, std::forward<Args>( args )... );
    assert( ptr->scheduled() );
    return ptr;
}

class EventHandle
{
public:
    EventHandle( Event* event ) :
        event_( event ),
        uid_( event->uid() )
    {
    }

    explicit operator bool() const
    {
        return event_->uid() == uid_;
    }

    Event* operator->() const
    {
        assert( event_->uid() == uid_ );
        return event_;
    }

private:
    Event* event_;
    uint64_t uid_;
};

class EventManager
{
public:
    timespan_t time() const
    {
        return time_;
    }
    bool empty() const
    {
        return heap_.empty();
    }

    void scheduleEvent( Event* event, timespan_t time );
    void rescheduleEvent( Event* event, timespan_t time );
    void cancelEvent( Event* event );

    void* allocateEvent( size_t size );

    void execute();

private:
    void recycleEvent_( Event* event );

    timespan_t time_;

    std::vector<Event*> heap_;
    std::vector<void*> recycledEvents_;

    size_t siftUp_( size_t pos );
    size_t siftDown_( size_t pos );
};
