#pragma once

#include "mutex"
#include "condition_variable"

namespace TaskQueue
{
    class Event
    {
    public:
        static const int kForever = -1;
        
        Event();
        Event(bool manual_reset, bool initially_signaled);
        ~Event();

        void set();
        void reset();

        bool wait(int give_up_after_ms, int warn_after_ms);

        bool wait(int give_up_after_ms){
            return wait(give_up_after_ms, give_up_after_ms == kForever ? 3000:kForever);
        }

        Event(const Event&) = delete;
        Event& operator = (const Event&) = delete;

    private:
        std::mutex event_mutex;
        std::condition_variable event_condition;
        const bool is_manual_reset;
        bool event_status;    

    }

    

}