#pragma once

enum event
{
	fx_step,
	fx_still,
	fx_jump,
	fx_collect,

	loading,
	collecting,
};

/**
 * \brief observes a specific subject and reacts to the subjects state changes
 */
class observer
{
public:
	virtual ~observer();

	/**
	 * \brief performs an action based on the event
	 * \param event is something that happens at runtime
	 */
	virtual void update(event event);
};

inline observer::~observer()
{
}

inline void observer::update(event event)
{
}

/**
 * \brief is observed by observers and pushes notifications to them
 */
class subject
{
public:
	virtual ~subject() = default;

	/**
	 * \brief add/register an observer to the subjects observer list
	 * \param observer to be added
	 */
	virtual void add_observer(observer& observer);

	/**
	 * \brief remove/unregister an observer from the observer list
	 * \param observer to be removed
	 */
	virtual void remove_observer(observer& observer);

	/**
	 * \brief calls update() on all observers in the observer list
	 * \param event to be passed to the observers
	 */
	virtual void notify_observers(event event);
};

inline void subject::add_observer(observer& observer)
{
}

inline void subject::remove_observer(observer& observer)
{
}

inline void subject::notify_observers(event event)
{
}
