#ifndef THREAD_POOL_HXX
#define THREAD_POOL_HXX

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#include "Singleton.hxx"

class ThreadWorkers {
public:
  inline void initialize(size_t threads)
  {
    threads = std::max(initial_threads_num, threads);
    for (size_t i = 0; i < threads; ++i)
    {
      workers.emplace_back([this] {
         for (;;)
         {
           std::function<void()> task;

           {
             std::unique_lock lock(this->queue_mutex);
             this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });

             if (this->stop && this->tasks.empty())
               return;

             task = std::move(this->tasks.front());
             this->tasks.pop();
           }

           task();
         }
      });
    }
  }

  // add new work item to the pool
  template<class F, class... Args, class R = typename std::result_of_t<F(Args...)>>
  std::future<R> execute(F &&f, Args&&... args)
  {
    auto task = std::make_shared<std::packaged_task<R()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<R> res = task->get_future();

    {
      std::unique_lock lock(queue_mutex);

      // don't allow enqueueing after stopping the pool
      if (stop)
        throw std::logic_error("enqueue on stopped ThreadPool");

      tasks.emplace([task] () { (*task)(); });
    }

    condition.notify_one();
    return res;
  }

  // the destructor joins all threads
  void shutdown()
  {
    {
      std::unique_lock lock(queue_mutex);
      stop = true;
    }

    condition.notify_all();
    for (std::thread &worker : workers) {
      worker.join();
    }
  }

private:
  // need to keep track of threads so we can join them
  static constexpr size_t initial_threads_num = 4;
  std::vector< std::thread > workers;

  // the task queue
  std::queue< std::function<void()> > tasks;

  // synchronization
  std::mutex queue_mutex;
  std::condition_variable condition;
  bool stop = false;
};

#endif // THREAD_POOL_HXX
