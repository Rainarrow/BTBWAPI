#include "UnitGroup.h"
#include "BehaviorTree.h"
#include "Blackboard.h"
#include <assert.h>

UnitGroup::UnitGroup(int unitType, unsigned unitCnt, Behavior * root, Blackboard * globalBlackboard) :
	m_unitType(unitType),
	m_unitCnt(unitCnt),
	m_root(root),
	m_globalBlackboard(globalBlackboard),
	m_localBlackboard(NULL)
{
	m_localBlackboard = new Blackboard(m_globalBlackboard);
	m_localBlackboard->SetUnitGroup(this);
	assert(m_root);
}

UnitGroup::~UnitGroup()
{
	SafeDelete(m_root);
	SafeDelete(m_localBlackboard);
}

bool UnitGroup::AddUnit(Unit unit)
{
	if(unit->getType() == m_unitType && m_units.size() < m_unitCnt)
	{
		m_units.push_back(unit);
		char name[20];
		sprintf_s(name, 20, "owner%d", m_units.size() - 1);
		m_localBlackboard->SetUnit(name, unit);
		m_localBlackboard->SetInt("groupsize", m_units.size());
		return true;
	}
	return false;
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

list<Unit> const & UnitGroup::GetUnits() const
{
	return m_units;
}

bool UnitGroup::UnitReadyToAtk(const Unit& unit) const
{
	return (unit->getAirWeaponCooldown() == 0 || unit->getGroundWeaponCooldown());
}
