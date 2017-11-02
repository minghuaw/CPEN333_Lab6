#ifndef LAB6_CIRCULARORDERQUEUE_H
#define LAB6_CIRCULARORDERQUEUE_H

#include "OrderQueue.h"
#include <cpen333/thread/semaphore.h>
#include <mutex>

#define CIRCULAR_BUFF_SIZE 8

/**
 * Queue implementation using a circular buffer
 * (i.e. a fixed-size queue)
 */
class CircularOrderQueue : public virtual OrderQueue {
    Order buff_[CIRCULAR_BUFF_SIZE];
    cpen333::thread::semaphore producer_;
    cpen333::thread::semaphore consumer_;
    std::mutex pmutex_;
    std::mutex cmutex_;
    size_t pidx_;
    size_t cidx_;


public:
    /**
     * Creates a queue with provided circular buffer size
     * @param buffsize size of circular buffer
     */
    CircularOrderQueue() :
            buff_(),
            producer_(CIRCULAR_BUFF_SIZE), consumer_(0),
            pmutex_(), cmutex_(), pidx_(0), cidx_(0) {}

    void add(const Order &order) {

        //==================================================
        // TODO: Safely add item to "queue"
        //    - wait for empty slot
        //    - safely acquire and increment producer index
        //    - fill slot
        //    - notify others of item availability
        //==================================================
        // wait for producer_ semaphore resource
        producer_.wait();

        // safely acquire and increment producder index
        int pidx;
        {
            std::lock_guard<decltype(pmutex_)> pmutexGuard(pmutex_);
            pidx = pidx_;
            // update producer index
            pidx_ = (pidx_ + 1) % CIRCULAR_BUFF_SIZE;
        }
        buff_[pidx] = order;

        // notify consumer_ semaphore
        consumer_.notify();
    }

    Order get() {

        //==================================================
        // TODO: Safely remove item from "queue"
        //    - wait for next filled slot
        //    - safely acquire and increment consumer index
        //    - remove item from slot
        //    - notify others of slot availability
        //==================================================
        // wait for consumer_ semaphore
        consumer_.wait();

        // safely acquireand increment consumer index
        int cidx;
        {
            std::lock_guard<decltype(cmutex_)> cmutexGuard(cmutex_);
            cidx = cidx_;
            // update consumer index
            cidx_ = (cidx_ + 1) % CIRCULAR_BUFF_SIZE;
        }

        Order out = buff_[cidx];
        // notify producer_ semaphore
        producer_.notify();

        return out;
    }

};

#endif //LAB6_CIRCULARORDERQUEUE_H
