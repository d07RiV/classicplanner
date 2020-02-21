#include "sim.h"

uint64_t Event::uidCounter_ = 0;

Event::Event( Sim& sim ) :
    sim_( sim ),
    uid_( ++uidCounter_ )
{
}

void Event::schedule_( timespan_t delta )
{
    sim_.eventManager.scheduleEvent( this, sim_.time() + delta );
}

void Event::reschedule( timespan_t delta )
{
    sim_.eventManager.rescheduleEvent( this, sim_.time() + delta );
}

void Event::cancel()
{
    sim_.eventManager.cancelEvent( this );
}

void* Event::operator new( std::size_t size, Sim& sim )
{
    return sim.eventManager.allocateEvent( size );
}

void EventManager::scheduleEvent( Event* event, timespan_t time )
{
    assert( !event->scheduled() );
    event->time_ = time;
    heap_.push_back( event );
    siftUp_( heap_.size() - 1 );
}

void EventManager::rescheduleEvent( Event* event, timespan_t time )
{
    assert( event->scheduled() );
    if ( time < event->time_ )
    {
        event->time_ = time;
        siftUp_( event->scheduled_ );
    }
    else if ( time > event->time_ )
    {
        event->time_ = time;
        siftDown_( event->scheduled_ );
    }
}

void EventManager::cancelEvent( Event* event )
{
    assert( event->scheduled() );
    size_t pos = event->scheduled_;
    event->scheduled_ = std::numeric_limits<size_t>::max();
    event->uid_ = 0;

    Event* end = heap_.back();
    heap_.pop_back();
    if ( pos < heap_.size() )
    {
        heap_[pos] = end;
        pos = siftUp_( pos );
        siftDown_( pos );
    }

    recycleEvent_( event );
}

void EventManager::execute()
{
    assert( !heap_.empty() );
    Event* event = heap_[0];
    event->scheduled_ = std::numeric_limits<size_t>::max();
    event->uid_ = 0;

    Event* end = heap_.back();
    heap_.pop_back();
    if ( !heap_.empty() )
    {
        heap_[0] = end;
        siftDown_( 0 );
    }

    time_ = event->time_;
    event->execute();

    recycleEvent_( event );
}

size_t EventManager::siftUp_( size_t pos )
{
    Event* ptr = heap_[pos];
    auto score = ptr->time_;
    while ( pos > 0 )
    {
        size_t next = ( pos - 1 ) / 2;
        Event* parent = heap_[next];
        if ( parent->time_ < score )
        {
            break;
        }
        heap_[pos] = parent;
        parent->scheduled_ = pos;
        pos = next;
    }
    heap_[pos] = ptr;
    ptr->scheduled_ = pos;
    return pos;
}

size_t EventManager::siftDown_( size_t pos )
{
    Event* ptr = heap_[pos];
    auto score = ptr->time_;
    while ( 2 * pos + 1 < heap_.size() )
    {
        size_t next = 2 * pos + 1;
        Event* child = heap_[next];
        if ( next + 1 < heap_.size() )
        {
            Event* child2 = heap_[next + 1];
            if ( child2->time_ < child->time_ )
            {
                child = child2;
                ++next;
            }
        }
        if ( score <= child->time_ )
        {
            break;
        }
        heap_[pos] = child;
        child->scheduled_ = pos;
        pos = next;
    }
    heap_[pos] = ptr;
    ptr->scheduled_ = pos;
    return pos;
}

constexpr size_t cAlignSize = sizeof( max_align_t );
constexpr size_t cEventSizeLog = logPowerOfTwo( nextPowerOfTwo( cAlignSize + sizeof( Event ) ) );

void* EventManager::allocateEvent( size_t size )
{
    size = nextPowerOfTwo( size + cAlignSize );
    size_t sizeLog = logPowerOfTwo( size );
    assert( sizeLog >= cEventSizeLog );
    sizeLog -= cEventSizeLog;
    if ( recycledEvents_.size() <= sizeLog )
    {
        recycledEvents_.resize( sizeLog + 1, nullptr );
    }

    void* ptr = nullptr;
    if ( recycledEvents_[sizeLog] )
    {
        ptr = recycledEvents_[sizeLog];
        recycledEvents_[sizeLog] = *reinterpret_cast<void**>( ptr );
    }
    else
    {
        ptr = malloc( size );
    }
    *reinterpret_cast<size_t*>( ptr ) = sizeLog;

    return reinterpret_cast<uint8_t*>( ptr ) + cAlignSize;
}

void EventManager::recycleEvent_( Event* event )
{
    assert( event->uid_ == 0 );
    event->~Event();

    void* ptr = reinterpret_cast<uint8_t*>( event ) - cAlignSize;
    size_t sizeLog = *reinterpret_cast<size_t*>( ptr );
    assert( sizeLog < recycledEvents_.size() );
    *reinterpret_cast<void**>( ptr ) = recycledEvents_[sizeLog];
    recycledEvents_[sizeLog] = ptr;
}
