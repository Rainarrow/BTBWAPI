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
	int underAtkOf;  //
	bool isHighPriority;
};

class Blackboard
{
public:
	Blackboard(Blackboard * parent = NULL);
	~Blackboard();

	void		SetUnit(string const & name, Unit unit);
	void		SetPosition(string const & name, Position const & pos);
	void		SetInt(string const & name, int value);

	void		RemoveUnit(string const & name);
	void		RemovePosition(string const & name);
	void		RemoveInt(string const & name);

	bool		GetUnit(string const & name, Unit & unit) const;
	bool		GetPosition(string const & name, Position & pos) const;
	bool		GetInt(string const & name, int & value) const;

private:

	Blackboard *				m_parent;
	hash_map<string, Unit>		m_units;
	hash_map<string, Position>	m_positions;
	hash_map<string, int>		m_ints;

	//vector<Target> m_targets;
};