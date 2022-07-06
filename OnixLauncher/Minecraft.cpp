#include "Minecraft.h"  
#include <sstream>
#include <fstream>
#include <vector>
#include <filesystem>
#include "Logger.h"
#include <chrono>
#include <cmath>

#include <Windows.h>
#include <TlHelp32.h>
#include <psapi.h>

#pragma comment (lib, "windowsapp.lib")


//uwshit stuff
//they messed up something in the winrt header so this is to fix it till they do .......................
namespace Windows::Foundation {
	namespace impl {
		using filetime_period = std::ratio_multiply<std::ratio<100>, std::nano>;
	}
	using TimeSpan = std::chrono::duration<int64_t, impl::filetime_period>;
}
namespace winrt::impl {
	template <typename Async>
	auto wait_for(Async const& async, Windows::Foundation::TimeSpan const& timeout);
}
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Management.Deployment.h>
#include <winrt/Windows.Management.Deployment.h>
#include <winrt/Windows.Foundation.Collections.h>

#undef min
#undef max

static std::optional<winrt::Windows::ApplicationModel::Package> getMinecraftPackage() {
	winrt::Windows::Management::Deployment::PackageManager manager;
	winrt::Windows::Foundation::Collections::IIterator<winrt::Windows::ApplicationModel::Package> packages = manager.FindPackagesForUser(winrt::hstring()).First();

	do
	{
		winrt::Windows::ApplicationModel::Package package = packages.Current();
		if (package.Id().Name() == L"Microsoft.MinecraftUWP") {
			return package;
		}
	} while (packages.MoveNext());
	return std::optional<winrt::Windows::ApplicationModel::Package>();
}




void Minecraft::Init() {
	DWORD bufferSize = 65535;
	std::string buff;
	buff.resize(bufferSize);
	bufferSize = GetEnvironmentVariableA("LOCALAPPDATA", &buff[0], bufferSize);

	{
		std::stringstream ss;
		ss << buff.c_str();
		ss << "\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\LocalState\\games\\com.mojang\\";
		minecraftPath = ss.str();

		ss << "minecraftpe\\external_servers.txt";
		minecraftServersPath = ss.str();
		ss.clear();

		clientLauncherFolder = buff.c_str() + std::string("\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\RoamingState\\OnixClient\\Launcher\\");
		clientDllPath = clientLauncherFolder + "OnixClient.dll";
		launcherOptionsPath = clientLauncherFolder + "OnixLauncherOptions.txt";
		try {
			std::filesystem::create_directories(clientLauncherFolder);
		}catch (const std::exception& ) {}
	}


	{
		//get magic gaming file
		std::stringstream ss;
		ss << buff.c_str() << "\\Packages\\Microsoft.XboxApp_8wekyb3d8bbwe\\LocalState\\XboxLiveGamer.xml";
		std::ifstream xboxGamer(ss.str().c_str());
		if (xboxGamer.is_open() == true) {
			std::string xbg;
			xbg.resize(600);
			//skip the junk at the start
			while (std::getline(xboxGamer, xbg)) {
				if (xbg == "  ],") break;
				xbg.clear();
			}
			xbg.clear();

			//store usefull values in an array (or potentialy useful at least idk)
			std::string xboxInfo[31];
			for (int i = 0; i < 31; i++) //null the array
				xboxInfo[i] = "";

			for (int i = 0; i < 31; i++) {
				if (std::getline(xboxGamer, xbg)) {
					xboxInfo[i] = xbg;
				}
				xbg.clear();
			}
			xboxGamer.close();

			//Get gamertag
			{
				std::stringstream gaming;
				char gCount = 1;
				for (int i = 3; i < xboxInfo[2].size(); i++) {
					if (xboxInfo[2][i] == '\"') {
						gCount++;
						if (gCount == 4) break;
					}
					else { if (gCount == 3) gaming << xboxInfo[2][i]; }
				}
				Minecraft::xboxUsername = gaming.str().c_str();
				Logger::log("User Logged in: ", Minecraft::xboxUsername.c_str());
			}

			//Get gamerpic
			{
				std::stringstream gaming;
				char gCount = 1;
				for (int i = 3; i < xboxInfo[4].size(); i++) {
					if (xboxInfo[4][i] == '\"') {
						gCount++;
						if (gCount == 4) break;
					}
					else { if (gCount == 3) gaming << xboxInfo[4][i]; }
				}
				gaming << "&format=jpg&h=64&w=64"; //as I don't need more than 64x64 image, but im sure you will be able to struggle with that later lol, gl
				Minecraft::xboxProfilePicture = gaming.str();
			}

		}
		else {
			xboxUsername = std::string("Unknown");
			Logger::log("Unable to read the Xbox username, Try installing & Logging in Xbox Console Companion APP");
		}
	}

	buff.resize(0);
	
{//Get minecraft version
	do {
		auto package = getMinecraftPackage();
		if (package.has_value()) {
			winrt::Windows::ApplicationModel::PackageVersion version = package->Id().Version();
			short vBuild = version.Build > 0 ? version.Build / 100 : 0;
			minecraftPackageVersion = std::to_string(version.Major) + '.' + std::to_string(version.Minor) + '.' + std::to_string(version.Build)+ '.' + std::to_string(version.Revision);
			version.Build = vBuild;
			minecraftVersion = std::to_string(version.Major) + '.' + std::to_string(version.Minor) + '.' + std::to_string(vBuild);

			minecraftIsX86 = false;
			bool knowsForSure = false;

			winrt::hstring str = package->InstalledPath() + L"\\Minecraft.Windows.exe";
			std::ifstream exein(str.c_str(), std::ios::binary);
			if (exein.is_open()) {
				char* headerinfo = (char*)malloc(0x512);
				if (!headerinfo) break;
				exein.read(headerinfo, 0x512);
				for (int i = 0; i < 0x512; i++) {
					if (headerinfo[i + 0] == 'P' &&
						headerinfo[i + 1] == 'E' &&
						headerinfo[i + 2] == '\0' &&
						headerinfo[i + 3] == '\0') {

						uint16_t* magicNumber = (uint16_t*)(headerinfo + i + 4);
						minecraftIsX86 = *magicNumber != 0x8664;
						knowsForSure = true;
						break;
					}
				}
				free(headerinfo);
			}
			if (knowsForSure) {
				Logger::log("Minecraft is ", isX86() ? "not X64" : "X64");
			} else {
				Logger::log("Minecraft is ", isX86() ? "probably not X64" : "probably X64");
			}
			break;
			
		}
	} while (false);

}

	Minecraft::onChanged();
}


