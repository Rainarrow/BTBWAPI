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
	Unit owner;
	if(!s_blackboard->GetUnit("owner0", owner))
		return BH_FAILURE;

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
	Unitset myUnits = Broodwar->enemy()->getUnits();
	for(Unitset::iterator unit = myUnits.begin(); unit != myUnits.end(); ++unit)
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
