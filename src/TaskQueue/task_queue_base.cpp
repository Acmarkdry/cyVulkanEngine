#include "task_queue_base.h"
#include "thread"

namespace TaskQueue
{
	namespace
	{
		thread_local TaskQueueBase* _current = nullptr;
	}

	TaskQueueBase* TaskQueueBase::current() {
		return _current;
	}

	TaskQueueBase::CurrentTaskQueueSetter::CurrentTaskQueueSetter(TaskQueueBase* taskQueue)
		: _previous(_current) {
		// todo 这样会报错，不太明白
		_previous = _current;
		_current = taskQueue;
	}

	TaskQueueBase::CurrentTaskQueueSetter::~CurrentTaskQueueSetter() {
		_current = _previous;
	}


	
}
