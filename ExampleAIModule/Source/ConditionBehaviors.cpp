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

	//if(owner->getShields() == 0) //For Protoss only
	//	return BH_SUCCESS;

	float ratio = (float)(owner->getHitPoints() + owner->getShields()) / owner->getType().maxHitPoints();
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

BH_STATUS IsReadyToAttack::Update(float deltaTime)
{
	Unit owner = NULL;
	s_blackboard->GetUnit("owner0", owner);

	if(owner == NULL || !owner->exists())
		return BH_FAILURE;

	
	if (owner->isAttackFrame() || (owner->getAirWeaponCooldown() == 0 && owner->getGroundWeaponCooldown() == 0))
	{
		//Broodwar->sendText("CD");
		return BH_SUCCESS;
	}
	else
	{
		//Broodwar->sendText("NCD");
		return BH_FAILURE;
	}
}