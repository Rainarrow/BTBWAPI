#include "UnitGroup.h"
#include "BehaviorTree.h"
#include "Blackboard.h"
#include <assert.h>

UnitGroup::UnitGroup(Unit unit, Behavior * root, Blackboard * globalBlackboard) :
	m_unit(unit),
	m_root(root),
	m_globalBlackboard(globalBlackboard),
	m_localBlackboard(new Blackboard())
{
	assert(m_root);
	m_localBlackboard->SetUnit("owner", m_unit);
}

UnitGroup::~UnitGroup()
{
	SafeDelete(m_root);
	SafeDelete(m_localBlackboard);
}

void UnitGroup::Update()
{
	if(m_unit->exists())
	{
		s_blackboard = m_localBlackboard;
		m_root->Tick(0.067f);
	}
}
