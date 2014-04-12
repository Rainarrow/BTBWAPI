#include "Blackboard.h"
#include <assert.h>

Blackboard::Blackboard() :
	m_unit(NULL),
	m_pos()
{
}

Blackboard::~Blackboard()
{
}

void Blackboard::SetUnit(string const & name, Unit unit)
{
	m_unit = unit;
}

void Blackboard::SetPosition(string const & name, Position const & pos)
{
	m_pos = pos;
}

Unit Blackboard::GetUnit(string const & name)
{
	return m_unit;
}

Position Blackboard::GetPosition(string const & name)
{
	return m_pos;
}

