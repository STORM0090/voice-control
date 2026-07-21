#ifndef VOICECONTROL_THREAD_SAFE_QUEUE_H
#define VOICECONTROL_THREAD_SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;

// Queue shared by two threads: one pushes, the other waits and pops.
template <class T>
class ThreadSafeQueue {
public:
    void push(T item) {
        {
            lock_guard<mutex> lock(m);
            data.push(move(item));
        }
        cv.notify_one();
    }

    T waitAndPop() {
        unique_lock<mutex> lock(m);
        cv.wait(lock, [this] { return !data.empty(); });
        T item = move(data.front());
        data.pop();
        return item;
    }

    void clear() {
        lock_guard<mutex> lock(m);
        queue<T> empty;
        data.swap(empty);
    }

private:
    queue<T> data;
    mutex m;
    condition_variable cv;
};

#endif
