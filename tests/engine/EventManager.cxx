#include <catch.hpp>
#include "../../src/engine/EventManager.hxx"

using string = std::string;

TEST_CASE("I selected POWERLINES, update placement method to the LINE tool", "[engine][eventmanager]")
{
  const auto topMostActiveLayer = POWERLINES;
  const auto new_placement_mode = detail::updatePlacementMode(topMostActiveLayer);
  CHECK(new_placement_mode == PlacementMode::LINE);
}

TEST_CASE("I selected BUILDINGS, update placement method to the SINGLE tool", "[engine][eventmanager]")
{
  const auto topMostActiveLayer = BUILDINGS;
  const auto new_placement_mode = detail::updatePlacementMode(topMostActiveLayer);
  CHECK(new_placement_mode == PlacementMode::SINGLE);
}

TEST_CASE("I selected ZONE, update placement method to the RECTANGLE tool", "[engine][eventmanager]")
{
  const auto topMostActiveLayer = ZONE;
  const auto new_placement_mode = detail::updatePlacementMode(topMostActiveLayer);
  CHECK(new_placement_mode == PlacementMode::RECTANGLE);
}

TEST_CASE("I selected TERRAIN, do not update placement tool, this is an invalid entry", "[engine][eventmanager]")
{
  const auto topMostActiveLayer = TERRAIN;
  const auto new_placement_mode = detail::updatePlacementMode(topMostActiveLayer);
  CHECK(new_placement_mode == std::nullopt);
}
