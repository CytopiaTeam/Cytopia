#ifndef RANDOMIZER_HXX_
#define RANDOMIZER_HXX_

#include <random>
#include <iterator>
#include <algorithm>
#include "../util/Singleton.hxx"

class Randomizer : public Singleton<Randomizer>
{
  static thread_local std::mt19937 generator;

  template <typename IntType> using UniformIntDist = std::uniform_int_distribution<IntType>;

public:
  Randomizer() = default;

  template <typename Iterator> void shuffle(Iterator begin, Iterator end) { std::shuffle(begin, end, generator); }

  /**
   * @brief Pick random item from container.
   * 
   * @threadsafe
   * 
   * @param begin the starting iterator
   * @param end the pointer following the last value
   * @returns a random iterator between [begin, end)
   */
  template <typename Iterator> Iterator choose(Iterator begin, Iterator end)
  {
    using DiffType = typename std::iterator_traits<Iterator>::difference_type;
    DiffType upper{std::distance(begin, end) - 1};
    UniformIntDist<DiffType> distn{DiffType{0}, upper};
    std::advance(begin, distn(generator));
    return begin;
  }
};

#endif
