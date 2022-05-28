#include "GamePlay.hxx"

void GamePlay::resetManagers()
{
  m_PowerManager.rebuildGrids();
  m_ZoneManager.reset();
}