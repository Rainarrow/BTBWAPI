#include "BehaviorTree.h"
#include <stack>

enum BTFLAGS {SEQ, SEL, ASEL, PAR, ILAN, ACT};


class BTBuilder 
{
public:
	BTBuilder(BTFLAGS NodeType, Parallel::POLICY forSuccess, Parallel::POLICY forFailure); 
	BTBuilder& AddSequence();
	BTBuilder& AddSelector();
	BTBuilder& AddActiveSelector();
	BTBuilder& AddParallel(Parallel::POLICY forSuccess, Parallel::POLICY forFailure);

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