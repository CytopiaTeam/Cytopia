#include "Randomizer.hxx"

thread_local RNG Randomizer::generator{std::random_device{}()};
