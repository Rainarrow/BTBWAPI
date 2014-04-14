#include "Blackboard.h"
#include <assert.h>

Blackboard::Blackboard(Blackboard * parent) :
	m_parent(parent)
{
}

Blackboard::~Blackboard()
{
}

void Blackboard::SetUnit(string const & name, Unit unit)
{
	m_units[name] = unit;
}

void Blackboard::SetPosition(string const & name, Position const & pos)
{
	m_positions[name] = pos;
}

void Blackboard::SetInt(string const & name, int value)
{
	m_ints[name] = value;
}

bool Blackboard::GetUnit(string const & name, Unit & unit) const
{
	auto it = m_units.find(name);
	if(it != m_units.end())
	{
		unit = it->second;
		return true;
	}

	if(m_parent)
		return m_parent->GetUnit(name, unit);

	return false;
}

bool Blackboard::GetPosition(string const & name, Position & pos) const
{
	auto it = m_positions.find(name);
	if(it != m_positions.end())
	{
		pos = it->second;
		return true;
	}

	if(m_parent)
		return m_parent->GetPosition(name, pos);

	return false;
}

bool Blackboard::GetInt(string const & name, int & value) const
{
	auto it = m_ints.find(name);
	if(it != m_ints.end())
	{
		value = it->second;
		return true;
	}

	if(m_parent)
		return m_parent->GetInt(name, value);

	return false;
}

void Blackboard::RemoveUnit(string const & name)
{
	m_units.erase(name);
}

void Blackboard::RemovePosition(string const & name)
{
	m_positions.erase(name);
}

void Blackboard::RemoveInt(string const & name)
{
	m_ints.erase(name);
}

