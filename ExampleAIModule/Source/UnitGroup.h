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

private:
	Unit			m_unit;
	Behavior *		m_root;
	Blackboard *	m_globalBlackboard;
	Blackboard *	m_localBlackboard;
};
