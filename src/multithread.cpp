/**
 * @file src/multithread.cpp
 * 
 * @brief Provides multithreading utilities for job queue management.
 * 
 * @author Samii Shabuse <sus24@drexel.edu>
 * @date November 7, 2025
 * 
 * @section Overview
 *
 * This file contains a simple job queue implementation to facilitate
 * multithreaded processing. 
 */

#include <thread>
#include <condition_variable>
#include <queue>

/**
 * @brief A thread-safe job queue for managing tasks.
 * 
 * @details This class provides a simple thread-safe job queue that allows
 *          multiple producer and consumer threads to push and pop jobs.
 *          It uses a mutex and condition variable to ensure safe access
 *          to the underlying queue.
 */
struct JobQueue {
    std::queue<int> q;
    bool closed = false;
    std::mutex m;
    std::condition_variable cv;

    /**
     * @brief Pushes a job onto the queue.
     * 
     * @param v The job to be added to the queue.
     * 
     * @return void
     * 
     * @details This function locks the mutex, adds the job to the queue,
     *          and notifies one waiting thread.
     */
    void push(int v){
        std::unique_lock<std::mutex> lk(m);
        q.push(v);
        cv.notify_one();
    }

    /**
     * @brief Pops a job from the queue.
     * 
     * @param v Reference to store the popped job.
     * 
     * @return true if a job was successfully popped, false if the queue is closed and empty.
     * 
     * @details This function locks the mutex and waits until a job is available
     *          or the queue is closed. If a job is available, it is removed from
     *          the queue and stored in the provided reference. If the queue is
     *          closed and empty, the function returns false.
     */
    bool pop(int &v){
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&]{ return closed || !q.empty(); });
        if(q.empty()) return false;
        v = q.front(); q.pop();
        return true;
    }

    /**
     * @brief Closes the job queue.
     * 
     * @return void
     * 
     * @details This function locks the mutex, sets the closed flag to true,
     *          and notifies all waiting threads.
     */
    void close(){
        std::unique_lock<std::mutex> lk(m);
        closed = true;
        cv.notify_all();
    }
};
