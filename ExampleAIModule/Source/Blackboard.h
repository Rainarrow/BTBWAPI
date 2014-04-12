#pragma once

#include <BWAPI.h>
#include <string>
using namespace std;
using namespace BWAPI;

class Blackboard
{
public:
	Blackboard();
	~Blackboard();

	void		SetUnit(string const & name, Unit unit);
	void		SetPosition(string const & name, Position const & pos);

	Unit		GetUnit(string const & name);
	Position	GetPosition(string const & name);

private:
	Unit		m_unit;
	Position	m_pos;
};