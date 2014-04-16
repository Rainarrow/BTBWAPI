#include "ActionBehaviors.h"
#include "BehaviorTree.h"
#include "Blackboard.h"
#include "UnitGroup.h"

////////////////////////////////////////////////////
// MoveTo
////////////////////////////////////////////////////

void MoveTo::OnInitialize()
{
	Unit owner;
	bool result = s_blackboard->GetUnit("owner0", owner);
	assert(result);

	int attack = 0;
	s_blackboard->GetInt("moveattack", attack);
	m_moveAttack = attack != 0;

	result = s_blackboard->GetPosition("moveto", m_pos);
	//assert(result);
	m_pos = owner->getPosition();
	m_pos.x -= 200;
	//m_pos.y += 100;
}

BH_STATUS MoveTo::Update(float deltaTime)
{
	Unit owner = NULL;
	if(!s_blackboard->GetUnit("owner0", owner))
		return BH_FAILURE;

	Broodwar->drawLine(CoordinateType::Map, owner->getPosition().x, owner->getPosition().y, m_pos.x, m_pos.y, m_moveAttack ? Colors::Red : Colors::White);

	if(owner->isMoving())
		return BH_RUNNING;

	Position pos = owner->getPosition();
	if(pos.getApproxDistance(m_pos) < 2.0f)
		return BH_SUCCESS;
	if(GetStatus() == BH_RUNNING)
		return BH_FAILURE;

	if(!m_moveAttack)
		owner->move(m_pos);
	else
		owner->attack(m_pos);

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
	Position center = group->CalcCenterPosition();

	Unit target = NULL;
	int bestScore = 500;
	Unitset units = Broodwar->enemy()->getUnits();
	for(Unitset::iterator unit = units.begin(); unit != units.end(); ++unit)
	{
		int score = unit->getHitPoints();
		score -= center.getApproxDistance(unit->getPosition()) / TILE_SIZE;
		if(score < bestScore)
		{
			bestScore = score;
			target = *unit;
		}
	}
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
	int radius = wtype.maxRange() * TILE_SIZE;
	Unitset units = owner->getUnitsInRadius(radius);
	if(units.empty())
		return BH_FAILURE;

	Position attackPos;
	units = Broodwar->enemy()->getUnits();
	if(units.empty())
		return BH_FAILURE;
	for(Unitset::iterator unit = units.begin(); unit != units.end(); ++unit)
	{
		//if(unit->getPlayer() != owner->getPlayer())
		{
			attackPos += unit->getPosition();
		}
	}
	attackPos /= units.size();

	Position dir = owner->getPosition() - attackPos;
	dir /= 2;
	//dir *= (int)(TILE_SIZE * 2.0f / dir.getLength());
	Position pos = owner->getPosition() + dir;
	s_blackboard->SetPosition("moveto", pos);
	return BH_SUCCESS;
}

