#include <iostream>
#include <atomic>
#include <chrono>
#include <functional>
#include <thread>
#include <future>

using std::function;
using std::chrono::system_clock;
using std::chrono::duration;
using std::chrono::milliseconds;
using std::chrono::time_point;
using std::future;

template<typename T>
class DelayedTask
{
public:
    DelayedTask() : _cancel(false) {}

    void setTask(milliseconds delay, function<void()> timeoutTask, function<T()> cancelTask = nullptr)
    {
        _cancel = false;
        auto task = [=] {
            time_point<system_clock> start = system_clock::now();
            while (!_cancel)
            {
                milliseconds interval = std::chrono::duration_cast<milliseconds>(system_clock::now() - start);
                if (interval < delay)
                {
                    continue;
                }
                return timeoutTask();
            }
            if (cancelTask != nullptr)
            {
                return cancelTask();
            }
            cout << "no cancel task...\n";
        };

        _fut = std::async(task);
    }

    void cancel()
    {
        _cancel = true;
        _fut.get();
    }
private:
    volatile std::atomic<bool> _cancel;
    future<T> _fut;
};

template<typename T>
void setEvent(DelayedTask<T>& taskAllocator)
{
    function<void()> timeoutEvent = [] {
        cout << "trigger timeout event...\n";
    };

    function<void()> cancelEvent = [] {
        cout << "event has been cancel\n";
    };

    taskAllocator.setTask(milliseconds(5000), timeoutEvent, cancelEvent);
}

int main(int argc, char* argv[])
{
    DelayedTask<void> taskAllocator;
    setEvent(taskAllocator);
    time_point<system_clock> start = system_clock::now();
    while (1)
    {
        milliseconds interval = std::chrono::duration_cast<milliseconds>(system_clock::now() - start);
        if (interval < milliseconds(2500))
        {
            cout << "wait task cancel...\n";
            std::this_thread::sleep_for(milliseconds(500));
            continue;
        }
        break;
    }
    taskAllocator.cancel();
    setEvent(taskAllocator);
    while(1);
    return 0;
}