const char* Minecraft::getXboxName() {
	return Minecraft::xboxUsername.c_str();
}

const char* Minecraft::getXboxPicLink() {
	return Minecraft::xboxProfilePicture.c_str();
}


const char* Minecraft::getMinecraftPath() {
	return Minecraft::minecraftPath.c_str();
}

const char* Minecraft::getServersPath() {
	return Minecraft::minecraftServersPath.c_str();
}

const char* Minecraft::getClientLauncherFolder() {
	return Minecraft::clientLauncherFolder.c_str();
}

const char* Minecraft::getClientDllPath() {
	return Minecraft::clientDllPath.c_str();
}

const char* Minecraft::getLauncherOptionsPath() {
	if (Minecraft::launcherOptionsPath.empty()) {
		DWORD bufferSize = 65535;
		std::string buff;
		buff.resize(bufferSize);
		bufferSize = GetEnvironmentVariableA("LOCALAPPDATA", &buff[0], bufferSize);
		launcherOptionsPath = buff.c_str() + std::string("\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\RoamingState\\OnixClient\\Launcher\\OnixLauncherOptions.txt");
	}
	return Minecraft::launcherOptionsPath.c_str();
}


const char* Minecraft::getVersion() {
	return Minecraft::minecraftVersion.c_str();
}

const char* Minecraft::getPackageVersion() {
	return Minecraft::minecraftPackageVersion.c_str();
}

