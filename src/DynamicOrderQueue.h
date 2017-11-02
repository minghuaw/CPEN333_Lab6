#ifndef LAB6_DYNAMICORDERQUEUE_H
#define LAB6_DYNAMICORDERQUEUE_H

#include "OrderQueue.h"
#include <deque>
#include <condition_variable>
#include <mutex>

/**
 * Dynamically-sized Queue Implementation
 *
 * Does not block when adding items
 */
class DynamicOrderQueue : public virtual OrderQueue {
    std::deque<Order> buff_;
    std::mutex mutex_;
    std::condition_variable cv_;

    bool ready;

public:
    /**
     * Creates the dynamic queue
     */
    DynamicOrderQueue() :
            buff_(), mutex_(), cv_() {
        ready = false;
    }

    void add(const Order &order) {

        //==================================================
        // TODO: Safely add item to "queue"
        //    - safely add to end of internal queue
        //    - notify others of item availability
        //==================================================
        {
            std::unique_lock<decltype(mutex_)> lock(mutex_);
            buff_.push_back(order);
            ready = true;
        }

        // notify all other waiting threads
//        cv_.notify_all();
        cv_.notify_all();
    }

    Order get() {

        //==================================================
        // TODO: Safely remove item from "queue"
        //    - wait until internal queue is non-empty
        //    - safely acquire item from internal queue
        //==================================================

        {
            std::unique_lock<decltype(mutex_)> lock(mutex_);
            cv_.wait(lock, [&] {return ready;});
//            cv_.wait(lock);
        }
        // get first item in queue
        Order out = buff_.front();
        buff_.pop_front();

        if(buff_.size()==0)
            ready = false;

        return out;
    }
};

#endif //LAB6_DYNAMICORDERQUEUE_H
