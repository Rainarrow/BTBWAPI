#include "ConditionBehaviors.h"
#include "BehaviorTree.h"
#include "Blackboard.h"
#include "UnitGroup.h"

BH_STATUS IsLowHp::Update(float deltaTime)
{
	Unit owner = NULL;
	s_blackboard->GetUnit("owner0", owner);
	if(owner == NULL || !owner->exists())
		return BH_FAILURE;

	float ratio = ((float)owner->getHitPoints()) / owner->getType().maxHitPoints();
	return ratio < 0.8f ? BH_SUCCESS : BH_FAILURE;
}

BH_STATUS IsBeingAttacked::Update(float deltaTime)
{
	Unit owner = NULL;
	s_blackboard->GetUnit("owner0", owner);
	if(owner == NULL || !owner->exists())
		return BH_FAILURE;

	return owner->isUnderAttack() ? BH_SUCCESS : BH_FAILURE;
}

