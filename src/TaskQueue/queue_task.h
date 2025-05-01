#pragma once

#include <type_traits>
#include <memory>

/*
 * 核心组件，将闭包封装为任务对象，支持任务执行和资源清理
 */
namespace TaskQueue
{
	class QueuedTask
	{
	public:
		virtual ~QueuedTask() = default;
		virtual bool run() = 0;
	};

	// 通用闭包模板
	template <typename Closure>
	class ClosureTask:public QueuedTask
	{
	public:
		// std::forward捕获闭包，run()执行闭包并返回true
		explicit ClosureTask(Closure&& closure):closure_(std::forward<Closure>(closure)) {}

	private:
		bool run() override
		{
			closure_();
			return true;
		}

		// decay关键字，将类型转换为对应的衰变形式，或者去除对应的引用付
		typename std::decay<Closure>::type closure_;
	};

	
	template <typename Closure>
	std::unique_ptr<QueuedTask> ToQueuedTask(Closure&& closure)
	{
		return std::make_unique<ClosureTask<Closure>>(std::forward<Closure>(closure));
	}

	// 析构函数里面调用清理闭包，用于资源释放
	template <typename Closure, typename Cleanup>
	class ClosureTaskWithCleanup: public ClosureTask<Closure>
	{
	public:
		ClosureTaskWithCleanup(Closure&& closure, Cleanup&& cleanup)
			: ClosureTask<Closure>(std::forward<Closure>(closure))
			, cleanup_(std::forward<Cleanup>(cleanup)) {}
		~ClosureTaskWithCleanup() override { cleanup_(); }
		
	private:
		typename std::decay<Cleanup>::type cleanup_;
	};
	
}
