#pragma once

#include "type_traits"
#include "memory"

namespace TaskQueue
{
	class QueueTask
	{
	public:
		virtual ~QueueTask() = default;
		virtual bool run() = 0;
	};

	template <typename Closure>
	class ClosureTask:public QueueTask
	{
	public:
		explicit ClosureTask(Closure&& closure):closure(std::forward<Closure>(closure)) {}

	private:
		bool run() override
		{
			closure();
			return true;
		}
		
		typename std::decay<Closure>::type closure;
	};

	template <typename Closure>
	std::unique_ptr<QueueTask> ToQueuedTask(Closure && closure)
	{
		return std::make_unique<ClosureTask<Closure>>(std::forward<Closure>(closure));
	}

	template <typename Closure, typename Cleanup>
	class ClosureTaskWithCleanup: public Closure<clk>
	
}