const char* Minecraft::addServer(const char* serverName, const char* serverIp, const short& serverPort) {
	std::ofstream out;
	std::ifstream rf;
	rf.open(Minecraft::minecraftServersPath.c_str());


	if (rf.is_open() == true) {
		std::vector<std::string> servers;
		servers.reserve(50);

		std::string serv;
		short serverIndex = 2;

		{ // add the new server to the list
			std::stringstream newServer;
			newServer << "1:" << serverName << ":" << serverIp << ":" << serverPort;
			servers.push_back(newServer.str());
		}

		//for each server basicly
		while (std::getline(rf, serv)) {

			short cursor = 0;
			short cCount = 0;
			short crfIndex = 0;
			short cfIndex = 0;
			short clIndex = 0;

			//Find server IP
			while (cfIndex == 0 || clIndex == 0) {

				if (serv[cursor] == ':' && crfIndex == 0 && cCount == 0)  crfIndex = cursor;
				if (serv[cursor] == ':' && cfIndex == 0 && cCount == 1)  cfIndex = cursor;
				if (serv[cursor] == ':' && clIndex == 0 && cCount == 2)  clIndex = cursor;
				if (serv[cursor] == ':') cCount++;

				cursor++;
				if (cursor == serv.size()) break;
			}

			//Assemble the IP string
			std::stringstream sName;
			for (int i = cfIndex + 1; i != clIndex; i++)
				sName << serv[i];


			// if ip != our new server
			if (sName.str() != serverIp) {
				const char* sip = serv.c_str();
				std::stringstream servMinecraft;

				servMinecraft << serverIndex;
				for (int i = crfIndex; i <= serv.size(); i++) {
					servMinecraft << sip[i];
				}

				servers.push_back(servMinecraft.str());
				serverIndex++;
			}
		}
		//wow, here servers is filled with values
		out.open(Minecraft::minecraftServersPath.c_str());
		if (out.is_open() == false) {
			rf.close();
			return "Impossible to write to file.";
		}
		
		//write all servers to file!
		for (int i = 0; i < servers.size(); i++) {
			out << servers[i];
			if (i < servers.size() - 1) out << '\n';
		}
		out.close();
		rf.close();

		return "Server added to list!";
	}
	else {
		out.open(Minecraft::minecraftServersPath.c_str());
		if (out.is_open() != true)
			return "Impossible to read/write to file.";

		out << "1:" << serverName << ":" << serverIp << ":" << serverPort;
		out.close();
		return "Server added to list!";
	}

}




static DWORD getPID(const char* procName) {
	DWORD* PidList = new DWORD[4096];
	DWORD damn = 0;
	if (!K32EnumProcesses(PidList, 4096 * sizeof(DWORD), &damn)) {
		delete[] PidList;
		Logger::log("impossible to get processes.");
		return 0;
	}
	damn = std::min((int)damn, 4096);

	for (DWORD i = 0; i < damn; i++) {
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PidList[i]);
		if (hProcess) {
			DWORD cbNeeded;
			HMODULE mod;
			if (!K32EnumProcessModules(hProcess, &mod, sizeof(HMODULE), &cbNeeded)) continue;
			char textualContent[128];
			K32GetModuleBaseNameA(hProcess, mod, textualContent, sizeof(textualContent) / sizeof(textualContent[0]));
			CloseHandle(hProcess);
			if (!strcmp(textualContent, procName))
				return PidList[i];
		}
	}
	return 0;
}




void Minecraft::start() {
	system("start /min explorer.exe shell:appsFolder\\Microsoft.MinecraftUWP_8wekyb3d8bbwe!App");
}
bool Minecraft::isOpen() {
	return getPID("Minecraft.Windows.exe") != 0;
}
bool Minecraft::isOpen_Slow(int delay) {
	DWORD* PidList = new DWORD[4096];
	Sleep(1000);
	DWORD damn = 0;
	if (!K32EnumProcesses(PidList, 4096 * sizeof(DWORD), &damn)) {
		//Logger::log("impossible to get processes.");
		delete[] PidList;
		return 0;
	}
	damn = std::min((int)damn, 4096);
	Sleep(1000);

	for (DWORD i = 0; i < damn; i++) {
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, PidList[i]);
		if (hProcess) {
			DWORD cbNeeded;
			HMODULE mod;
			if (!K32EnumProcessModules(hProcess, &mod, sizeof(HMODULE), &cbNeeded)) continue;
			char textualContent[128];
			K32GetModuleBaseNameA(hProcess, mod, textualContent, sizeof(textualContent) / sizeof(textualContent[0]));
			CloseHandle(hProcess);
			Sleep(delay);
			if (!strcmp(textualContent, "Minecraft.Windows.exe")) {
				delete[] PidList;
				Sleep(200);
				return true;
			}
		}
	}
	delete[] PidList;
	Sleep(200);
	return false;
}
bool Minecraft::isDllInjected(void* hProc, const char* dllpath) {
	HMODULE hMods[1024];
	DWORD cbNeeded;
	if (EnumProcessModules((HANDLE)hProc, hMods, sizeof(hMods), &cbNeeded)) {
		for (int i = 0; i < std::min((int)(cbNeeded / sizeof(HMODULE)), 1024); i++) {
			char szModName[MAX_PATH];

			if (K32GetModuleFileNameExA(hProc, hMods[i], szModName, sizeof(szModName) / sizeof(char))) 
				if (strstr(szModName, dllpath) != 0) 
					return true;
		}
	}
	return false;
}
bool Minecraft::waitForStart(int timeout) {
	int timenow = 0;
	do {
		if (getPID("Minecraft.Windows.exe") != 0)
			break;
		Sleep(1);
		timenow++;
		if (timeout < timenow) return false;
	} while (true);
	return true;
}
bool Minecraft::waitForClose(int timeout) {
	int timenow = 0;
	do {
		if (getPID("Minecraft.Windows.exe") == 0)
			break;
		Sleep(1);
		timenow++;
		if (timeout < timenow) return false;
	} while (true);
	return true;
}

