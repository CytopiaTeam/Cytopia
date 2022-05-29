#ifndef DISCORDRPC_HXX_
#define DISCORDRPC_HXX_

struct DiscordRpc {
  static void init();
  static void updatePresence(const char *map);
  static void processCallback();
  static void shutdown();
};

#endif // DISCORDRPC_HXX_
