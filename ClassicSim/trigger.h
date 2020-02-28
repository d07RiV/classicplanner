#pragma once

template<class Ctx>
class Trigger
{
public:
    using Context = Ctx;
    using Handler = std::function<void( const Ctx& )>;

    template<class Func>
    void subscribe( Func&& func )
    {
        handlers_.emplace_back( std::forward<Func>( func ) );
    }

    void fire( const Ctx& context )
    {
        for ( auto& handler : handlers_ )
        {
            handler( context );
        }
    }

private:
    std::vector<Handler> handlers_;
};

namespace TriggerContexts
{

struct SpellCast
{
    Actor* source;
    Actor* target;
    Spell* spell;
};

}

using TriggerSpellCast = Trigger<TriggerContexts::SpellCast>;
