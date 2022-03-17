#ifndef RANDOM_GENERATOR_HXX_
#define RANDOM_GENERATOR_HXX_

#include "Singleton.hxx"
#include <random>
#include <algorithm>


class RandomEngine : public Singleton<RandomEngine>
{
public:
  RandomEngine() = default;
  ~RandomEngine() = default;

  std::default_random_engine getRandomEngine() { return rng; };

private:  
  std::random_device rd = std::random_device{};; 
  std::default_random_engine rng = std::default_random_engine{rd()};;
};

#endif