#pragma once
#include <string>
#include "Utils.h"

class Options {
public:

	static inline bool hideWorldNames = false;
	static inline bool hideServerIps = false;
	static inline bool hideMinecraftUsername = false;
	static inline bool disableRpc = false;
	static inline bool noInLauncherRpc = false;

	static inline bool useCustomDll = false;
	static inline std::string customDllPath;

	static inline bool lightTheme = false;
	static inline Event<bool> onThemeChanged;

	static inline bool hideToTrayAfterLaunch = true;
	static inline bool minimizeAfterLaunch = false;

	static const char* getDllPath();

	static void loadOptions();
	static void saveOptions();
};

