#pragma once

#include <BWAPI.h>
#include <string>
#include <list>
using namespace std;
using namespace BWAPI;

class Blackboard;
class Behavior;
class UnitGroup
{
public:
	UnitGroup(Behavior * root, Blackboard * globalBlackboard);
	~UnitGroup();

	void					AddUnit(Unit unit);
	void					RemoveUnit(Unit unit);
	void					Update();

	Position				CalcCenterPosition() const; //Returns the average position of the group
	bool					IsPositionWalkable(const Position& pos) const;
	list<Unit> const &		GetUnits() const;

private:
	list<Unit>				m_units;
	Behavior *				m_root;
	Blackboard *			m_globalBlackboard;
	Blackboard *			m_localBlackboard;
};
