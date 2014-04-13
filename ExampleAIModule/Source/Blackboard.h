#pragma once

#include <BWAPI.h>
#include <string>
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
	Blackboard();
	~Blackboard();

	void		SetUnit(string const & name, Unit unit);
	void		SetPosition(string const & name, Position const & pos);

	Unit		GetUnit(string const & name);
	Position	GetPosition(string const & name);

	Unit		GetTarget() const;

private:
	Unit		m_unit;
	Position	m_pos;
	vector<Target> m_targets;
};