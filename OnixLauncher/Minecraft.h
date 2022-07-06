#pragma once
#include <string>
#include "Utils.h"

class Minecraft
{
private:

	 static inline std::string minecraftPath;
	 static inline std::string minecraftServersPath;
	 static inline std::string minecraftVersion;
	 static inline std::string minecraftPackageVersion;
	 static inline std::string xboxUsername;
	 static inline std::string xboxProfilePicture;
	 static inline std::string clientLauncherFolder;
	 static inline std::string clientDllPath;
	 static inline std::string launcherOptionsPath;
	 static inline bool minecraftIsX86;


public:
	static void Init();
	static inline Event<> onChanged;

	static const char* getMinecraftPath();
	static const char* getServersPath();
	static const char* getVersion();
	static const char* getPackageVersion();
	static const char* getXboxName();
	static const char* getXboxPicLink();
	static const char* getClientLauncherFolder();
	static const char* getClientDllPath();
	static const char* getLauncherOptionsPath();
	static inline const bool isX86() { return minecraftIsX86; }
	
	static bool injectClient(const char* filePath, std::string* errorMessage = nullptr);
	static void start();
	static bool isOpen();
	static bool isOpen_Slow(int delay);
	static bool isDllInjected(void* hProc, const char* dllpath);
	static bool waitForStart(int timeout = 5000);
	static bool waitForClose(int timeout = 5000);
	static bool longWaitForClose();
	static void kill();

	static bool installAppx(const std::string& appxPath, std::string& statusLabel, bool& updateWindow);

	static const char* addServer(const char* serverName, const char* serverIp, const short& serverPort);
};

