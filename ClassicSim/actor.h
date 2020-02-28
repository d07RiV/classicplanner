#pragma once

class ActorResources
{
public:
    std::array<double, RESOURCE_MAX> base, initial, current, max;
};

class Actor
{
public:
    ActorResources resources;

    TriggerSpellCast onSpellCast;
};
