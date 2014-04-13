#include "BehaviorTree.h"
#include "Blackboard.h"
#include <assert.h>

Blackboard * s_blackboard = NULL;

////////////////////////////////////////////////////
// Behavior
////////////////////////////////////////////////////

BH_STATUS Behavior::Tick(float deltaTime)
{
	if (m_status != BH_RUNNING)
	{
		OnInitialize();
	}

	m_status = Update(deltaTime);

	if (m_status != BH_RUNNING)
	{
		OnTerminate(m_status);
	}
	return m_status;
}

void Behavior::Reset()
{
	m_status = BH_INVALID;
}

void Behavior::Abort()
{
	OnTerminate(BH_ABORTED);
	m_status = BH_ABORTED;
}

bool Behavior::IsTerminated() const
{
	return m_status == BH_SUCCESS  ||  m_status == BH_FAILURE;
}

bool Behavior::IsRunning() const
{
	return m_status == BH_RUNNING;
}

BH_STATUS Behavior::GetStatus() const
{
	return m_status;
}

////////////////////////////////////////////////////
// Repeat Decorator
////////////////////////////////////////////////////

Decorator::~Decorator()
{
	SafeDelete(m_child);
}

void Decorator::AddChild(Behavior * child)
{
	assert(m_child == NULL);
	m_child = child;
}

Repeat::Repeat() :
	m_limit(0),
	m_counter(0)
{
}

void Repeat::SetCount(int cnt)
{
	m_limit = cnt;
}

void Repeat::OnInitialize()
{
	assert(m_child);
	m_counter = 0;
}

BH_STATUS Repeat::Update(float deltaTime)
{
	for(;;)
	{
		m_child->Tick(deltaTime);
		if (m_child->GetStatus() == BH_RUNNING)
			break;
		if (m_child->GetStatus() == BH_FAILURE)
			return BH_FAILURE;
		if(++m_counter == m_limit)
			return BH_SUCCESS;
		m_child->Reset();
	}
	return BH_INVALID;
}

////////////////////////////////////////////////////
// Composites
////////////////////////////////////////////////////

Composite::~Composite()
{
	for(auto it = m_children.begin(); it != m_children.end(); ++it)
		SafeDelete(*it);
	m_children.clear();
}

void Composite::AddChild(Behavior * child)
{
	m_children.push_back(child);
}

void Composite::RemoveChild(Behavior * child)
{
	assert(!"Not Implemented");
}

void Composite::ClearChildren()
{
	m_children.clear();
}

void Sequence::OnInitialize()
{
	assert(!m_children.empty());
	m_currentChild = m_children.begin();
}

BH_STATUS Sequence::Update(float deltaTime)
{
	// Keep going until a child behavior says it's running.
	for(;;)
	{
		BH_STATUS status = (*m_currentChild)->Tick(deltaTime);

		// If the child fails, or keeps running, do the same.
		if(status != BH_SUCCESS)
		{
			return status;
		}

		// Hit the end of the array, job done!
		if(++m_currentChild == m_children.end())
		{
			return BH_SUCCESS;
		}
	}
}

void Selector::OnInitialize()
{
	assert(!m_children.empty());
	m_currentChild = m_children.begin();
}

BH_STATUS Selector::Update(float deltaTime)
{
	// Keep going until a child behavior says its running.
	for(;;)
	{
		BH_STATUS status = (*m_currentChild)->Tick(deltaTime);

		// If the child succeeds, or keeps running, do the same.
		if(status != BH_FAILURE)
		{
			return status;
		}

		// Hit the end of the array, it didn't end well...
		if(++m_currentChild == m_children.end())
		{
			return BH_FAILURE;
		}
	}
}

Parallel::Parallel(POLICY forSuccess, POLICY forFailure) :
	m_successPolicy(forSuccess),
	m_failurePolicy(forFailure)
{
}

BH_STATUS Parallel::Update(float deltaTime)
{
	int successCnt = 0, failureCnt = 0;

	for(auto it = m_children.begin(); it != m_children.end(); ++it)
	{
		Behavior * behavior = *it;
		if(!behavior->IsTerminated())
		{
			behavior->Tick(deltaTime);
		}

		if(behavior->GetStatus() == BH_SUCCESS)
		{
			++successCnt;
			if(m_successPolicy == REQUIRE_ONE)
			{
				return BH_SUCCESS;
			}
		}
		else if(behavior->GetStatus() == BH_FAILURE)
		{
			++failureCnt;
			if(m_failurePolicy == REQUIRE_ONE)
			{
				return BH_FAILURE;
			}
		}
	}

	if(m_failurePolicy == REQUIRE_ALL && failureCnt == m_children.size())
	{
		return BH_FAILURE;
	}
	if(m_successPolicy == REQUIRE_ALL && successCnt == m_children.size())
	{
		return BH_SUCCESS;
	}

	return BH_RUNNING;
}

void Parallel::OnTerminate(BH_STATUS status)
{
	for(auto it = m_children.begin(); it != m_children.end(); ++it)
	{
		Behavior * behavior = *it;
		if (behavior->IsRunning())
		{
			behavior->Abort();
		}
	}
}

void ActiveSelector::OnInitialize()
{
	assert(!"Not to be called");
	m_currentChild = m_children.end();
}

BH_STATUS ActiveSelector::Update(float deltaTime)
{
	auto prev = m_currentChild;

	Selector::OnInitialize();
	BH_STATUS result = Selector::Update(deltaTime);

	if(prev != m_children.end() && m_currentChild != prev)
	{
		(*prev)->OnTerminate(BH_ABORTED);
	}
	return result;
}

////////////////////////////////////////////////////

void MoveTo::OnInitialize()
{
	Unit owner = s_blackboard->GetUnit("owner");
	m_pos = owner->getPosition();
	m_pos.x -= 200;
	//m_pos.y += 100;
}

BH_STATUS MoveTo::Update(float deltaTime)
{
	Unit owner = s_blackboard->GetUnit("owner");
	if(owner->isMoving())
		return BH_RUNNING;
	Position pos = owner->getPosition();
	if(pos.getApproxDistance(m_pos) < 2.0f)
		return BH_SUCCESS;
	if(GetStatus() == BH_RUNNING)
		return BH_FAILURE;

	owner->attack(m_pos);
	return BH_RUNNING;
}

//////////////////////////////
// ATTACK BEHAVIOR
//////////////////////////////

void Attack::OnInitialize()
{
	m_target = s_blackboard->GetUnit("target");
}

BH_STATUS Attack::Update(float deltaTime)
{
	if(!m_target->exists())
		return BH_FAILURE;

	Unit owner = s_blackboard->GetUnit("owner");

	UnitCommand currentCmd(owner->getLastCommand());

	if (currentCmd.getType() == UnitCommandTypes::Attack_Unit && currentCmd.getTarget() == m_target)
		return BH_RUNNING;

	if (owner->isAttackFrame()) // Unit is in Attack animation
		return BH_RUNNING;

	//Unit is not attacking its designated target, make it do so

	owner->attack(m_target);
	return BH_SUCCESS;

	//Does this ever fail?

	//@TODO: DebugDraw
}

void Delay::OnInitialize()
{
	m_delayLeft = 3.0f;
}

BH_STATUS Delay::Update(float deltaTime)
{
	m_delayLeft -= deltaTime;
	if(m_delayLeft > 0.0f)
		return BH_RUNNING;
	return BH_SUCCESS;
}

