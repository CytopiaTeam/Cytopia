#ifndef EVENTS_HXX_
#define EVENTS_HXX_

#include "util/Meta.hxx"
#include "util/TransitiveModel.hxx"

using UIEvents = TypeList<struct TerminateEvent /* Add UI Events here */ >;
using GameEvents = TypeList<struct TerminateEvent /* Add Game Events here */>;

/**
 * @brief Event triggered when the game must terminate
 */
struct TerminateEvent { };

template <typename UITargetType, typename UpdateEventType>
struct UIChangeEvent 
{
  UITargetType target;
  UpdateEventType data;
};

template <typename TransitiveType>
using TransitiveStateChange = UIChangeEvent<ObserverWPtr<typename TransitiveType::Transition>, typename TransitiveType::Transition>;



#endif
