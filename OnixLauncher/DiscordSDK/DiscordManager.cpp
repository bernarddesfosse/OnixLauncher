#include "DiscordManager.h"
#include <Windows.h>
#include <fstream>
#include <cstdio>
#include <filesystem>
#include "../Minecraft.h"
#include "../Logger.h"
#include "../Utils.h"
#include "../json.hpp"
#include "../Options.h"
namespace json = nlohmann;

#ifndef NDEBUG
#pragma comment(lib, "DiscordSDK/DiscordRPC_x64_Debug.lib")
#else
#pragma comment(lib, "DiscordSDK/DiscordRPC_x64_Release.lib")
#endif

void DiscordManager::Deinit() {
    Discord_ClearPresence();
    Discord_Shutdown();
}

void DiscordManager::AddServers() {
    rpcServers.clear();
    MemoryBuffer server = Utils::downloadFileInMemory("https://raw.githubusercontent.com/bernarddesfosse/onixclientautoupdate/main/DiscordRpcServers.json");
    const char* jsonStr = R"({"servers":[{"icon":"server_cubecraft","ips":["mco.cubecraft.net"],"name":"Cubecraft"},{"icon":"server_hive","ips":["geo.hivebedrock.network","fr.hivebedrock.network","ca.hivebedrock.network","sg.hivebedrock.network","jp.hivebedrock.network"],"name":"The Hive"},{"icon":"server_mineville","ips":["play.inpvp.net"],"name":"Mineville"},{"icon":"server_mineplex","ips":["mco.mineplex.com"],"name":"Mineplex"},{"icon":"server_galaxite","ips":["play.galaxite.net"],"name":"Galaxite"},{"icon":"server_lifeboat","ips":["mco.lbsg.net"],"name":"Lifeboat"},{"icon":"server_nethergaymes","ips":["play.nethergames.org"],"name":"NetherGames"},{"icon":"server_hyperlands","ips":["play.hyperlandsmc.net"],"name":"Hyperlands"},{"icon":"server_zeqa","ips":["zeqa.net","51.222.245.157","66.70.181.97","651.222.244.138","51.210.223.196","164.132.200.60","51.210.223.195","51.79.163.9","139.99.120.127","51.79.177.168","51.79.162.196"],"name":"Zeqa"},{"icon":"server_pixelparadise","ips":["play.pixelparadise.gg"],"name":"Pixel Paradise"}]})";

    if (server) {
        ((char*)server.buffer)[server.size] = 0;
        jsonStr = (char*)server.buffer;
    }
     // default servers

    

    json::json serverRoot = json::json::parse(jsonStr);
    auto root = serverRoot.find("servers");
    if (root != serverRoot.end() && root->is_array()) {
        for (const json::json& server : *root) {
            DiscordManager::Server newServer;
            auto nameI = server.find("name");
            if (nameI != server.end() && nameI->is_string())
                newServer.name = nameI->get<std::string>();
            auto iconI = server.find("icon");
            if (iconI != server.end() && iconI->is_string())
                newServer.icon = iconI->get<std::string>();
            auto ipListRootI = server.find("ips");
            if (ipListRootI != server.end() && ipListRootI->is_array())
                for (const json::json& ip : *ipListRootI) {
                    if (ip.is_string())
                        newServer.ips.emplace_back(ip.get<std::string>());
                }
            rpcServers.emplace_back(newServer);
        }
    }
}

DiscordManager::Server* DiscordManager::getServer(const std::string& ip) {
    for (auto& server : rpcServers)
        for (const std::string& serverIp : server.ips)
            if (serverIp == ip)
                return &server;
    return nullptr;
}


static void DiscordManager_HandleReady(const DiscordUser* request) {
    Logger::logf("Rpc initialised for %s#%s/%s", Logger::Level::Info, request->username, request->discriminator, request->userId);
    DiscordManager::Update("In the Launcher");
}
static void DiscordManager_HandleError(int code, const char* message) {
    Logger::log("DiscordRPC Error: ", message, Logger::Level::Warn);
}

static const char* GetLargeImage() {
    SYSTEMTIME time;
    GetLocalTime(&time);

    if (time.wMonth == 10) return "onixlogobuthalloween";
    if (time.wMonth == 12) return "onixlogobutchristmas";

    static bool isSuspicious = rand() % 150 == 3;
    return isSuspicious ? "sussylogo" : "logo";
}

