#include "BTBuilder.h"

BTBuilder::BTBuilder(BTFLAGS NodeType = SEQ) : m_depthCounter(0)
{
	if(NodeType == SEQ)
		m_root = new Sequence();
	//@TODO: Add Other types
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

BTBuilder& BTBuilder::AddAction(Behavior * action)
{
	if(m_depthCounter == 0 && m_internalNodes.empty())
		m_root->AddChild(action);
	else
		m_internalNodes.top()->AddChild(action);

	return *this;
}

BTBuilder& BTBuilder::End()
{
	if(m_flags.top() == SEQ) //@TODO add other types
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