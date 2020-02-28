#pragma once

class Action
{
public:
    virtual void execute( Actor* target ) = 0;
};

class Spell
{
public:
};
