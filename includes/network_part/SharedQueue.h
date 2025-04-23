/**
 * @file SharedQueue.h
 * @brief Thread-safe queue implementation.
 * @details Provides synchronized access to a queue from multiple threads.
 */
#ifndef SHAREDQUEUE_H
#define SHAREDQUEUE_H

#include <condition_variable>
#include <deque>
#include <mutex>


 /**
  * @class SharedQueue
  * @brief Thread-safe queue template class.
  * @tparam T Type of queue elements.
  */
template <typename T> class SharedQueue
{
public:
    SharedQueue() = default;
    ~SharedQueue() = default;

    /**
     * @brief Returns front element without removal.
     * @return Reference to front element.
     */
    T& front();

    /**
     * @brief Removes and returns front element.
     * @return Reference to front element.
     */
    T& pop_front();

    /**
     * @brief Adds element to back (copy version).
     * @param item Element to add.
     */
    void push_back(const T& item);
    /**
     * @brief Adds element to back (move version).
     * @param item Element to add.
     */
    void push_back(T&& item);
    /**
     * @brief Clears the queue.
     */
    void clear();

    /**
     * @brief Gets current queue size.
     * @return Number of elements.
     */
    int size();
    /**
     * @brief Checks if queue is empty.
     * @return true if empty.
     */
    bool empty();

private:
    std::deque<T> queue_; /**< Underlying queue container. */
    std::mutex mutex_; /**< Queue access mutex. */
    std::condition_variable cond_; /**< Queue access condition variable. */
};


// Template method implementations...
template <typename T> T& SharedQueue<T>::front()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty()) { cond_.wait(mlock); }
    return queue_.front();
}

template <typename T> T& SharedQueue<T>::pop_front()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    while (queue_.empty()) { cond_.wait(mlock); }
    T& front = queue_.front();
    queue_.pop_front();
    return front;
}

template <typename T> void SharedQueue<T>::push_back(const T& item)
{
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push_back(item);
    mlock.unlock();     // unlock before notification to minimize mutex con
    cond_.notify_one(); // notify one waiting thread
}

template <typename T> void SharedQueue<T>::push_back(T&& item)
{
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push_back(std::move(item));
    mlock.unlock();     // unlock before notification to minimize mutex con
    cond_.notify_one(); // notify one waiting thread
}

template <typename T> int SharedQueue<T>::size()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    int size = queue_.size();
    mlock.unlock();
    return size;
}

template <typename T> bool SharedQueue<T>::empty()
{
    return size() == 0;
}

template <typename T> void SharedQueue<T>::clear()
{
    std::unique_lock<std::mutex> mlock(mutex_);
    std::deque<T>().swap(queue_);
    mlock.unlock();
    cond_.notify_one();
}

#endif // SHAREDQUEUE_H
