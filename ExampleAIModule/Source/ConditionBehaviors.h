#pragma once

#include "BehaviorTree.h"

class IsLowHp : public Behavior
{
public:
	virtual BH_STATUS		Update(float deltaTime);
};

class IsBeingAttacked : public Behavior
{
public:
	virtual BH_STATUS		Update(float deltaTime);
};
