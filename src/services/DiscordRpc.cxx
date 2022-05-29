#include "DiscordRpc.hxx"
#include "LOG.hxx"

#ifdef USE_DISCORD_RPC
#include "discord.h"
#include <memory>

static std::unique_ptr<discord::Core> core;
constexpr DiscordClientId appId = 982331112061808731;

#endif // USE_DISCORD_RPC

void DiscordRpc::init()
{
#ifdef USE_DISCORD_RPC
  discord::Core *newcore{};
  auto result = discord::Core::Create(appId, DiscordCreateFlags_Default, &newcore);
  core.reset(newcore);
  if (!core)
  {
    LOG(LOG_ERROR) << "Failed to instantiate discord core! (err " << static_cast<int>(result) << ")";
    return;
  }

  core->SetLogHook(discord::LogLevel::Debug, [](discord::LogLevel level, const char *message) { 
    LOG(LOG_INFO) << "Discord: " << static_cast<uint32_t>(level) << " " << message << "\n"; 
  });
#endif // USE_DISCORD_RPC
}

void DiscordRpc::updatePresence(const char *map)
{
#ifdef USE_DISCORD_RPC
  if (!core)
    return;

  std::string statestr = "Playing map: ";
  statestr.append(map);

  discord::Activity activity{};
  activity.SetDetails("Where am I?");
  activity.SetState(statestr.c_str());
  activity.SetType(discord::ActivityType::Playing);
  core->ActivityManager().UpdateActivity(activity, [](discord::Result result) { 
    LOG(LOG_INFO) << ((result == discord::Result::Ok) ? "Succeeded" : "Failed") << " updating activity!";
  });
#endif // USE_DISCORD_RPC
}

void DiscordRpc::processCallback()
{
#ifdef USE_DISCORD_RPC
  if (core)
    core->RunCallbacks();
#endif 
}

void DiscordRpc::shutdown()
{
#ifdef USE_DISCORD_RPC
  core.reset();
#endif // USE_DISCORD_RPC
}
