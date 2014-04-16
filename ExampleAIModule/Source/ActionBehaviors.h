#pragma once

#include "BehaviorTree.h"

class MoveTo : public Behavior
{
public:

	virtual void			OnInitialize();
	virtual BH_STATUS		Update(float deltaTime);

private:
	Position				m_pos;
	bool					m_moveAttack;
};

class Attack : public Behavior
{
public:

	virtual void			OnInitialize();
	virtual BH_STATUS		Update(float deltaTime);

private:
	Unit					m_target;
};

class FindTarget : public Behavior
{
public:

	virtual void			OnInitialize();
	virtual BH_STATUS		Update(float deltaTime);
};

class CalculateFallbackPos : public Behavior
{
public:
	virtual BH_STATUS		Update(float deltaTime);
};
