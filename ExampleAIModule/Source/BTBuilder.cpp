#include "BTBuilder.h"
#include <iostream>

BTBuilder::BTBuilder(Behavior * root)
{
	m_root = root;
}

BTBuilder& BTBuilder::AddSequence()
{
	Behavior * seq = new Sequence;

	if(m_internalNodes.empty()) 
		m_root->AddChild(seq);
	else
		m_internalNodes.top()->AddChild(seq);

	m_internalNodes.push(seq);

	return *this;
}

BTBuilder& BTBuilder::AddSelector()
{
	Behavior * sel = new Selector;

	if(m_internalNodes.empty()) 
		m_root->AddChild(sel);
	else
		m_internalNodes.top()->AddChild(sel);

	m_internalNodes.push(sel);
	return *this;
}

BTBuilder& BTBuilder::AddActiveSelector()
{
	Behavior * asel = new ActiveSelector;

	if(m_internalNodes.empty()) 
		m_root->AddChild(asel);
	else
		m_internalNodes.top()->AddChild(asel);

	m_internalNodes.push(asel);
	return *this;
}

BTBuilder& BTBuilder::AddParallel(Parallel::POLICY forSuccess, Parallel::POLICY forFailure)
{
	Behavior * par = new Parallel(forSuccess, forFailure);

	if(m_internalNodes.empty())
		m_root->AddChild(par);
	else
		m_internalNodes.top()->AddChild(par);

	m_internalNodes.push(par);
	return *this;
}
BTBuilder& BTBuilder::AddAction(Behavior * action)
{
	if(m_internalNodes.empty())
		m_root->AddChild(action);
	else
		m_internalNodes.top()->AddChild(action);

	return *this;
}

BTBuilder& BTBuilder::End()
{
	assert(!m_internalNodes.empty());

	m_internalNodes.pop();

	return *this;
}

BTBuilder& BTBuilder::AddDecorator(Decorator * dec)
{
	if(m_internalNodes.empty())
		m_root->AddChild(dec);
	else
		m_internalNodes.top()->AddChild(dec);

	m_internalNodes.push(dec);
	return *this;
}

Behavior * BTBuilder::GetTree() const
{
	if(m_internalNodes.empty())
		return m_root;
	else
		return nullptr;
}
