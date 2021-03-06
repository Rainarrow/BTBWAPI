#include "ActionBehaviors.h"
#include "BehaviorTree.h"
#include "Blackboard.h"
#include "UnitGroup.h"

////////////////////////////////////////////////////
// MoveTo
////////////////////////////////////////////////////

void MoveTo::OnInitialize()
{
	int attack = 0;
	s_blackboard->GetInt("moveattack", attack);
	m_moveAttack = attack != 0;
}

BH_STATUS MoveTo::Update(float deltaTime)
{
	Unit owner = NULL;
	if(!s_blackboard->GetUnit("owner0", owner))
		return BH_FAILURE;

	Position movePos;
	if(!s_blackboard->GetPosition("moveto", movePos))
		return BH_FAILURE;

	Broodwar->drawLine(CoordinateType::Map, owner->getPosition().x, owner->getPosition().y, movePos.x, movePos.y, m_moveAttack ? Colors::Red : Colors::White);

	if(owner->isMoving())
		return BH_RUNNING;

	Position pos = owner->getPosition();
	if(pos.getApproxDistance(movePos) < 2.0f)
		return BH_SUCCESS;
	if(GetStatus() == BH_RUNNING)
		return BH_FAILURE;

	if(!m_moveAttack)
		owner->move(movePos);
	else
		owner->attack(movePos);

	return BH_RUNNING;
}

////////////////////////////////////////////////////
// GroupMoveTo
////////////////////////////////////////////////////

void GroupMoveTo::OnInitialize()
{
	int attack = 0;
	s_blackboard->GetInt("moveattack", attack);
	m_moveAttack = attack != 0;
}

BH_STATUS GroupMoveTo::Update(float deltaTime)
{
	UnitGroup * group = s_blackboard->GetUnitGroup();
	if(group == NULL || group->GetUnits().empty())
		return BH_FAILURE;

	bool moving = false;
	int i = 0, moveDone = 0;
	for(auto it = group->GetUnits().begin(); it != group->GetUnits().end(); ++it, ++i)
	{
		Unit unit = *it;
		if(!unit->exists())
			continue;

		char name[10];
		sprintf_s(name, 10, "moveto%d", i);
		Position movePos;
		if(!s_blackboard->GetPosition(name, movePos))
			continue;

		moving = true;

		Broodwar->drawLine(CoordinateType::Map, unit->getPosition().x, unit->getPosition().y, movePos.x, movePos.y, m_moveAttack ? Colors::Red : Colors::White);

		//if(unit->isMoving())
		//	continue;

		Position pos = unit->getPosition();
		if(!m_moveAttack && pos.getApproxDistance(movePos) < 5.0f)
		{
			++moveDone;
			continue;
		}

		if(GetStatus() == BH_RUNNING)
		{
			//return BH_FAILURE;
		}

		if(!m_moveAttack)
			unit->move(movePos);
		else
			unit->attack(movePos);
	}

	if(!moving)
		return BH_FAILURE;
	if(moveDone == group->GetUnits().size())
		return BH_SUCCESS;

	return BH_RUNNING;
}

//////////////////////////////
// ATTACK BEHAVIOR
//////////////////////////////

void Attack::OnInitialize()
{
	bool result = s_blackboard->GetUnit("target", m_target);
	assert(result);
}

BH_STATUS Attack::Update(float deltaTime)
{
	if(!m_target->exists())
		return BH_SUCCESS;

	UnitGroup * group = s_blackboard->GetUnitGroup();
	if(group == NULL || group->GetUnits().empty())
		return BH_FAILURE;

	bool attacking = false;
	for(auto it = group->GetUnits().begin(); it != group->GetUnits().end(); ++it)
	{
		Unit unit = *it;
		if(!unit->exists())
			continue;
		attacking = true;

		UnitCommand currentCmd(unit->getLastCommand());

		if (currentCmd.getType() == UnitCommandTypes::Attack_Unit && currentCmd.getTarget() == m_target)
		{
			Broodwar->drawLine(CoordinateType::Map, unit->getPosition().x, unit->getPosition().y, m_target->getPosition().x, m_target->getPosition().y, Colors::Red);
			continue;
		}
		if (unit->isAttackFrame()) // Unit is in Attack animation
		{
			Broodwar->drawLine(CoordinateType::Map, unit->getPosition().x, unit->getPosition().y, m_target->getPosition().x, m_target->getPosition().y, Colors::Red);
			continue;
		}

		//Unit is not attacking its designated target, make it do so
		unit->attack(m_target);

		//Debug draw
		Broodwar->drawLine(CoordinateType::Map, unit->getPosition().x, unit->getPosition().y, m_target->getPosition().x, m_target->getPosition().y, Colors::Red);
	}

	return attacking ? BH_RUNNING : BH_FAILURE;
}

//////////////////////////////
// GroupAttack
//////////////////////////////

