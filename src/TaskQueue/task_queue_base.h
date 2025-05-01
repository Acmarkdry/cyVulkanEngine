#pragma once

#include <memory>
#include <string>
#include "queue_task.h"

namespace TaskQueue
{
	class TaskQueueBase
	{
	public:
		virtual void deleteThis() = 0;

		virtual void postTask(std::unique_ptr<QueueTask> task) = 0;
		virtual void postDelayTask(std::unique_ptr<QueuedTask> task, uint32_t milliseconds) = 0;

		static TaskQueueBase* current();
		bool isCurrent() const
		{
			return current() == this;
		}

		virtual const std::string& name() const = 0;

	protected:
		class CurrentTaskQueueSetter
		{
		public:
			explicit CurrentTaskQueueSetter(TaskQueueBase* taskQueue);
			CurrentTaskQueueSetter(const CurrentTaskQueueSetter&) = delete;
			CurrentTaskQueueSetter& operator=(const CurrentTaskQueueSetter&) = delete;
			~CurrentTaskQueueSetter();
			
		private:
			TaskQueueBase* const _previous;
		};

		virtual ~TaskQueueBase() = default;
	};

	struct TaskQueueDeleter
	{
		void operator()(TaskQueueBase* taskQueue) const
		{
			taskQueue->deleteThis();
		}
	};
	
	
}