void DiscordManager::Start() {
    SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN | THREAD_PRIORITY_IDLE);
    startTime = time(NULL);
    if (!Options::disableRpc) lastEnableSetting = !Options::disableRpc;

    std::string RpcFile = Minecraft::getClientLauncherFolder() + std::string("Server.txt");
    bool wasMinecraftStarted = Minecraft::isOpen();
    time_t lastAccess = time(0);
    while (!wantDeath) {
        if (Options::disableRpc != lastEnableSetting) {
            lastEnableSetting = Options::disableRpc;
            if (Options::disableRpc) {
                Deinit();
                rpcServers.clear();
            } else {
                DiscordEventHandlers handlers;
                memset(&handlers, 0, sizeof(handlers));
                handlers.ready = DiscordManager_HandleReady;
                handlers.errored = DiscordManager_HandleError;
                Discord_Initialize("981243754625269760", &handlers, 1, 0);

                AddServers();
            }
        }
        if (Options::disableRpc) {
            Sleep(250);
            continue;
        }

        Sleep(250);
        Discord_RunCallbacks();
        Sleep(250);
#ifdef DISCORD_DISABLE_IO_THREAD
        Discord_UpdateConnection();
#endif

        bool mcStartedNow = Minecraft::isOpen_Slow(1);
        Sleep(100);
        if (mcStartedNow != wasMinecraftStarted) {
            wasMinecraftStarted = mcStartedNow;
            if (!mcStartedNow) {
                DeleteFileA(RpcFile.c_str());
                DiscordManager::Update("In the Launcher");
            }
        }
        Sleep(300);
        if (std::filesystem::exists(RpcFile)) {
            struct stat stats;
            if (stat(RpcFile.c_str(), &stats) == 0) {
                Sleep(200);
                if (stats.st_mtime > lastAccess) {
                    std::ifstream in(RpcFile);
                    if (in.is_open()) {
                        std::string serverIp;
                        std::getline(in, serverIp);
                        lastAccess = stats.st_mtime;
                        if (serverIp.empty()) {
                            Update("In the menus");
                        } else {
                            Sleep(5);
                            char buff[256];
                            sprintf_s(buff, 256, "Playing %s", serverIp.c_str());
                            Sleep(500);
                            Update(buff);
                            Sleep(500);
                        }
                        in.close();
                    }
                }
            }
        }

#ifdef DISCORD_DISABLE_IO_THREAD
        Discord_UpdateConnection();
#endif
        Sleep(500);
    }
    isDead = true;
}

void DiscordManager::Update(const char* Status) {
    if (Options::disableRpc) return;
    lastStatus = Status;
    if (Options::noInLauncherRpc && !strcmp(Status, "In the Launcher"))
        return Discord_ClearPresence();


    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    bool isPlayingWorld = (strstr(Status, "Playing in a World,") == Status || strstr(Status, "Playing In a World,") == Status);
    if (Options::hideWorldNames && isPlayingWorld)
        discordPresence.state = "Playing in a World";
    else if (Options::hideServerIps && !isPlayingWorld && strstr(Status, "Playing") == Status)
        discordPresence.state = "Playing on a Server";
    else
        discordPresence.state = Status;
    discordPresence.startTimestamp = startTime;
    discordPresence.endTimestamp = NULL;
    discordPresence.largeImageKey = GetLargeImage();
    discordPresence.largeImageText = "Onix Client";
    discordPresence.smallImageKey = "mc-icon";

    if (!Options::hideServerIps) {
        Server* server = getServer(strstr(Status, "Playing") == Status ? Status + 8 : Status);
        char dhjksadgyuash[69];
        if (server) {
            sprintf_s(dhjksadgyuash, 69, "Playing %s%s", ((server->name.c_str()[0] == 'I' || server->name.c_str()[0] == 'i') && server->name.c_str()[1] == 'n') ? "" : "on ", server->name.c_str());
            discordPresence.state = dhjksadgyuash;
            discordPresence.smallImageKey = server->icon.c_str();
        }
    }

    char buff[50];
    memset(buff, 0, 50); 
    sprintf_s(buff, 50, "%s %s", "As", Options::hideMinecraftUsername ? "-Anonymous-" : Minecraft::getXboxName());
    discordPresence.smallImageText = buff;

    char buff2[50];
    memset(buff2, 0, 50); 
    sprintf_s(buff2, 50, "%s %s", "Minecraft", Minecraft::getVersion());
    discordPresence.details = buff2;


    Discord_UpdatePresence(&discordPresence);  
    Logger::log("Updated RPC to: ", discordPresence.state);
}