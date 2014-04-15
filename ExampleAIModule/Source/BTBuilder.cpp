#include "BTBuilder.h"
#include <iostream>

BTBuilder::BTBuilder(BTFLAGS NodeType = SEQ, Parallel::POLICY forSuccess = Parallel::REQUIRE_ALL, Parallel::POLICY forFailure = Parallel::REQUIRE_ALL) : m_depthCounter(0)
{
	if(NodeType == SEQ)
		m_root = new Sequence();
	else if(NodeType == SEL)
		m_root = new Selector();
	else if(NodeType == ASEL)
		m_root = new ActiveSelector();
	else if(NodeType == PAR)
		m_root = new Parallel(forSuccess, forFailure);
}

BTBuilder& BTBuilder::AddSequence()
{
	m_depthCounter++;
	m_flags.push(SEQ);
	Behavior * seq = new Sequence;
	m_root->AddChild(seq);
	m_internalNodes.push(seq);

	return *this;
}

BTBuilder& BTBuilder::AddSelector()
{
	m_depthCounter++;
	m_flags.push(SEL);
	Behavior * sel = new Selector();
	m_root->AddChild(sel);
	m_internalNodes.push(sel);

	return *this;
}

BTBuilder& BTBuilder::AddActiveSelector()
{
	m_depthCounter++;
	m_flags.push(ASEL);
	Behavior * asel = new ActiveSelector();
	m_root->AddChild(asel);
	m_internalNodes.push(asel);

	return *this;
}

BTBuilder& BTBuilder::AddParallel(Parallel::POLICY forSuccess, Parallel::POLICY forFailure)
{
	m_depthCounter++;
	m_flags.push(PAR);
	Behavior * par = new Parallel(forSuccess, forFailure);
	m_root->AddChild(par);
	m_internalNodes.push(par);

	return *this;
}
BTBuilder& BTBuilder::AddAction(Behavior * action)
{
	if(m_depthCounter == 0 && m_internalNodes.empty())
		m_root->AddChild(action);
	else if (m_depthCounter != 0 && !m_internalNodes.empty())
		m_internalNodes.top()->AddChild(action);
	else
		std::cerr << "Error: Internal Node stack mismatch with flag stack";

	return *this;
}

BTBuilder& BTBuilder::End()
{
	assert(!m_flags.empty() && !m_internalNodes.empty());

	if(m_flags.top() == SEQ || m_flags.top() == SEL || m_flags.top() == ASEL || m_flags.top() == PAR) 
	{
		m_depthCounter--;
		m_internalNodes.pop();
	}
	m_flags.pop();

	return *this;
}

Behavior * BTBuilder::GetTree()
{
	if(m_flags.empty() && m_depthCounter == 0)
		return m_root;
	else
		return nullptr;
}