bool Minecraft::longWaitForClose() {
	DWORD* PidList = new DWORD[4096];
	DWORD damn = 0;
	if (!K32EnumProcesses(PidList, 4096 * sizeof(DWORD), &damn)) {
		//Logger::log("impossible to get processes.");
		delete[] PidList;
		return false;
	}
	damn = std::min((int)damn, 4096);

	for (DWORD i = 0; i < damn; i++) {
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | SYNCHRONIZE, FALSE, PidList[i]);
		if (hProcess) {
			DWORD cbNeeded;
			HMODULE mod;
			if (!K32EnumProcessModules(hProcess, &mod, sizeof(HMODULE), &cbNeeded)) continue;
			char textualContent[128];
			K32GetModuleBaseNameA(hProcess, mod, textualContent, sizeof(textualContent) / sizeof(textualContent[0]));
			if (!strcmp(textualContent, "Minecraft.Windows.exe")) {
				delete[] PidList;
				WaitForSingleObject(hProcess, INFINITE);
				CloseHandle(hProcess);
				return true;
			}
			CloseHandle(hProcess);
		}
	}
	return false;
}

void Minecraft::kill() {
	DWORD pid = getPID("Minecraft.Windows.exe");
	if (!pid) return;
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, pid);
	if (hProcess) {
		TerminateProcess(hProcess, 0);
		CloseHandle(hProcess);
		Sleep(250);
	}
}

static bool backupFiles(winrt::Windows::ApplicationModel::Package mcpackage, std::string& statusLabel, bool& updateWindow) {
	std::string gameRoot;
	DWORD gameRootSize=512;
	gameRoot.reserve(gameRootSize);
	gameRootSize = GetEnvironmentVariableA("LOCALAPPDATA", &gameRoot[0], gameRootSize);
	gameRoot = gameRoot.c_str();
	gameRoot += "\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\";
	
	std::string backupRoot;
	DWORD backupRootSize=512;
	backupRoot.reserve(backupRootSize);
	backupRootSize = GetTempPathA(backupRootSize , &backupRoot[0]);
	backupRoot = backupRoot.c_str();
	backupRoot += "MinecraftBackup\\";
	try {
		if (std::filesystem::exists(backupRoot))
			std::filesystem::remove_all(backupRoot);
	} catch (const std::exception&) {
	}
	std::filesystem::create_directories(backupRoot);

	//std::filesystem::copy
	std::string comMojangFolder = gameRoot + "LocalState\\games\\com.mojang";
	std::string roamingStateFolder = gameRoot + "RoamingState";
	std::string backupComMojangFolder = backupRoot + "com.mojang";
	std::string backupRoamingStateFolder = backupRoot + "RoamingState";
	std::filesystem::create_directories(backupComMojangFolder);
	std::filesystem::create_directories(backupRoamingStateFolder);
	comMojangFolder += "\\*"; comMojangFolder.push_back(0);
	roamingStateFolder += "\\*"; roamingStateFolder.push_back(0);
	backupComMojangFolder.push_back(0);
	backupRoamingStateFolder.push_back(0);

	statusLabel = "Backing up RoamingState...";
	updateWindow = true;
	{
		SHFILEOPSTRUCTA s = { 0 };
		s.hwnd = 0;
		s.wFunc = FO_COPY;
		s.pTo = backupRoamingStateFolder.c_str();
		s.pFrom = roamingStateFolder.c_str();
		s.fFlags = FOF_NOCONFIRMATION;
		s.lpszProgressTitle = "Backing up Minecraft files...";
		s.fAnyOperationsAborted = false;
		if (SHFileOperationA(&s)) return false; // returns 0 on SUCCESS
	}
	statusLabel = "Backing up LocalState...";
	updateWindow = true;
	{
		SHFILEOPSTRUCTA s = { 0 };
		s.hwnd = 0;
		s.wFunc = FO_COPY;
		s.pTo = backupComMojangFolder.c_str();
		s.pFrom = comMojangFolder.c_str();
		s.fFlags = FOF_NOCONFIRMATION;
		s.lpszProgressTitle = "Backing up Minecraft files...";
		s.fAnyOperationsAborted = false;
		if (SHFileOperationA(&s)) return false; //returns 0 on SUCCESS
	}
	return true;
}