BH_STATUS GroupAttack::Update(float deltaTime)
{
	UnitGroup * group = s_blackboard->GetUnitGroup();
	if(group == NULL || group->GetUnits().empty())
		return BH_FAILURE;

	bool attacking = false;
	int i = 0;
	for(auto it = group->GetUnits().begin(); it != group->GetUnits().end(); ++it, ++i)
	{
		Unit unit = *it;
		if(!unit->exists())
			continue;
		attacking = true;

		Unit target = NULL;
		char name[10];
		sprintf_s(name, 10, "target%d", i);
		s_blackboard->GetUnit(name, target);
		if(target == NULL)
			continue;

		UnitCommand currentCmd(unit->getLastCommand());

		if (currentCmd.getType() == UnitCommandTypes::Attack_Unit && currentCmd.getTarget() == target)
		{
			Broodwar->drawLine(CoordinateType::Map, unit->getPosition().x, unit->getPosition().y, target->getPosition().x, target->getPosition().y, Colors::Red);
			continue;
		}
		if (unit->isAttackFrame()) // Unit is in Attack animation
		{
			Broodwar->drawLine(CoordinateType::Map, unit->getPosition().x, unit->getPosition().y, target->getPosition().x, target->getPosition().y, Colors::Red);
			continue;
		}

		//Unit is not attacking its designated target, make it do so
		unit->attack(target);

		//Debug draw
		Broodwar->drawLine(CoordinateType::Map, unit->getPosition().x, unit->getPosition().y, target->getPosition().x, target->getPosition().y, Colors::Red);
	}

	return attacking ? BH_SUCCESS : BH_FAILURE;
}

////////////////////////////////////////////////////
// FindTarget
////////////////////////////////////////////////////

void FindTarget::OnInitialize()
{
}

BH_STATUS FindTarget::Update(float deltaTime)
{
	UnitGroup * group = s_blackboard->GetUnitGroup();
	if(group == NULL || group->GetUnits().empty())
		return BH_FAILURE;
	//Position center = group->CalcCenterPosition();

	Unit owner = NULL;
	s_blackboard->GetUnit("owner0", owner);
	if (owner == NULL || !owner->exists())
		return BH_FAILURE;

	WeaponType wtype = owner->getType().groundWeapon();
	//int radius = wtype.maxRange();
	int radius = 6 * TILE_SIZE; // Dragoon weapon range

	Unit target = NULL;
	//int bestScore = 500;

	//Unitset units = Broodwar->enemy()->getUnits();
	Unitset units = owner->getUnitsInRadius(radius, Filter::IsEnemy);
	//Unitset units = owner->getUnitsInWeaponRange(wtype); // Causes crash!

	if (units.empty()) // No enemy in range, scanning all map
	{
		units = Broodwar->enemy()->getUnits();
		if (units.empty()) // Still no enemy, fail
			return BH_FAILURE;
		else // Enemy out of range, fine the closest one
		{
			target = *units.begin();
			for (Unitset::iterator unit = units.begin(); unit != units.end(); ++unit) 
			{
				if (owner->getPosition().getApproxDistance(unit->getPosition()) < owner->getPosition().getApproxDistance(target->getPosition()))
					target = *unit;
			}
		}

	}
	else //Enemy in range, find the lowest hp one
	{
		target = *units.begin();
		for (Unitset::iterator unit = units.begin(); unit != units.end(); ++unit) // Enemy in range, find the one with lowest hp
		{
			if (target->getHitPoints() + target->getShields() > unit->getHitPoints() + unit->getShields())
				target = *unit;
		}
	}


	/*
	for(Unitset::iterator unit = units.begin(); unit != units.end(); ++unit)
	{
		int score = unit->getHitPoints();
		score -= center.getApproxDistance(unit->getPosition());// / TILE_SIZE;
		if(score < bestScore)
		{
			bestScore = score;
			target = *unit;
		}
	}
	*/
	if(target)
	{
		s_blackboard->SetUnit("target", target);
		return BH_SUCCESS;
	}
	else
	{
		s_blackboard->SetUnit("target", NULL);
		return BH_FAILURE;
	}
}

////////////////////////////////////////////////////
// CalculateFallbackPos
////////////////////////////////////////////////////

BH_STATUS CalculateFallbackPos::Update(float deltaTime)
{
	Unit owner = NULL;
	s_blackboard->GetUnit("owner0", owner);
	if(owner == NULL || !owner->exists() || !owner->isUnderAttack())
		return BH_FAILURE;

	Player player = owner->getLastAttackingPlayer();
	if(player == NULL)
		return BH_FAILURE;

	WeaponType wtype = owner->getType().groundWeapon();
	//int radius = wtype.maxRange();// * TILE_SIZE;
	int radius = 6 * TILE_SIZE; // Dragoon weapon range
	Unitset units = owner->getUnitsInRadius(radius, Filter::IsEnemy);
	//Unitset units = owner->getUnitsInWeaponRange(wtype);
	if(units.empty())
		return BH_FAILURE;

	Position attackPos(0, 0);
	/*
	units = Broodwar->enemy()->getUnits();
	if(units.empty())
		return BH_FAILURE;
		*/
	for(Unitset::iterator unit = units.begin(); unit != units.end(); ++unit)
	{
		//if(unit->getPlayer() != owner->getPlayer())
		{
			attackPos += unit->getPosition();
		}
	}
	attackPos /= units.size();

	Broodwar->drawCircle(CoordinateType::Map, attackPos.x, attackPos.y, 5, Colors::Red, true);
	Position dir = owner->getPosition() - attackPos;
	dir /= 2;
	Broodwar->drawLine(CoordinateType::Map, owner->getPosition().x, owner->getPosition().y, attackPos.x, attackPos.y, Colors::Yellow);
	//dir *= (int)(TILE_SIZE * 2.0f / dir.getLength());
	Position pos = owner->getPosition() + dir;
	s_blackboard->SetPosition("moveto", pos);
	Broodwar->sendText("FallingBack");
	return BH_SUCCESS;
}

