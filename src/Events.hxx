#ifndef EVENTS_HXX_
#define EVENTS_HXX_

#include <string>
#include <betterEnums.hxx>

#include "util/Meta.hxx"

using std::string;

/**
 * @brief Events to be handled by the UI Loop
 */
using UIEvents = TypeList<
  struct TerminateEvent,
  struct ActivitySwitchEvent,
  struct WindowResizeEvent,
  struct WindowRedrawEvent,
  struct UIChangeEvent
  /* Add UI Events here */
  >;

/**
 * @brief Events to be handled by the Game Loop
 */
using GameEvents = TypeList<
  struct TerminateEvent,
#ifdef USE_AUDIO
  /* All AudioEvents */
  struct AudioTriggerEvent, struct AudioTrigger3DEvent, struct AudioPlayEvent, struct AudioPlay3DEvent,
  struct AudioMusicVolumeChangeEvent, struct AudioSoundVolumeChangeEvent, struct AudioSetMutedEvent,
  struct AudioStopEvent, struct AudioPruneEvent,
#endif // USE_AUDIO
  /* All MouseEvents */
  struct MousePositionEvent,
  struct ClickEvent,
  struct ScrollEvent,
  struct ActivitySwitchEvent 
  /* Add Game Events here */
  >;

/**
 * @brief Event triggered when the game must terminate
 */
struct TerminateEvent
{
};

#ifdef USE_AUDIO
#include "events/AudioEvents.hxx"
#endif // USE_AUDIO
#include "events/UIEvents.hxx"
#include "events/MouseEvents.hxx"

#endif

