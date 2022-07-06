#include "Options.h"
#include "Minecraft.h"
#include "Logger.h"
#include "DiscordSDK/DiscordManager.h"
#include <vector>
#include <fstream>

void Options::loadOptions() {

	std::vector<std::string> lines;
	std::ifstream in(Minecraft::getLauncherOptionsPath());
	if (!in.is_open()) {
		Logger::log("Unble to read launcher settings", Logger::Level::Warn);
		return;
	}
	std::string temporary_line;
	while (std::getline(in, temporary_line))
		lines.emplace_back(temporary_line);
	in.close();

	auto safelyGetBoolAt = [&](int position, bool& option) -> bool {
		try {
			if (lines.size() <= (size_t)position) return false;
			std::string& str = lines.at(position);
			if (!str.empty())
				option = str.at(0) != '0';
			return true;
		}
		catch (const std::exception&) {
			return false;
		};
	};
	auto safelyGetStringAt = [&](int position, std::string& option) -> bool {
		try {
			if (lines.size() <= (size_t)position) return false;
			std::string& str = lines.at(position);
			if (!str.empty())
				option = str;
			return true;
		}
		catch (const std::exception&) {
			return false;
		};
	};


	safelyGetBoolAt(0, hideWorldNames);
	safelyGetBoolAt(1, hideServerIps);
	safelyGetBoolAt(2, hideMinecraftUsername);
	safelyGetBoolAt(3, disableRpc);
	safelyGetBoolAt(4, useCustomDll);
	safelyGetStringAt(5, customDllPath);
	safelyGetBoolAt(6, lightTheme);
	safelyGetBoolAt(7, hideToTrayAfterLaunch);
	safelyGetBoolAt(7, noInLauncherRpc);
	safelyGetBoolAt(8, hideToTrayAfterLaunch);
	safelyGetBoolAt(9, minimizeAfterLaunch);


	if (hideToTrayAfterLaunch)
		minimizeAfterLaunch = false;
}

const char* Options::getDllPath() {
	return useCustomDll ? customDllPath.c_str() : Minecraft::getClientDllPath();
}

void Options::saveOptions() {
	//save them to disk 
	std::ofstream out(Minecraft::getLauncherOptionsPath());
	if (!out.is_open()) {
		Logger::log("Unble to save launcher settings", Logger::Level::Error);
		return;
	}

	if (hideToTrayAfterLaunch)
		minimizeAfterLaunch = false;
		
	out << hideWorldNames << '\n'
		<< hideServerIps << '\n'
		<< hideMinecraftUsername << '\n'
		<< disableRpc << '\n'
		<< useCustomDll << '\n'
		<< customDllPath << '\n'
		<< lightTheme << '\n'
		<< noInLauncherRpc << '\n'
		<< hideToTrayAfterLaunch << '\n'
		<< minimizeAfterLaunch;

	out.close();

	//apply changes to rpc options
	if (!DiscordManager::lastStatus.empty())
		DiscordManager::Update(DiscordManager::lastStatus.c_str());
}