extern bool IsInstallingAppx;
bool Minecraft::installAppx(const std::string& appxPath, std::string& statusLabel, bool& updateWindow) {
	IsInstallingAppx = true;
	winrt::Windows::Management::Deployment::PackageManager packageManager;
	//auto package = getMinecraftPackage();
	//if (package.has_value())
	//	if (!backupFiles(*package, statusLabel, updateWindow))
	//		return false;
	//bool didBackup = package.has_value();
	//if (didBackup) statusLabel = "Minecraft files are backed up!";
	//updateWindow = didBackup;
	
	using DeploymentOptions = winrt::Windows::Management::Deployment::DeploymentOptions;
	using Uri = winrt::Windows::Foundation::Uri;

	std::vector<winrt::Windows::Foundation::Uri> dependencies = { Uri(winrt::to_hstring("https://github.com/bernarddesfosse/onix_compatible_appx/raw/main/Microsoft.Services.Store.Engagement_10.0.19011.0_x64__8wekyb3d8bbwe.Appx")) };

	auto progress = packageManager.AddPackageAsync(Uri(winrt::to_hstring(appxPath.c_str())),
		std::move(dependencies),
		DeploymentOptions::ForceUpdateFromAnyVersion | DeploymentOptions::ForceTargetApplicationShutdown);

	progress.Progress([&statusLabel, &updateWindow](winrt::Windows::Foundation::IAsyncOperationWithProgress<winrt::Windows::Management::Deployment::DeploymentResult, winrt::Windows::Management::Deployment::DeploymentProgress> const& asyncInfo, winrt::impl::param_type<winrt::Windows::Management::Deployment::DeploymentProgress> const& progressInfo) {
		if (progressInfo.percentage == 0) 
			statusLabel = "Extracting....";
		else
			statusLabel = "Installing new version: " + std::to_string(progressInfo.percentage) + "%";
		updateWindow = true;
		});

	volatile bool finishedInstalling = false;
	volatile bool failed = false;

	progress.Completed([&finishedInstalling](winrt::Windows::Foundation::IAsyncOperationWithProgress<winrt::Windows::Management::Deployment::DeploymentResult, winrt::Windows::Management::Deployment::DeploymentProgress>const& operation, winrt::Windows::Foundation::AsyncStatus status) {
			finishedInstalling = true;
		});
	statusLabel = "Preparing....";
	updateWindow = true;
	while (!finishedInstalling) Sleep(1);

	HRESULT installed = progress.ErrorCode().value;
	Minecraft::Init();
	if (installed == S_OK) {
		statusLabel = "Successfully installed version!)";
		IsInstallingAppx = false;
		return true;
	} else {
		statusLabel = "Failed to install version! (" + std::to_string(installed) + ")";
		Sleep(10000);
		updateWindow = true;
		IsInstallingAppx = false;
		return false;
	}
}
	
static constexpr char const* rc = "?"; // replacement_char
// table mapping ISO-8859-1 characters to similar ASCII characters
static std::array<char const*, 96> conversions = { {
   " ",  "!","c","L", rc,"Y", "|","S", rc,"C","a","<<",   rc,  "-",  "R", "-",
	rc,"+/-","2","3","'","u", "P",".",",","1","o",">>","1/4","1/2","3/4", "?",
   "A",  "A","A","A","A","A","AE","C","E","E","E", "E",  "I",  "I",  "I", "I",
   "D",  "N","O","O","O","O", "O","*","0","U","U", "U",  "U",  "Y",  "P","ss",
   "a",  "a","a","a","a","a","ae","c","e","e","e", "e",  "i",  "i",  "i", "i",
   "d",  "n","o","o","o","o", "o","/","0","u","u", "u",  "u",  "y",  "p", "y"
} };

