#include "Randomizer.hxx"

thread_local std::mt19937 Randomizer::generator{std::random_device{}()};
