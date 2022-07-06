#pragma once
#include <ctime>
#include <vector>
#include <string>
#include "include/discord_register.h"
#include "include/discord_rpc.h"

class DiscordManager {
public:

	struct Server {
		std::string name;
		std::string icon;
		std::vector<std::string> ips;
	};
	static inline std::vector<Server> rpcServers;

	static inline int64_t startTime;
	static inline bool wantDeath = false;
	static inline bool isDead = false;
	static inline bool lastEnableSetting = false;
	static inline std::string lastStatus;

	static void Start();
	static void Deinit();
	static void AddServers();
	static Server* getServer(const std::string& ip);

	static void Update(const char* Status);

};

