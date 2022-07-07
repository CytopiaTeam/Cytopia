#ifndef THREAD_SCENEUPDATE_HXX
#define THREAD_SCENEUPDATE_HXX

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

struct BaseScene;

class ThreadSceneUpdate : public Singleton<ThreadSceneUpdate> {
public:
  friend class Singleton<ThreadSceneUpdate>;

  inline void nextFrame() { m_frameLocker.fetch_add(1); }
  inline bool isReady() const { return m_frameLocker.load() == 0; }

  void initialize();

  // the destructor joins all threads
  void shutdown()
  {
    m_stop = true;
    m_worker.join();
  }

  void setupScene(BaseScene *scene)
  {
    m_scene = scene;
  }
private:
  void execute();

  ThreadSceneUpdate() = default;
  ~ThreadSceneUpdate() = default;

  BaseScene *m_scene = nullptr;
  std::thread m_worker;
  std::atomic<int> m_frameLocker;
  bool m_stop = false;
};

#endif // THREAD_SCENEUPDATE_HXX
