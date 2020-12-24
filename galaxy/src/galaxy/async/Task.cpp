///
/// Task.cpp
/// protostar
///
/// Refer to LICENSE.txt for more details.
///

#include "Task.hpp"

///
/// Core namespace.
///
namespace pr
{
	Task::Task()
	    : m_done {false}
	{
	}

	Task::~Task()
	{
		m_done = true;
	}

	void Task::set(std::function<void(void)>&& func)
	{
		m_task = std::move(func);
	}

	void Task::exec()
	{
		m_task();
		m_done = true;
		std::atomic_notify_one(&m_done);
	}

	void Task::wait_until_done()
	{
		if (!m_done)
		{
			std::atomic_wait(&m_done, false);
		}
	}

	bool Task::is_done()
	{
		return m_done;
	}
} // namespace pr