////////////////////////////////////////////////////
// CalculateRegroupPos
////////////////////////////////////////////////////

BH_STATUS CalculateRegroupPos::Update(float deltaTime)
{
	Unit owner = NULL;
	s_blackboard->GetUnit("owner0", owner);
	if(owner == NULL || !owner->exists() || !owner->isUnderAttack())
		return BH_FAILURE;

	Position movePos(0, 0);
	int radius = 6 * TILE_SIZE; // Dragoon weapon range
	Unitset units = owner->getUnitsInRadius(radius, Filter::IsAlly);
	for(Unitset::iterator unit = units.begin(); unit != units.end(); ++unit)
		movePos += unit->getPosition();
	movePos /= units.size();

	int dist = owner->getPosition().getApproxDistance(movePos);
	if(dist < 3)
		return BH_FAILURE;

	s_blackboard->SetPosition("moveto", movePos);

	return BH_SUCCESS;
}

////////////////////////////////////////////////////
// CheckCriticalTarget
////////////////////////////////////////////////////

BH_STATUS CheckCriticalTarget::Update(float deltaTime)
{
	Unit criticalTarget = NULL;
	s_blackboard->GetUnit("criticaltarget", criticalTarget);
	if(criticalTarget == NULL || !criticalTarget->exists())
		return BH_FAILURE;

	s_blackboard->SetUnit("target", criticalTarget);
	return BH_SUCCESS;
}

////////////////////////////////////////////////////
// FindGroupMoveToEnemy
////////////////////////////////////////////////////

BH_STATUS FindGroupTarget::Update(float deltaTime)
{
	UnitGroup * group = s_blackboard->GetUnitGroup();
	if(group == NULL || group->GetUnits().empty())
		return BH_FAILURE;
	Position center = group->CalcCenterPosition();

	list<Unit> units = group->GetUnits();
	//Unitset enemies = Broodwar->enemy()->getUnits();
	Unitset enemies = Broodwar->getUnitsInRadius(center, 4.0f * TILE_SIZE, Filter::IsEnemy && Filter::GetType == UnitTypes::Enum::Protoss_Zealot);
	if(enemies.empty())
		enemies = Broodwar->enemy()->getUnits();

	if(enemies.empty())
		return BH_FAILURE;
	int i = 0;

	auto it = units.begin();
	for(Unitset::iterator enemy = enemies.begin(); enemy != enemies.end(); ++enemy)
	{
		for(int unitsAssigned = 0; it != units.end() && unitsAssigned < 3; ++it, ++i)
		{
			++unitsAssigned;
			char name[10];
			sprintf_s(name, 10, "target%d", i);

			s_blackboard->SetUnit(name, *enemy);
		}
	}

	return BH_SUCCESS;
}

////////////////////////////////////////////////////
// FindCenterAttack
////////////////////////////////////////////////////

BH_STATUS FindCenterAttack::Update(float deltaTime)
{
	Position center;
	Unitset units = Broodwar->enemy()->getUnits();
	for(Unitset::iterator unit = units.begin(); unit != units.end(); ++unit)
	{
		center += unit->getPosition();
	}
	center /= Broodwar->enemy()->getUnits().size();

	s_blackboard->SetInt("moveattack", 1);

	int groupSize = 0;
	s_blackboard->GetInt("groupsize", groupSize);
	if(groupSize == 0)
		return BH_FAILURE;

	UnitGroup * group = s_blackboard->GetUnitGroup();
	if(group == NULL || group->GetUnits().empty())
		return BH_FAILURE;

	Position groupCenter = group->CalcCenterPosition();
	int i = 0;
	for(auto it = group->GetUnits().begin(); it != group->GetUnits().end(); ++it, ++i)
	{
		Unit unit = *it;
		Position movePos = groupCenter - unit->getPosition() + center;
		char name[10];
		sprintf_s(name, 10, "moveto%d", i);
		s_blackboard->GetPosition(name, center);
	}

	return BH_SUCCESS;
}

////////////////////////////////////////////////////
// FindCriticalTarget
////////////////////////////////////////////////////

BH_STATUS FindCriticalTarget::Update(float deltaTime)
{
	Unitset units = Broodwar->enemy()->getUnits();
	for(Unitset::iterator unit = units.begin(); unit != units.end(); ++unit)
	{
		if(unit->getType() == UnitTypes::Enum::Protoss_Reaver)
		{
			s_blackboard->GetUnit("criticaltarget", *unit);
			return BH_SUCCESS;
		}
	}

	return BH_FAILURE;
}

