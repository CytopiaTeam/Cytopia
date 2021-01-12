#include "LOG.hxx"
#include "MessageQueue.hxx"
#include <atomic>
#include <thread>
#include <future>
#include <catch.hpp>

/* Message Queues are useful to synchronize events
 * across threads. It's one of the easiest and most
 * efficient way to share memory safely and concurrently
 */

/* First, we define an Event type and Event Queue */
using Event = int;
/* Note: You should always have a TERMINATE event.
 * In our case, we will use 0 for TERMINATE.
 */
using EventQueue = MessageQueue<Event>;
using namespace std::chrono_literals;
template <typename Type> using Deque = std::deque<Type>;

/* Next, we need a shared context for our threads */
struct Context
{
  EventQueue ConsumerMQ;
  EventQueue Producer1MQ;
  EventQueue Producer2MQ;
  /* Usually, each thread has its own EventQueue.
   * We would also add all shared data here.
   * Shared data is data that should be accessible
   * by multiple threads. 
   */
  inline void terminate() noexcept
  {
    ConsumerMQ.push(0);
    Producer1MQ.push(0);
    Producer2MQ.push(0);
  }
};

/* Now we define the Event producer threads */
void Producer(Context &application, EventQueue &myMQ)
{
  Deque<Event> MyEvents{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  while (not MyEvents.empty())
  {
    if (myMQ.peek())
    {
      break;
    }
    Event event = MyEvents.front();
    MyEvents.pop_front();
    application.ConsumerMQ.push(std::move(event));
    std::this_thread::sleep_for(50ms);
  }
}

using TestResult = Deque<int>;

/* Now we define the Event consumer thread */
void Consumer(Context &application, TestResult &result)
{
  while (true)
  {
    EventQueue::Enumerable events = application.ConsumerMQ.getEnumerable();
    CHECK(events.size() > 0);
    result.insert(result.end(), events.begin(), events.end());
    if (result.back() == 0)
    {
      result.pop_back();
      break;
    }
  }
}

using Future = std::future<void>;
using FutureStatus = std::future_status;
using Launch = std::launch;
SCENARIO("Message Queues are reliable", "[util][thread]")
{
  GIVEN("I have two producer threads and a consumer thread")
  {
    Context context;
    TestResult result;
    /* Here we could use threads, but we are using Futures to prevent a deadlock */
    Future consumer = std::async(Launch::async, &Consumer, std::ref(context), std::ref(result));
    Future producer1 = std::async(Launch::async, &Producer, std::ref(context), std::ref(context.Producer1MQ));
    Future producer2 = std::async(Launch::async, &Producer, std::ref(context), std::ref(context.Producer2MQ));
    WHEN("My producers send all their messages to the consumer")
    {
      /* Our tests should not wait more than 5s for the MessageQueue */
      auto PingDelta = 50ms;
      int maxPings = 100;

      /* Wait for Producer 1 to finish */
      FutureStatus status = FutureStatus::timeout;
      while (status == FutureStatus::timeout and maxPings-- > 0)
      {
        if (maxPings % 10 == 0)
          LOG(LOG_INFO) << "Pinging producer 1...\n";
        status = producer1.wait_for(PingDelta);
      }
      if (status != FutureStatus::ready)
      {
        context.terminate();
        CHECK(status == FutureStatus::ready);
      }

      /* Wait for Producer 2 to finish */
      status = FutureStatus::timeout;
      while (status == FutureStatus::timeout and maxPings-- > 0)
      {
        if (maxPings % 10 == 0)
          LOG(LOG_INFO) << "Pinging producer 2...\n";
        status = producer2.wait_for(PingDelta);
      }
      if (status != FutureStatus::ready)
      {
        context.terminate();
        CHECK(status == FutureStatus::ready);
      }

      /* Wait for Consumer to finish */
      context.terminate();
      status = FutureStatus::timeout;
      while (status == FutureStatus::timeout and maxPings-- > 0)
      {
        if (maxPings % 10 == 0)
          LOG(LOG_INFO) << "Pinging consumer...\n";
        status = consumer.wait_for(PingDelta);
      }
      CHECK(status == FutureStatus::ready);

      THEN("My consumer receives all their messages")
      {
        /* We can really receive these events in any possible order */
        int x1 = 1, x2 = 1;
        for (int event : result)
        {
          if (event == x1)
          {
            x1++;
          }
          else
          {
            CHECK(event == x2++);
          }
        }
      }
    }
  }
}
