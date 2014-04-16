#include "BehaviorTree.h"
#include <stack>

enum NODETYPE {SEQ, SEL, ASEL, PAR, DEC, ACT}; //WARNING: BEHAVIORTREE.H HAS ITS OWN COPY OF THIS; KEEP THOSE IDENTICAL


class BTBuilder 
{
public:
	BTBuilder(Behavior * root);
	BTBuilder& AddSequence();
	BTBuilder& AddSelector();
	BTBuilder& AddActiveSelector();
	BTBuilder& AddParallel(Parallel::POLICY forSuccess, Parallel::POLICY forFailure);

	BTBuilder& AddAction(Behavior * action);
	BTBuilder& End();
	Behavior * GetTree() const;

private:
	std::stack<Behavior *> m_internalNodes;
	Behavior * m_root;
};