#include "UnitGroup.h"
#include "BehaviorTree.h"
#include "Blackboard.h"
#include <assert.h>

UnitGroup::UnitGroup(Behavior * root, Blackboard * globalBlackboard) :
	m_root(root),
	m_globalBlackboard(globalBlackboard),
	m_localBlackboard(new Blackboard())
{
	assert(m_root);
}

UnitGroup::~UnitGroup()
{
	SafeDelete(m_root);
	SafeDelete(m_localBlackboard);
}

void UnitGroup::AddUnit(Unit unit)
{
	m_units.push_back(unit);
	m_localBlackboard->SetUnit("owner", unit);
}

void UnitGroup::RemoveUnit(Unit unit)
{
	m_units.remove(unit);
}

void UnitGroup::Update()
{
	if(!m_units.empty())
	{
		s_blackboard = m_localBlackboard;
		m_root->Tick(0.067f);
	}
}

Position UnitGroup::CalcCenterPosition() const
{
	assert(!m_units.empty());

	Position result(0, 0);

	for(auto it = m_units.begin(); it != m_units.end(); ++it)
	{
		result += (*it)->getPosition();
	}

	result.x /= m_units.size();
	result.y /= m_units.size();

	return result;

}