#include <thread>
#include <condition_variable>
#include <queue>

struct JobQueue {
    std::queue<int> q;
    bool closed = false;
    std::mutex m;
    std::condition_variable cv;

    void push(int v){
        std::unique_lock<std::mutex> lk(m);
        q.push(v);
        cv.notify_one();
    }
    bool pop(int &v){
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&]{ return closed || !q.empty(); });
        if(q.empty()) return false;
        v = q.front(); q.pop();
        return true;
    }
    void close(){
        std::unique_lock<std::mutex> lk(m);
        closed = true;
        cv.notify_all();
    }
};
