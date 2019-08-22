#ifndef RANDOMIZER_HXX_
#define RANDOMIZER_HXX_

#include <random>
#include <iterator>

#include "../GameService.hxx"
#include "../engine/basics/LOG.hxx"


using RNG = std::mt19937;
class Randomizer : public GameService
{
  thread_local static RNG generator;

  template <typename IntType>
  using UniformInt = std::uniform_int_distribution<IntType>;

public:
  Randomizer(GameService::ServiceTuple& context) : 
    GameService(context)
  { }

  /**
   * @returns a random iterator between [begin, end)
   * @threadsafe
   * @param begin the starting iterator
   * @param end the pointer following the last value
   */
  template<typename Iterator>
  Iterator choose(Iterator begin, Iterator end)
  {
    using DiffType = typename std::iterator_traits<Iterator>::difference_type;
    DiffType upper {std::distance(begin, end) - 1};
    UniformInt<DiffType> distn{DiffType{0}, upper};
    std::advance(begin, distn(generator));
    return begin;
  }
};

#endif
