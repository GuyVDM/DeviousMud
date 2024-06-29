#pragma once
#include "Shared/Game/State.hpp"

class WorldEntity;

class EntityIdleState : public DM::State::State<WorldEntity>
{
public:	IMPLEMENT_STATE_METHODS
};

class EntityWalkingState : public DM::State::State<WorldEntity>
{
public:	IMPLEMENT_STATE_METHODS
};

class EntityAttackingState : public DM::State::State<WorldEntity>
{
public:	IMPLEMENT_STATE_METHODS
};

class EntityDeathState : public DM::State::State<WorldEntity>
{
public:	IMPLEMENT_STATE_METHODS
};