#include "GameFixture.hxx"

GameFixture::GameFixture()
{
  m_Game.initialize();
}

GameFixture::~GameFixture()
{
  m_Game.shutdown();
}
