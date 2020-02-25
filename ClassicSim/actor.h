#pragma once

class Spell
{
public:
};

struct Action
{
    Spell* spell;
    Actor* target;
};

class ActorResources
{
public:
    std::array<double, RESOURCE_MAX> base, initial, current, max;
};

class Actor
{
public:
    ActorResources resources;
};
