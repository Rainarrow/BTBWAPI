#pragma once

#include <BWAPI.h>
#include <string>
using namespace std;
using namespace BWAPI;

class Blackboard;
class Behavior;
class UnitGroup
{
public:
	UnitGroup(Unit unit, Behavior * root, Blackboard * globalBlackboard);
	~UnitGroup();

	void			Update();

	Position		CalcCenterPosition() const; //Returns the average position of the group
	bool			IsPositionWalkable(const Position& pos) const;

private:
	Unit			m_unit;
	vector<Unit>	m_units;
	Behavior *		m_root;
	Blackboard *	m_globalBlackboard;
	Blackboard *	m_localBlackboard;
};
