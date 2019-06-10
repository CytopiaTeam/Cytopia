#ifndef TRANSITIVE_MODEL_HXX_
#define TRANSITIVE_MODEL_HXX

#include <type_traits>

#include "Observer.hxx"

/**
 * @struct Transition
 * @brief Represents the change between one state to another
 * @
 */
template <typename State, State::Operation Type>
struct Transition
{
  static_assert(std::is_enum<State::Operation>::value, "State::Operations must be an enum-type");
  State::Operations type;
  
};

/**
 * @class TransitiveModel
 * @brief A subject model that is tightly coupled with its operations.
 *        Specifically, for each possible transition in that Model 
 *        there exists a Transition<> type to represent it.
 *        Observers to this model must update(...) with those
 *        Transition objects
 */
template <typename State>
class TransitiveModel : protected State, public Subject< Transition<State> > { };

#endif
