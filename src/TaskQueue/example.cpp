// #include <iostream>
// #include "task_queue.h"
// #include <memory>
// #include <thread>
// #include "event.h"
// #include "task_queue_manager.h"
//
// using namespace std;
//
// int main()
// {
//     cout << "Hello World!" << endl;
//
//     TQMgr->create({"worker1", "worker2", "worker3"});
//
//     TaskQueue::Event ev;
//
//     const int N = 12;
//     std::thread threads[N];
//     for (int n = 0; n < N; ++n) {
//         threads[n] = std::thread([&ev]{
//             for (int i = 0; i < 10000; ++i) {
//                 TQ("worker1")->postTask([&ev, i](){
//                     cout << "exec task in 'worker1' queue: " << ", i = " << i << endl;
//                 });
//
//                 TQ("worker1")->postDelayedTask([&ev, i](){
//                     cout << "exec delayed task in 'worker1' queue: " << ", i = " << i << endl;
//                 }, 1000);
//
//                 TQ("worker2")->postDelayedTask([&ev, i](){
//                     cout << "exec delayed task in 'worker2' queue: " << ", i = " << i << endl;
//                     if (i == 9999) {
//                         ev.set();
//                     }
//                 }, 1000);
//             }
//         });
//     }
//
//     ev.wait(-1);
//     for (int n = 0; n < N; ++n) {
//         threads[n].join();
//     }
//
//     return 0;
// }