static std::string to_ascii(const std::wstring& utf16) {
	std::u32string utf32(utf16.begin(), utf16.end());

	std::string ascii;
	ascii.reserve(utf16.size());
	for (char32_t c : utf32) {
		if (c <= U'\u007F')
			ascii.push_back(static_cast<char>(c));
		else if (U'\u00A0' <= c && c <= U'\u00FF')
			ascii.append(conversions[c - U'\u00A0']);
		else if (c == L'’')
			ascii.push_back('\'');
		else
			ascii.append(rc);
	}
	return ascii;
}
static std::string LastErrorAsString() {
	DWORD lastError = GetLastError();
	wchar_t* message = nullptr;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&message, 0, NULL);
	std::string returnValue = to_ascii(std::wstring(L"(") + std::to_wstring(lastError) + std::wstring(L") ") + message);

	LocalFree(message);
	return returnValue;
}


bool Minecraft::injectClient(const char* filePath, std::string* errorMessage) {
	Logger::log("Trying to inject: ", filePath);
	//Check if the file is a thing in the universe
	WIN32_FIND_DATAA FindFileData;
	HANDLE handle = FindFirstFileA(filePath, &FindFileData);
	if (handle != INVALID_HANDLE_VALUE)
		FindClose(handle);
	else {
		Logger::log("There is no file to inject at: ", filePath, Logger::Level::Error);
		if (errorMessage) *errorMessage = "There is no file to inject at: " + std::string(filePath);
		return false;
	}

	
	//get PID
	DWORD mcPid = getPID("Minecraft.Windows.exe");
	if (mcPid == NULL) {
		Logger::log("Could not locate Minecraft.Windows.exe", Logger::Level::Error);
		if (errorMessage) *errorMessage = "Could not locate Minecraft.Windows.exe";
		return false;
	}
	Logger::log("Minecraft PID: ", std::to_string(mcPid).c_str());


	HANDLE hProc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, 0, mcPid);
	if (!hProc) {
		Logger::log("Unable to have access to minecraft: ", LastErrorAsString(), Logger::Level::Error);
		if (errorMessage) *errorMessage = "Permission denied when accessing the minecraft process.\n" + LastErrorAsString();
		return false;
	}
	
	if (Minecraft::isDllInjected(hProc, filePath)) {
		Logger::log("This Dll is already injected!", Logger::Level::Error);
		if (errorMessage) *errorMessage = "The dll is already injected";
		CloseHandle(hProc);
		return false;
	}

	void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (loc == nullptr) {
		Logger::log("Unable to allocate remote memory: ", LastErrorAsString(), Logger::Level::Error);
		if (errorMessage) *errorMessage = "Could not allocate memory in minecraft\n" + LastErrorAsString();
		CloseHandle(hProc);
		return false;
	}

	SIZE_T written = 0;
	if (WriteProcessMemory(hProc, loc, filePath, strlen(filePath) + 1, &written) == 0 || written == 0) {
		Logger::log("Unable to write in remote allocated memory: ", LastErrorAsString(), Logger::Level::Error);
		if (errorMessage) *errorMessage = "Could not write memory in Minecraft.Windows.exe\n" + LastErrorAsString();
		VirtualFreeEx(hProc, loc, 0, 0x8000);
		CloseHandle(hProc);
		return false;
	}


	HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);
	if (hThread) {
		WaitForSingleObject(hThread, 4000);
		Sleep(200);
		Logger::log("Remote thread finished.", Logger::Level::Info);
		VirtualFreeEx(hProc, loc, 0, 0x8000);

		if (!Minecraft::isDllInjected(hProc, filePath)) {
			CloseHandle(hProc);
			if (errorMessage) *errorMessage = "Everything went fine but the dll is absent :(";
			Logger::log("Everything went well, but the dll could not be injected", Logger::Level::Error);
			Logger::log("This is most likely due to your antivirus blocking the modification of an other process's code (minecraft)", Logger::Level::Error);
			DWORD exitCode;
			if (GetExitCodeThread(hThread, &exitCode)) {
				if (errorMessage) *errorMessage += "\nLoadResult: " + std::to_string(exitCode);
				Logger::logf("Load result: %d", Logger::Level::Info, exitCode);
			}
			else {
				if (errorMessage) *errorMessage += "\nNo load result availible.";
				Logger::log("No availible load result", Logger::Level::Warn);
			}
			CloseHandle(hThread);
			return false;
		}

		CloseHandle(hThread);
		CloseHandle(hProc);
		return true;
	}
	else {
		if (errorMessage) *errorMessage = "Could not execute code inside of minecraft\n" + LastErrorAsString();
		Logger::log("Could not create remote thread: ", LastErrorAsString(), Logger::Level::Error);
	}
	


	VirtualFreeEx(hProc, loc, 0, 0x8000);
	CloseHandle(hProc);

	return false;
}
