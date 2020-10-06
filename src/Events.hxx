#ifndef EVENTS_HXX_
#define EVENTS_HXX_

#include <string>

#include "betterEnums.hxx"
#include "util/Meta.hxx"

using std::string;

/**
 * @brief Events to be handled by the Game Loop
 */
using GameEvents = TypeList<
#ifdef USE_AUDIO
    /* All AudioEvents */
    struct AudioTriggerEvent, struct AudioTrigger3DEvent, struct AudioTriggerReverbEvent, struct AudioTriggerReverb3DEvent,
    struct AudioTriggerEchoEvent, struct AudioTriggerEcho3DEvent, struct AudioPlayEvent, struct AudioPlay3DEvent,
    struct AudioPlayReverbEvent, struct AudioPlayReverb3DEvent, struct AudioPlayEchoEvent, struct AudioPlayEcho3DEvent,
    struct AudioMusicVolumeChangeEvent, struct AudioSoundVolumeChangeEvent, struct AudioSetMutedEvent, struct AudioStopEvent,
    struct AudioPruneEvent,
#endif // USE_AUDIO
    struct TerminateEvent,
    struct ActivitySwitchEvent
    /* Add Game Events here */
    >;

#ifdef USE_AUDIO
#include "events/AudioEvents.hxx"
#endif // USE_AUDIO
#include "events/UIEvents.hxx"

/**
 * @brief Event triggered when the game must terminate
 */
struct TerminateEvent
{
};

#endif
