#include "GameService.hxx"

GameService::GameService(ServiceTuple &services) : m_Services(services) {}

GameService::ServiceTuple & GameService::getGameContext() { return m_Services; }
