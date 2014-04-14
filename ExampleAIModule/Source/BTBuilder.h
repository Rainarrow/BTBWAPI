#include "BehaviorTree.h"
#include <stack>

enum BTFLAGS {SEQ, SEL, ACT};


class BTBuilder 
{
public:
	BTBuilder(BTFLAGS NodeType); 
	BTBuilder& AddSequence();
	BTBuilder& AddAction(Behavior * action);
	BTBuilder& End();
	Behavior * GetTree();

private:
	std::stack<BTFLAGS> m_flags;
	Behavior * m_root;
	std::stack<Behavior *> m_internalNodes;
	Behavior * m_currentNode;
	int m_depthCounter;
};