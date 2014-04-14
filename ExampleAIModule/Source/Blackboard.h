#pragma once

#include <BWAPI.h>
#include <string>
#include <hash_map>

using namespace std;
using namespace BWAPI;

struct Target
{
	Unit unit;
	double distance; //Distance from center of the group
	int underAtkOfMelee;
	int underAtkOfRanged;
	bool isHighPriority;
};

class UnitGroup;
class Blackboard
{
public:
	Blackboard(Blackboard * parent = NULL);
	~Blackboard();

	void			SetUnit(string const & name, Unit unit);
	void			SetPosition(string const & name, Position const & pos);
	void			SetInt(string const & name, int value);
	void			SetUnitGroup(UnitGroup * group);

	void			RemoveUnit(string const & name);
	void			RemovePosition(string const & name);
	void			RemoveInt(string const & name);

	Unit		GetTarget() const;
	Unit		FindBestTargetForDragoon() const;
	bool		GetUnit(string const & name, Unit & unit) const;
	bool		GetInt(string const & name, int & value) const;
	bool			GetPosition(string const & name, Position & pos) const;
	UnitGroup *		GetUnitGroup() const;

private:

	Blackboard *				m_parent;
	UnitGroup *					m_unitGroup;
	hash_map<string, Unit>		m_units;
	hash_map<string, Position>	m_positions;
	hash_map<string, int>		m_ints;

	//vector<Target> m_targets;
};