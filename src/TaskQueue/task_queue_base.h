#pragma once

#include <memory>
#include <string>
#include "queued_task.h"

namespace TaskQueue {

// Asynchronously executes tasks in a way that guarantees that they're executed
// in FIFO order and that tasks never overlap. Tasks may always execute on the
// same worker thread and they may not. To DCHECK that tasks are executing on a
// known task queue, use IsCurrent().
class TaskQueueBase {
public:
    // Starts destruction of the task queue.
    // On return ensures no task are running and no new tasks are able to start
    // on the task queue.
    // Responsible for deallocation. Deallocation may happen syncrhoniously during
    // Delete or asynchronously after Delete returns.
    // Code not running on the TaskQueue should not make any assumption when
    // TaskQueue is deallocated and thus should not call any methods after Delete.
    // Code running on the TaskQueue should not call Delete, but can assume
    // TaskQueue still exists and may call other methods, e.g. PostTask.
    virtual void deleteThis() = 0;

    // Schedules a task to execute. Tasks are executed in FIFO order.
    // If |task->Run()| returns true, task is deleted on the task queue
    // before next QueuedTask starts executing.
    // When a TaskQueue is deleted, pending tasks will not be executed but they
    // will be deleted. The deletion of tasks may happen synchronously on the
    // TaskQueue or it may happen asynchronously after TaskQueue is deleted.
    // This may vary from one implementation to the next so assumptions about
    // lifetimes of pending tasks should not be made.
    virtual void postTask(std::unique_ptr<QueuedTask> task) = 0;

    // Schedules a task to execute a specified number of milliseconds from when
    // the call is made. The precision should be considered as "best effort"
    // and in some cases, such as on Windows when all high precision timers have
    // been used up, can be off by as much as 15 millseconds.
    virtual void postDelayedTask(std::unique_ptr<QueuedTask> task, uint32_t milliseconds) = 0;

    // Returns the task queue that is running the current thread.
    // Returns nullptr if this thread is not associated with any task queue.
    static TaskQueueBase* current();
    bool isCurrent() const { return current() == this; }

    virtual const std::string& name() const = 0;

protected:
    class CurrentTaskQueueSetter {
    public:
        explicit CurrentTaskQueueSetter(TaskQueueBase* taskQueue);
        CurrentTaskQueueSetter(const CurrentTaskQueueSetter&) = delete;
        CurrentTaskQueueSetter& operator=(const CurrentTaskQueueSetter&) = delete;
        ~CurrentTaskQueueSetter();

    private:
        TaskQueueBase* const _previous;
    };

    // Users of the TaskQueue should call Delete instead of directly deleting
    // this object.
    virtual ~TaskQueueBase() = default;
};

struct TaskQueueDeleter {
    void operator()(TaskQueueBase* taskQueue) const { taskQueue->deleteThis(); }
};

}  // namespace webrtc
