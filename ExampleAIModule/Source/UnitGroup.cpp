#include "UnitGroup.h"
#include "BehaviorTree.h"
#include "Blackboard.h"
#include <assert.h>

using namespace BWAPI;

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

Position UnitGroup::CalcCenterPosition() const
{
	assert(!m_units.empty());

	Position result(0, 0);

	for (const auto u : m_units)
	{
		result += u->getPosition();
	}

	result.x /= m_units.size();
	result.y /= m_units.size();

	return result;
