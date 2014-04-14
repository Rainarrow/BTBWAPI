#pragma once

#include "BehaviorTree.h"

using namespace BWAPI;

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
