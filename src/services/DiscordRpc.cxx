#include "DiscordRpc.hxx"
#include "LOG.hxx"

#define USE_DISCORD_RPC
#ifdef USE_DISCORD_RPC
#include "discord_game_sdk.h"

static IDiscordCore *core = nullptr;
static IDiscordCoreEvents events = nullptr;
static IDiscordActivityManager *activityManager = nullptr;
#endif // USE_DISCORD_RPC

void DiscordRpc::init()
{
#ifdef USE_DISCORD_RPC
  DiscordCreateParams discord_params;
  discord_params.client_id = 980219396125523978;
  discord_params.flags = DiscordCreateFlags_Default;
  discord_params.events = &events;
  discord_params.event_data = &core;

  DiscordCreate(DISCORD_VERSION, &discord_params, &core);
#endif // USE_DISCORD_RPC
}

void DiscordRpc::updatePresence(const char *map)
{
#ifdef USE_DISCORD_RPC
  if (!core)
    return;

  DiscordActivity activity{};
  snprintf(activity.state, 127, "Playing map: %s", map);
  snprintf(activity.details, 127, "Where am I?");
  activityManager  = core->get_activity_manager(core);
  activityManager->update_activity(activityManager, &activity, nullptr, [] (void *callback_data, EDiscordResult result) {
    LOG(LOG_DEBUG) << "discord result:" << result;
  });
#endif // USE_DISCORD_RPC
}

void DiscordRpc::processCallback()
{
#ifdef USE_DISCORD_RPC
  if (!core)
    return;

  core->run_callbacks(core);
#endif 
}

void DiscordRpc::shutdown()
{
#ifdef USE_DISCORD_RPC
  if (core)
    core->destroy(core);

  core = nullptr;
#endif // USE_DISCORD_RPC
}
