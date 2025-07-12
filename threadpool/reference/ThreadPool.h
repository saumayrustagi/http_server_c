// ThreadPool.h
#pragma once

#include <vector>         // For std::vector
#include <queue>          // For std::queue
#include <thread>         // For std::thread
#include <mutex>          // For std::mutex, std::unique_lock
#include <condition_variable> // For std::condition_variable
#include <functional>     // For std::function
#include <stdexcept>      // For std::runtime_error

// The ThreadPool class definition
class ThreadPool {
public:
    /**
     * @brief Constructor for the ThreadPool.
     * @param num_threads The number of worker threads to create in the pool.
     * @throws std::runtime_error if thread creation fails.
     */
    explicit ThreadPool(size_t num_threads);

    /**
     * @brief Destructor for the ThreadPool.
     * Stops all worker threads and joins them.
     */
    ~ThreadPool();

    /**
     * @brief Enqueues a task to be executed by a worker thread.
     * @tparam F The type of the callable (function, lambda, etc.).
     * @tparam Args The types of the arguments for the callable.
     * @param f The callable object (e.g., a lambda, function pointer).
     * @param args The arguments to pass to the callable.
     * @throws std::runtime_error if trying to enqueue a task after the pool has stopped.
     */
    template<class F, class... Args>
    void enqueue(F&& f, Args&&... args);

private:
    // Vector to store all worker threads
    std::vector<std::thread> workers;

    // Queue to hold tasks (std::function<void()> allows any callable)
    std::queue<std::function<void()>> tasks;

    // Mutex to protect access to the tasks queue
    std::mutex queue_mutex;

    // Condition variable to notify worker threads about new tasks or shutdown
    std::condition_variable condition;

    // Flag to signal threads to stop their work and exit
    bool stop;

    /**
     * @brief The main loop executed by each worker thread.
     * This function continuously fetches and executes tasks from the queue.
     */
    void worker_loop();
};

// ThreadPool.cpp
// (This would typically be in a separate .cpp file, but for immersive blocks, it's combined)

/**
 * @brief Constructor for the ThreadPool.
 * Initializes the pool, sets up synchronization primitives, and launches worker threads.
 * @param num_threads The number of worker threads to create.
 */
ThreadPool::ThreadPool(size_t num_threads) : stop(false) {
    if (num_threads == 0) {
        throw std::invalid_argument("Number of threads cannot be zero.");
    }
    // Create and launch worker threads
    for (size_t i = 0; i < num_threads; ++i) {
        // Emplace_back constructs the thread in-place
        // std::bind is used to create a callable that calls worker_loop on 'this' object
        workers.emplace_back(&ThreadPool::worker_loop, this);
    }
}

/**
 * @brief Destructor for the ThreadPool.
 * Sets the stop flag, notifies all workers, and joins them to ensure graceful shutdown.
 */
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true; // Signal all workers to stop
    }
    condition.notify_all(); // Wake up all waiting workers

    // Join all worker threads to ensure they have finished
    for (std::thread& worker : workers) {
        if (worker.joinable()) { // Check if the thread is joinable (not already joined)
            worker.join();
        }
    }
}

/**
 * @brief The main loop executed by each worker thread.
 * This function continuously fetches and executes tasks from the queue.
 */
void ThreadPool::worker_loop() {
    for (;;) { // Infinite loop for worker
        std::function<void()> task; // Placeholder for the task to execute
        {
            // Acquire a unique lock on the mutex
            std::unique_lock<std::mutex> lock(queue_mutex);

            // Wait until there are tasks in the queue or the stop flag is set
            // The lambda predicate prevents spurious wakeups
            condition.wait(lock, [this] {
                return !tasks.empty() || stop;
            });

            // If stop is true AND there are no more tasks, exit the worker loop
            if (stop && tasks.empty()) {
                return; // Thread exits
            }

            // Dequeue a task
            task = std::move(tasks.front()); // Use std::move for efficiency
            tasks.pop();
        } // Lock is automatically released here as unique_lock goes out of scope

        // Execute the task outside the lock to allow other threads to access the queue
        task();
    }
}

/**
 * @brief Enqueues a task to be executed by a worker thread.
 * @tparam F The type of the callable (function, lambda, etc.).
 * @tparam Args The types of the arguments for the callable.
 * @param f The callable object (e.g., a lambda, function pointer).
 * @param args The arguments to pass to the callable.
 * @throws std::runtime_error if trying to enqueue a task after the pool has stopped.
 */
template<class F, class... Args>
void ThreadPool::enqueue(F&& f, Args&&... args) {
    // Create a packaged task (std::function) from the callable and its arguments
    // std::bind is used to bind the arguments to the function
    auto task_lambda = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        tasks.emplace(std::move(task_lambda)); // Add the task to the queue
    }
    condition.notify_one(); // Notify one waiting worker that a new task is available
}

// main.cpp
// (This would typically be in a separate .cpp file for testing)

#include <iostream>
#include <string>
#include <chrono> // For std::chrono::seconds, milliseconds
#include <thread> // For std::this_thread::sleep_for

// Include the ThreadPool header
// #include "ThreadPool.h" // In a real project, you'd include this

// Example task functions
void print_message(const std::string& msg, int delay_ms) {
    std::cout << "Task: " << msg << " (Thread ID: " << std::this_thread::get_id() << ")" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    std::cout << "Task: " << msg << " finished." << std::endl;
}

void calculate_sum(int a, int b) {
    std::cout << "Calculating sum of " << a << " + " << b << " (Thread ID: " << std::this_thread::get_id() << ")" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    int sum = a + b;
    std::cout << "Sum: " << sum << " (Thread ID: " << std::this_thread::get_id() << ") finished." << std::endl;
}

int main() {
    std::cout << "Initializing Thread Pool with 4 threads..." << std::endl;
    ThreadPool pool(4); // Create a thread pool with 4 worker threads

    std::cout << "Enqueuing tasks..." << std::endl;

    // Enqueue some tasks using lambdas
    for (int i = 0; i < 8; ++i) {
        pool.enqueue([i]() {
            std::cout << "Lambda Task " << i << " (Thread ID: " << std::this_thread::get_id() << ") executing..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << "Lambda Task " << i << " finished." << std::endl;
        });
    }

    // Enqueue tasks using bound functions
    pool.enqueue(print_message, "Hello from task 1", 200);
    pool.enqueue(calculate_sum, 10, 20);
    pool.enqueue(print_message, "Another task", 150);
    pool.enqueue(calculate_sum, 5, 8);
    pool.enqueue(print_message, "Last task", 300);

    std::cout << "All tasks enqueued. Main thread continuing..." << std::endl;

    // The main thread can do other work while tasks are processed
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "Main thread finished. Thread pool will now be destroyed (tasks will complete)." << std::endl;
    // The pool's destructor will be called automatically when 'pool' goes out of scope,
    // which will stop all worker threads and join them.

    return 0;
}
