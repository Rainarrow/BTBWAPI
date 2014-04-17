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

class GroupMoveTo : public Behavior
{
public:

	virtual void			OnInitialize();
	virtual BH_STATUS		Update(float deltaTime);

private:
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

class GroupAttack : public Behavior
{
public:

	virtual void			OnInitialize();
	virtual BH_STATUS		Update(float deltaTime);
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

class CalculateRegroupPos : public Behavior
{
public:
	virtual BH_STATUS		Update(float deltaTime);
};

// Melee - Zealots
class CheckCriticalTarget : public Behavior
{
public:
	virtual BH_STATUS		Update(float deltaTime);
};

class FindGroupMoveToEnemy : public Behavior
{
public:
	virtual BH_STATUS		Update(float deltaTime);
};

class FindCenterAttack : public Behavior
{
public:
	virtual BH_STATUS		Update(float deltaTime);
};

// Global
class FindCriticalTarget : public Behavior
{
public:
	virtual BH_STATUS		Update(float deltaTime);
};

