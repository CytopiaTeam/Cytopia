#include "ThreadSceneUpdate.hxx"
#include "../scenes/Scenes.hxx"

void ThreadSceneUpdate::initialize()
{
  m_worker = std::thread([this] {
    for (;;)
    {
      if (this->m_stop)
        return;

      if (m_frameLocker.load() == 0)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        continue;
      }

      this->execute();
      m_frameLocker.fetch_sub(1);
    }
  });
}

void ThreadSceneUpdate::execute() 
{
  if (m_scene)
    m_scene->onUpdate();
}