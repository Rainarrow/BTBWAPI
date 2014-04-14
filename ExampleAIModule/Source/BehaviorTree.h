#pragma once

#include <BWAPI.h>
#include <vector>
#include <string>
using namespace std;
using namespace BWAPI;

////////////////////////////////////////////////////

template<typename T>
void SafeDelete(T *& val)
{
	if(val)
	{
		delete val;
		val = NULL;
	}
}

template<typename T>
void SafeDeleteArray(T *& val)
{
	if(val)
	{
		delete[] val;
		val = NULL;
	}
}

////////////////////////////////////////////////////

enum BH_STATUS
{
	BH_INVALID,
	BH_SUCCESS,
	BH_FAILURE,
	BH_RUNNING,
	BH_ABORTED,
};

class Behavior;
typedef vector<Behavior* > Behaviors;

////////////////////////////////////////////////////

class Behavior
{
public:

	Behavior() : m_status(BH_INVALID)
	{
	}

	virtual ~Behavior()
	{
	}

	virtual void			OnInitialize() {}
	virtual void			OnTerminate(BH_STATUS) {}
	virtual BH_STATUS		Update(float deltaTime) = 0;
	virtual void			AddChild(Behavior * child) {}

	BH_STATUS				Tick(float deltaTime);
	void					Reset();
	void					Abort();
	bool					IsTerminated() const;
	bool					IsRunning() const;
	BH_STATUS				GetStatus() const;

private:

	BH_STATUS				m_status;
};

////////////////////////////////////////////////////
// Decorators
////////////////////////////////////////////////////

class Decorator : public Behavior
{
public:
	Decorator() : m_child(NULL) {}
	virtual ~Decorator();
	virtual void			AddChild(Behavior * child);

protected:
	Behavior *				m_child;
};

class Repeat : public Decorator
{
public:
	Repeat();

	virtual void			OnInitialize();
	virtual BH_STATUS		Update(float deltaTime);

	void					SetCount(int cnt);

private:
	int						m_limit;
	int						m_counter;
};

////////////////////////////////////////////////////
// Composites
////////////////////////////////////////////////////

class Composite : public Behavior
{
public:
	virtual ~Composite();

	virtual void			AddChild(Behavior * child);
	void					RemoveChild(Behavior *);
	void					ClearChildren();

protected:
	Behaviors				m_children;
};

class Sequence : public Composite
{
public:

	virtual void			OnInitialize();
	virtual BH_STATUS		Update(float deltaTime);

protected:
	Behaviors::iterator		m_currentChild;
};

class Selector : public Composite
{
public:

	virtual void			OnInitialize();
	virtual BH_STATUS		Update(float deltaTime);

protected:
	Behaviors::iterator		m_currentChild;
};

class Parallel : public Composite
{
public:
	enum POLICY
	{
		REQUIRE_ONE,
		REQUIRE_ALL,
	};

	Parallel(POLICY forSuccess, POLICY forFailure);

	virtual BH_STATUS		Update(float deltaTime);
	virtual void			OnTerminate(BH_STATUS status);

protected:
	POLICY					m_successPolicy;
	POLICY					m_failurePolicy;
};

class ActiveSelector : public Selector
{
public:

	virtual void			OnInitialize();
	virtual BH_STATUS		Update(float deltaTime);
};

////////////////////////////////////////////////////

class MoveTo : public Behavior
{
public:

	virtual void			OnInitialize();
	virtual BH_STATUS		Update(float deltaTime);

private:
	Position				m_pos;
	bool					m_moveAttack;
};

class Attack : public Behavior
{
public:

	virtual void			OnInitialize();
	virtual BH_STATUS		Update(float deltaTime);

private:
	Unit					m_target;
};

class AttackMoveTo : public Behavior
{
public:

	virtual void			OnInitialize();
	virtual BH_STATUS		Update(float deltaTime);

private:
	Unit					m_target;
};

class Delay : public Behavior
{
public:

	virtual void			OnInitialize();
	virtual BH_STATUS		Update(float deltaTime);
	void					SetDelay(float delay);

private:
	float					m_delayLeft;
};
class Blackboard;
extern Blackboard * s_blackboard;
