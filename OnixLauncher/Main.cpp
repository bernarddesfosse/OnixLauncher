#include <windows.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <assert.h>
#include "resource.h"
#include "Graphics.h"
#include "Logger.h"
#include "Animations.h"
#include "Minecraft.h"
#include "Utils.h"
#include "DiscordSDK/DiscordManager.h"
#include "UIManager.h"
#include "Options.h"
#include <Psapi.h>

UIManager* mainWindow = nullptr;
extern UIManager* settingsWindow;
const char* LAUNCHER_VERSION = "0.02";

std::string latestGithubVersion;
ClientInformation lastClientInformation;




bool ClientSupportsMinecraftVersion = false;
bool IsReadyToLaunchTheClient = false;
bool ShouldAvoidLaunching = false;
bool IsInstallingAppx = false;
std::atomic_bool CurrentlyUpdatingTheDll = false;

DWORD UpdateOnixClientDllFile(bool throwMessageBoxes) {
	if (CurrentlyUpdatingTheDll) return 0;

	CurrentlyUpdatingTheDll = true;
	char UrlBuffer[256];
	sprintf_s(UrlBuffer, 256, "https://raw.githubusercontent.com/bernarddesfosse/onixclientautoupdate/main/LatestHash.txt?rtticprnawgiyuttpswthrudoingywyutbijdwc=%llu", time(NULL));
	MemoryBuffer onixDllLatestHash = Utils::downloadFileInMemory(UrlBuffer);
	if (!onixDllLatestHash) {
		Logger::log("Unable to check for dll updates!", Logger::Level::Warn);
		CurrentlyUpdatingTheDll = false;
		return 1;
	}
	sprintf_s(UrlBuffer, 256, "https://raw.githubusercontent.com/bernarddesfosse/onixclientautoupdate/main/LatestVersion.txt?rtticprnawgiyuttpswthrudoingywyutbijdwc=%llu", time(NULL));
	MemoryBuffer onixDllLatestVersion = Utils::downloadFileInMemory(UrlBuffer);
	if (!onixDllLatestVersion) {
		Logger::log("Unable to check for dll updates!", Logger::Level::Warn);
		CurrentlyUpdatingTheDll = false;
		return 1;
	}

	char GithubHash[48] = { 0 };
	memcpy_s(GithubHash, sizeof(GithubHash), onixDllLatestHash.buffer, onixDllLatestHash.size);
	char GithubVersion[48] = { 0 };
	memcpy_s(GithubVersion, sizeof(GithubVersion), onixDllLatestVersion.buffer, onixDllLatestVersion.size);
	{ //   \n removal
		for (int i = 0; i < 48; i++) {
			if (GithubHash[i] == '\0') break;
			if (GithubHash[i] == '\n') { GithubHash[i] = '\0'; break; };
			if (GithubHash[i] == '\r') { GithubHash[i] = '\0'; break; };
		}
		for (int i = 0; i < 48; i++) {
			if (GithubVersion[i] == '\0') break;
			if (GithubVersion[i] == '\n') { GithubVersion[i] = '\0'; break; };
			if (GithubVersion[i] == '\r') { GithubVersion[i] = '\0'; break; };
		}
	}
	latestGithubVersion = GithubVersion;

	if (Options::useCustomDll) {
		lastClientInformation = Utils::getClientDllInfo(Options::getDllPath());
		ClientSupportsMinecraftVersion = true;
		if (!lastClientInformation) {
			Logger::log("Could not get DLL information (file might not exist)!", Logger::Level::Error);
			CurrentlyUpdatingTheDll = false;
			return 0;
		}

		if (std::filesystem::exists(Options::getDllPath())) {
			TextElement* element = (TextElement*)mainWindow->getElement("OnixClientVersionText");
			if (element && element->type == UIElement::Type::Text) {
				element->text = "Version " + std::string(lastClientInformation.clientVersion);
				mainWindow->gfx->redraw = true;
			}
		}

		bool minecraftVersionSupported = false;
		if (strlen(Minecraft::getPackageVersion()) == 0)
			Logger::log("Unable to get MCBE version!", Logger::Level::Warn);
		else {
			for (const std::string& version : lastClientInformation.supportedMinecraftPackageVersions) {
				if (version == Minecraft::getPackageVersion()) {
					minecraftVersionSupported = true;
					break;
				}
			}
			if (!minecraftVersionSupported || lastClientInformation.supportedMinecraftPackageVersions.empty() || lastClientInformation.supportedMinecraftVersions.empty()) {
				ClientSupportsMinecraftVersion = false;
				Logger::logf("The dll DOES NOT supports your minecraft version (%s or %s)", Logger::Level::Warn, Minecraft::getPackageVersion(), Minecraft::getVersion());
				Logger::logf("Latest supported version for this dll: %s (or %s)", Logger::Level::Warn, lastClientInformation.supportedMinecraftPackageVersions[0].c_str(), lastClientInformation.supportedMinecraftVersions[0].c_str());
				Logger::logf("Oldest supported version for this dll: %s (or %s)", Logger::Level::Warn, lastClientInformation.supportedMinecraftPackageVersions.back().c_str(), lastClientInformation.supportedMinecraftVersions.back().c_str());
			}
			else {
				Logger::logf("The dll Supports your minecraft version (%s or %s)", Logger::Level::Info, Minecraft::getPackageVersion(), Minecraft::getVersion());
				ClientSupportsMinecraftVersion = true;
			}
			CurrentlyUpdatingTheDll = false;
			return 0;
		}
	}

	if (std::filesystem::exists(Minecraft::getClientDllPath())) {
		std::string currentDllHash = Utils::getMD5Hash(Minecraft::getClientDllPath());

		if (GithubHash != currentDllHash) {
			Logger::log("We found an update!");
			Logger::log("Starting the download...");
			sprintf_s(UrlBuffer, 256, "https://raw.githubusercontent.com/bernarddesfosse/onixclientautoupdate/main/OnixClient.dll?rtticprnawgiyuttpswthrudoingywyutbijdwc=%llu", time(NULL));
			MemoryBuffer onixDll = Utils::downloadFileInMemory(UrlBuffer);
			if (!onixDll) {
				Logger::log("Unable to download the new version.", Logger::Level::Warn);
				if (throwMessageBoxes)
					Utils::showMessageBox("No Downloads?", "Could not download the client from the github server.");
				CurrentlyUpdatingTheDll = false;
				return 1;
			}
			Logger::log("New dll finished downloading");
			if (!DeleteFileA(Minecraft::getClientDllPath())) {
				Logger::log("Unable to delete old dll file, Killing minecraft", Logger::Level::Warn);
				Minecraft::kill(); //stop using my dll you ***** ** ****
				if (!DeleteFileA(Minecraft::getClientDllPath())) {
					Logger::log("Unable to delete old dll file, update failed.", Logger::Level::Error);
					CurrentlyUpdatingTheDll = false;
					return 1;
				}
			}
			onixDll.writeToFile(Minecraft::getClientDllPath());
			if (!std::filesystem::exists(Minecraft::getClientDllPath())) {
				Logger::log("Could not write new dll to disk.", Logger::Level::Error);
				if (throwMessageBoxes)
					Utils::showMessageBox("Failure?", "Could not save the dll to disk?");
			}
			else {
				Logger::log("New dll is now on disk");
			}
		}
	}
	else {
		Logger::log("No dll found, downloading..", Logger::Level::Info);
		sprintf_s(UrlBuffer, 256, "https://raw.githubusercontent.com/bernarddesfosse/onixclientautoupdate/main/OnixClient.dll?rtticprnawgiyuttpswthrudoingywyutbijdwc=%llu", time(NULL));
		MemoryBuffer onixDll = Utils::downloadFileInMemory(UrlBuffer);
		if (!onixDll) {
			Logger::log("Unable to download the new version.", Logger::Level::Warn);
			CurrentlyUpdatingTheDll = false;
			if (throwMessageBoxes)
				Utils::showMessageBox("No Downloads?", "Could not download the client from the github server.");
			return 1;
		}
		Logger::log("New dll finished downloading");
		onixDll.writeToFile(Minecraft::getClientDllPath());
		if (!std::filesystem::exists(Minecraft::getClientDllPath())) {
			Logger::log("Could not write new dll to disk.", Logger::Level::Error);
			if (throwMessageBoxes)
				Utils::showMessageBox("Failure?", "Could not save the dll to disk?");
		}
		else {
			Logger::log("New dll is now on disk");
		}
	}

	if (std::filesystem::exists(Minecraft::getClientDllPath())) {
		ClientInformation clientInfo = Utils::getClientDllInfo(Minecraft::getClientDllPath());
		lastClientInformation = clientInfo;
		if (clientInfo) {
			bool minecraftVersionSupported = false;
			if (strlen(Minecraft::getPackageVersion()) == 0)
				Logger::log("Unable to get MCBE version!", Logger::Level::Warn);
			else {
				for (const std::string& version : clientInfo.supportedMinecraftPackageVersions) {
					if (version == Minecraft::getPackageVersion()) {
						minecraftVersionSupported = true;
						break;
					}
				}
				if (!minecraftVersionSupported || clientInfo.supportedMinecraftPackageVersions.empty() || clientInfo.supportedMinecraftVersions.empty()) {
					ClientSupportsMinecraftVersion = false;
					Logger::logf("The dll DOES NOT supports your minecraft version (%s or %s)", Logger::Level::Warn, Minecraft::getPackageVersion(), Minecraft::getVersion());
					Logger::logf("Latest supported version for this dll: %s (or %s)", Logger::Level::Warn, clientInfo.supportedMinecraftPackageVersions[0].c_str(), clientInfo.supportedMinecraftVersions[0].c_str());
					Logger::logf("Oldest supported version for this dll: %s (or %s)", Logger::Level::Warn, clientInfo.supportedMinecraftPackageVersions.back().c_str(), clientInfo.supportedMinecraftVersions.back().c_str());
				}
				else {
					Logger::logf("The dll Supports your minecraft version (%s or %s)", Logger::Level::Info, Minecraft::getPackageVersion(), Minecraft::getVersion());
					ClientSupportsMinecraftVersion = true;
				}
			}
			if (std::atof(clientInfo.clientVersion.c_str()) < std::atof(GithubVersion)) {
				Logger::log("Dll is outdated, deleting it. If the dll is new you might want to try again in a bit.", Logger::Level::Error);
				if (!DeleteFileA(Minecraft::getClientDllPath())) {
					Logger::log("Unable to delete dll file, Killing minecraft", Logger::Level::Warn);
					Minecraft::kill(); //stop using my dll you ***** ** ****
					if (!DeleteFileA(Minecraft::getClientDllPath())) {
						Logger::log("Unable to delete dll file...", Logger::Level::Error);
						CurrentlyUpdatingTheDll = false;
						return 1;
					}
				}
			}
			else
				Logger::log("Dll is up to date!");

			if (std::filesystem::exists(Minecraft::getClientDllPath())) {
				TextElement* element = (TextElement*)mainWindow->getElement("OnixClientVersionText");
				if (element && element->type == UIElement::Type::Text) {
					element->text = "Version " + std::string(clientInfo.clientVersion);
					mainWindow->gfx->redraw = true;
				}
			}
		}
		else {
			Logger::log("Unable to get information for the dll.", Logger::Level::Error);
		}
	}
	else {
		Logger::log("No dll on the disk, antivirus deleted?", Logger::Level::Error);
	}
	CurrentlyUpdatingTheDll = false;
	return 1;
}

ClientInformation GetLatestClientInformation() {
	std::string dll = Utils::getTempFile();
	char* UrlBuffer = new char[512];
	sprintf_s(UrlBuffer, 512, "https://raw.githubusercontent.com/bernarddesfosse/onixclientautoupdate/main/OnixClient.dll?rtticprnawgiyuttpswthrudoingywyutbijdwc=%llu", time(NULL));
	MemoryBuffer onixDll = Utils::downloadFileInMemory(UrlBuffer);
	delete[] UrlBuffer;
	if (!onixDll) {
		Logger::log("Unable to download the new version.", Logger::Level::Warn);
		CurrentlyUpdatingTheDll = false;
		return ClientInformation();
	}
	Logger::log("New dll finished downloading");
	DeleteFileA(dll.c_str());
	onixDll.writeToFile(dll.c_str());
	if (!std::filesystem::exists(dll.c_str())) {
		Logger::log("Could not write new dll to disk.", Logger::Level::Error);
	}
	else {
		Logger::log("New dll is now on disk");
	}
	ClientInformation info = Utils::getClientDllInfo(dll.c_str());
	DeleteFileA(dll.c_str());
	return info;
}

std::string DownloadMinecraftVersion(const std::string& packageVersion, std::string& statusLabel, bool& updateWindow, const char* path = nullptr) {
	std::string finalPath;
	if (path)
		finalPath = path;
	else {
		finalPath = Utils::getTempFile();
	}

	statusLabel = "Getting Versions...";
	updateWindow = true;
	MemoryBuffer versionList = Utils::downloadFileInMemory("https://raw.githubusercontent.com/bernarddesfosse/onix_compatible_appx/main/versions.txt");
	if (!versionList) {
		Utils::showMessageBox("Could not download!", "Could not get the list of minecraft versions.");
		return std::string();
	}
	
	std::stringstream ss((char*)versionList.buffer);
	while (ss.peek() != EOF) {
		std::string versionName;
		std::getline(ss, versionName);
		std::string PackageVersion;
		if (ss.peek() == EOF) break;
		std::getline(ss, PackageVersion);
		std::string versionUrl;
		if (ss.peek() == EOF) break;
		std::getline(ss, versionUrl);

		if (packageVersion == PackageVersion) {
			struct BloatedDownloadProgressionStructure : public IBindStatusCallback {

				std::string& updateLabel;
				bool& updateWindow;
				virtual HRESULT STDMETHODCALLTYPE OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, __RPC__in_opt LPCWSTR szStatusText) override {
					if (ulStatusCode != tagBINDSTATUS::BINDSTATUS_DOWNLOADINGDATA) return S_OK;
					if (ulProgress == 0 || ulProgressMax == 0) return S_OK;
					double progress = ((double)ulProgress / (double)ulProgressMax) * 100.0;
					updateWindow = false;
					std::string prog = "Downloading... " + std::to_string((int)progress) + '.' + std::to_string((int)((progress - (double)(int)progress) * 10.0)) + '%';
					memset(updateLabel.data(),(int) updateLabel.capacity(), 0);
					sprintf_s(updateLabel.data(), (int)updateLabel.capacity(), "%s", prog.c_str());
					//updateLabel = "Downloading... " + std::to_string((int)(((double)ulProgress / (double)ulProgressMax) * 100.0)) + '%';
					updateWindow = true;
					return S_OK;
				}

				UINT refcount = 1;
				BloatedDownloadProgressionStructure(std::string& UpdateLabel, bool& UpdateWindow) : updateLabel(UpdateLabel), updateWindow(UpdateWindow)
				{}
				
				virtual HRESULT STDMETHODCALLTYPE OnStartBinding(DWORD dwReserved, __RPC__in_opt IBinding* pib) { return E_NOTIMPL; }
				virtual HRESULT STDMETHODCALLTYPE GetPriority(__RPC__out LONG* pnPriority) { return E_NOTIMPL; }
				virtual HRESULT STDMETHODCALLTYPE OnLowResource(DWORD reserved) { return E_NOTIMPL; }
				virtual HRESULT STDMETHODCALLTYPE OnStopBinding(HRESULT hresult, __RPC__in_opt LPCWSTR szError) { return E_NOTIMPL; }
				virtual HRESULT STDMETHODCALLTYPE GetBindInfo(DWORD* grfBINDF,BINDINFO* pbindinfo) {return E_NOTIMPL;}
				virtual HRESULT STDMETHODCALLTYPE OnDataAvailable(DWORD grfBSCF,DWORD dwSize,FORMATETC* pformatetc,STGMEDIUM* pstgmed) {return E_NOTIMPL;}
				virtual HRESULT STDMETHODCALLTYPE OnObjectAvailable(__RPC__in REFIID riid, __RPC__in_opt IUnknown* punk) { return E_NOTIMPL;}
				virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) { return E_NOTIMPL; }
				virtual ULONG   STDMETHODCALLTYPE AddRef(void)  { return ++refcount; }
				virtual ULONG   STDMETHODCALLTYPE Release(void) { return --refcount; }
			};

			statusLabel = "Starting download...";
			updateWindow = true;
			BloatedDownloadProgressionStructure progressCallback(statusLabel, updateWindow);
			if (URLDownloadToFileA(0, versionUrl.c_str(), finalPath.c_str(), 0, &progressCallback) != S_OK) return std::string();
			statusLabel = "Downloaded!";
			updateWindow = true;
			return finalPath;
		}
	}
	return std::string();
}

bool MinecraftInstalled(bool installLatest) {
	ClientInformation clientinfo = lastClientInformation;
	if (!lastClientInformation || installLatest) //so we download latest supported
		clientinfo = GetLatestClientInformation();
	//game not installed
	while ((clientinfo && Minecraft::getPackageVersion()[0] == '\0') || installLatest) {
		bool supportsVersion = false;
		if (!installLatest) {
			for (const std::string& version : clientinfo.supportedMinecraftPackageVersions)
				if (version == Minecraft::getPackageVersion()) {
					supportsVersion = true;
					break;
				}
		}
		else //this list is sorted so front will be latest supported
			supportsVersion = clientinfo.supportedMinecraftPackageVersions.front() == Minecraft::getPackageVersion();
		if (supportsVersion) 
			if (installLatest) {
				Utils::showMessageBox("No Changes Needed!", "You are already on the latest version that onix client supports.", "Ok");
				break;
			}
			else
				break;
		
		int result = 1;
		if (!installLatest)
			result = Utils::showMessageBox("No Minecrafts?", "You don't seem to have minecraft, Do you want to install it?", "Yes", "No");

		if (result == 0 || result == 2) break;
		std::string version;
		if (result == 1) { //latest
			version = clientinfo.supportedMinecraftPackageVersions.front();
		}
		else if (result == 2) { //lowest
			version = clientinfo.supportedMinecraftPackageVersions.back();
		}
		mainWindow->elementsLock.lock();
		for (UIElement* elem : mainWindow->elements)
			if (elem->name != "OnixLogo" && elem->name != "OnixClientText")
				elem->visible = elem->enabled = false;
		TextElement& updateElement = (TextElement&)mainWindow->addUiElement(new TextElement);
		if (Options::lightTheme)
			updateElement.color = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);
		else
			updateElement.color = D2D1::ColorF(1.f, 1.f, 1.f, 1.f);
		updateElement.posX = 50.f;
		updateElement.posY = 300.f;
		updateElement.fontSize = 24.f;
		updateElement.text = "Installing Minecraft..., please wait.";
		updateElement.text.reserve(256);
		updateElement.name = "UpdateStatusText";
		updateElement.name.reserve(256);
		updateElement.visible = true;
		updateElement.enabled = true;
		mainWindow->elementsLock.unlock();

		std::string appxPath = DownloadMinecraftVersion(version, updateElement.text, (bool&)mainWindow->gfx->redraw);



		if (!Minecraft::installAppx(appxPath, updateElement.text, (bool&)mainWindow->gfx->redraw))
			Utils::showMessageBox("Failed to install Minecraft", "Minecraft installation failed, sorry.");

		updateElement.text = "Cleaning up...";
		mainWindow->gfx->redraw = true;
		DeleteFileA(appxPath.c_str());

		for (UIElement* elem : mainWindow->elements)
			elem->visible = elem->enabled = true;
		mainWindow->elementsLock.lock();
		mainWindow->elements.erase(std::remove_if(mainWindow->elements.begin(), mainWindow->elements.end(), [](UIElement* elem) { return elem->name == "UpdateStatusText"; }), mainWindow->elements.end());
		mainWindow->elementsLock.unlock();
		mainWindow->gfx->redraw = true;
		break;
	}


	//using a while loop instead of if to have break;
	//X86 upgrade to x64
	while (clientinfo && Minecraft::isX86() && Minecraft::getPackageVersion()[0] != '\0') {

		int result = Utils::showMessageBox("Incompatible Architecture", "Onix Client only work with Minecraft x64, do you want to install x64?\nWhich x64 version do you want to install?", "Latest Supported", "Same Version", "None");
		if (result == 0 || result == 3) break;
		std::string version;
		if (result == 1) { //latest
			version = clientinfo.supportedMinecraftPackageVersions.front();
		}
		else {
			for (const std::string& packageVersion : clientinfo.supportedMinecraftPackageVersions)
				if (packageVersion == Minecraft::getPackageVersion()) {
					version = packageVersion;
					break;
				}
			if (version.empty()) {
				if (Utils::showMessageBox("Could not find version.", "Your current minecraft version is not compatible with the Client, do you want to install the latest supported one?", "yes", "no") == 1) {
					version = clientinfo.supportedMinecraftPackageVersions.front();
				}
				else
					break;
			}
		}
		mainWindow->elementsLock.lock();
		for (UIElement* elem : mainWindow->elements)
			if (elem->name != "OnixLogo" && elem->name != "OnixClientText")
				elem->visible = elem->enabled = false;
		TextElement& updateElement = (TextElement&)mainWindow->addUiElement(new TextElement);
		if (Options::lightTheme)
			updateElement.color = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);
		else
			updateElement.color = D2D1::ColorF(1.f, 1.f, 1.f, 1.f);
		updateElement.posX = 50.f;
		updateElement.posY = 300.f;
		updateElement.fontSize = 24.f;
		updateElement.text = "Installing Minecraft..., please wait.";
		updateElement.text.reserve(256);
		updateElement.name = "UpdateStatusText";
		updateElement.name.reserve(256);
		updateElement.visible = true;
		updateElement.enabled = true;
		mainWindow->elementsLock.unlock();

		std::string appxPath = DownloadMinecraftVersion(version, updateElement.text, (bool&)mainWindow->gfx->redraw);



		if (!Minecraft::installAppx(appxPath, updateElement.text, (bool&)mainWindow->gfx->redraw))
			Utils::showMessageBox("Failed to install Minecraft", "Minecraft installation failed, sorry.");

		updateElement.text = "Cleaning up...";
		mainWindow->gfx->redraw = true;
		DeleteFileA(appxPath.c_str());

		for (UIElement* elem : mainWindow->elements)
			elem->visible = elem->enabled = true;
		mainWindow->elementsLock.lock();
		mainWindow->elements.erase(std::remove_if(mainWindow->elements.begin(), mainWindow->elements.end(), [](UIElement* elem) { return elem->name == "UpdateStatusText"; }), mainWindow->elements.end());
		mainWindow->elementsLock.unlock();
		mainWindow->gfx->redraw = true;
		break;
	}


	//unsupported version
	while (clientinfo && Minecraft::getPackageVersion()[0] != '\0') {
		bool supportsVersion = false;
		for (const std::string& version : clientinfo.supportedMinecraftPackageVersions)
			if (version == Minecraft::getPackageVersion()) {
				supportsVersion = true;
				break;
			}
		if (supportsVersion) break;
		int result = Utils::showMessageBox("Incompatible Version", "Onix Client does not work with your Minecraft version. Which version do you want to install?", "Latest Supported", "Lowest Supported", "None");
		if (result == 0 || result == 3) break;
		std::string version;
		if (result == 1) { //latest
			version = clientinfo.supportedMinecraftPackageVersions.front();
		}
		else if (result == 2) { //lowest
			version = clientinfo.supportedMinecraftPackageVersions.back();
		}
		mainWindow->elementsLock.lock();
		for (UIElement* elem : mainWindow->elements)
			if (elem->name != "OnixLogo" && elem->name != "OnixClientText")
				elem->visible = elem->enabled = false;
		TextElement& updateElement = (TextElement&)mainWindow->addUiElement(new TextElement);
		if (Options::lightTheme)
			updateElement.color = D2D1::ColorF(0.f, 0.f, 0.f, 1.f);
		else
			updateElement.color = D2D1::ColorF(1.f, 1.f, 1.f, 1.f);
		updateElement.posX = 50.f;
		updateElement.posY = 300.f;
		updateElement.fontSize = 24.f;
		updateElement.text = "Installing Minecraft..., please wait.";
		updateElement.text.reserve(256);
		updateElement.name = "UpdateStatusText";
		updateElement.name.reserve(256);
		updateElement.visible = true;
		updateElement.enabled = true;
		mainWindow->elementsLock.unlock();

		std::string appxPath = DownloadMinecraftVersion(version, updateElement.text, (bool&)mainWindow->gfx->redraw);



		if (!Minecraft::installAppx(appxPath, updateElement.text, (bool&)mainWindow->gfx->redraw))
			Utils::showMessageBox("Failed to install Minecraft", "Minecraft installation failed, sorry.");

		updateElement.text = "Cleaning up...";
		mainWindow->gfx->redraw = true;
		DeleteFileA(appxPath.c_str());

		for (UIElement* elem : mainWindow->elements)
			elem->visible = elem->enabled = true;
		mainWindow->elementsLock.lock();
		mainWindow->elements.erase(std::remove_if(mainWindow->elements.begin(), mainWindow->elements.end(), [](UIElement* elem) { return elem->name == "UpdateStatusText"; }), mainWindow->elements.end());
		mainWindow->elementsLock.unlock();
		mainWindow->gfx->redraw = true;
		break;
	}
	return Minecraft::getPackageVersion()[0] != '\0';
}

/*bool InstallFontResource() { // get font going
    bool firstFontGamed = true;
    {
        HRSRC resourceIdentifierHandle = FindResource(0, MAKEINTRESOURCE(IDR_TTF1), L"TTF"); // bold
        assert(resourceIdentifierHandle != 0);
        if (!resourceIdentifierHandle) return false;
        HGLOBAL loadedResource = LoadResource(GetModuleHandleA(NULL), resourceIdentifierHandle);
        assert(loadedResource != 0);
        if (!loadedResource) return false;
        DWORD resourceSize = SizeofResource(GetModuleHandleA(NULL), resourceIdentifierHandle);
        void* resource = LockResource(loadedResource);
        assert(resource != 0);
        if (!resource) return false;
        DWORD installedFonts = 0;
        AddFontMemResourceEx(resource, resourceSize, 0, &installedFonts);
        assert(installedFonts > 0);
        firstFontGamed = installedFonts > 0;
    }

    {
        HRSRC resourceIdentifierHandle = FindResource(0, MAKEINTRESOURCE(IDR_TTF3), L"TTF"); // regular
        assert(resourceIdentifierHandle != 0);
        if (!resourceIdentifierHandle) return false;
        HGLOBAL loadedResource = LoadResource(GetModuleHandleA(NULL), resourceIdentifierHandle);
        assert(loadedResource != 0);
        if (!loadedResource) return false;
        DWORD resourceSize = SizeofResource(GetModuleHandleA(NULL), resourceIdentifierHandle);
        void* resource = LockResource(loadedResource);
        assert(resource != 0);
        if (!resource) return false;
        DWORD installedFonts = 0;
        AddFontMemResourceEx(resource, resourceSize, 0, &installedFonts);
        assert(installedFonts > 0);
        firstFontGamed = firstFontGamed && installedFonts > 0;
    }

    {
        HRSRC resourceIdentifierHandle = FindResource(0, MAKEINTRESOURCE(IDR_TTF2), L"TTF"); // boldnt
        assert(resourceIdentifierHandle != 0);
        if (!resourceIdentifierHandle) return false;
        HGLOBAL loadedResource = LoadResource(GetModuleHandleA(NULL), resourceIdentifierHandle);
        assert(loadedResource != 0);
        if (!loadedResource) return false;
        DWORD resourceSize = SizeofResource(GetModuleHandleA(NULL), resourceIdentifierHandle);
        void* resource = LockResource(loadedResource);
        assert(resource != 0);
        if (!resource) return false;
        DWORD installedFonts = 0;
        AddFontMemResourceEx(resource, resourceSize, 0, &installedFonts);
        assert(installedFonts > 0);
        firstFontGamed = firstFontGamed && installedFonts > 0;
    }

	return firstFontGamed;
}*/

void GetRidOfOtherInstances() {
	DWORD* PidList = new DWORD[4096];
	DWORD damn = 0;
	if (!K32EnumProcesses(PidList, 4096 * sizeof(DWORD), &damn)) {
		//Logger::log("impossible to get processes.");
		delete[] PidList;
		return;
	}
	damn = min((int)damn, 4096);

	wchar_t* myProcessFile = new wchar_t[MAX_PATH];
	wchar_t* textualContent = new wchar_t[MAX_PATH];
	DWORD currentProcessIdentifier = GetCurrentProcessId();
	K32GetModuleFileNameExW(GetCurrentProcess(), GetModuleHandleA(NULL), myProcessFile, MAX_PATH);
	for (DWORD i = 0; i < damn; i++) {
		if (PidList[i] == currentProcessIdentifier) continue;
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, PidList[i]);
		if (hProcess) {
			DWORD cbNeeded;
			HMODULE mod;
			if (!K32EnumProcessModules(hProcess, &mod, sizeof(HMODULE), &cbNeeded)) continue;
			K32GetModuleFileNameExW(hProcess, mod, textualContent, MAX_PATH);
			if (!wcscmp(myProcessFile, textualContent)) {
				delete[] textualContent;
				delete[] myProcessFile;
				delete[] PidList;
				TerminateProcess(hProcess, 0);
				CloseHandle(hProcess);
				return;
			}
			CloseHandle(hProcess);
		}
	}
	delete[] PidList;
	delete[] textualContent;
	delete[] myProcessFile;
}

extern int ImGuiMain();
extern void ShowSettingsWindow();

int CALLBACK wWinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int cmdShow) {
	{
		char szFileName[MAX_PATH * 2];
		GetModuleFileNameA(NULL, szFileName, MAX_PATH * 2);
		int lastBackSlash = -1;
		for (int i = 0; i < sizeof(szFileName) && szFileName[i] != '\0'; i++)
			if (szFileName[i] == '\\' || szFileName[i] == '/')
				lastBackSlash = i;
		if (lastBackSlash >= 0)
			szFileName[lastBackSlash] = 0;
		SetCurrentDirectoryA(szFileName);
	}

	CoInitialize(0);
	srand((uint32_t)time(NULL));
	UIAnimations::init();
	Logger::Init();
	//bool FontsInstalled = InstallFontResource();
	//Logger::logf("Font: %s", Logger::Level::Info, FontsInstalled ? "Installed" : "Could not install");

#ifndef NDEBUG
	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ImGuiMain, 0, 0, 0));
#endif

	HBRUSH WindowColoration = CreateSolidBrush(RGB(77, 255, 53));

	//Creating Class
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(WNDCLASSEX));
	wClass.cbSize = sizeof(WNDCLASSEX);
	wClass.hbrBackground = (HBRUSH)WindowColoration;//(HBRUSH)COLOR_3DDKSHADOW;
	wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wClass.hInstance = hInst;
	wClass.lpfnWndProc = DefWindowProcA;
	wClass.lpszClassName = L"Gaming";
	wClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wClass);

	//Window Size
	RECT rect = { 0,0,600,380 };
	AdjustWindowRectEx(&rect, WS_POPUP | WS_VISIBLE, false, WS_EX_LAYERED);

	//  HWND wHandle = CreateWindowEx(0, L"Gaming", L"Gaming Window", WS_POPUP | WS_VISIBLE | WS_SYSMENU, 100, 100, (rect.right - rect.left), (rect.bottom - rect.top), NULL, NULL, hInstance, 0);
	HWND wHandle = CreateWindowExW(WS_EX_LAYERED, L"Gaming", L"Onix Launcher", WS_POPUP | WS_VISIBLE, (GetSystemMetrics(SM_CXSCREEN) / 2) - ((rect.right - rect.left) / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - ((rect.bottom - rect.top) / 2), (rect.right - rect.left), (rect.bottom - rect.top), NULL, NULL, hInst, 0);
	if (!wHandle) return -1;
	Options::loadOptions(); //for theme setting we have to load it there.
	SetLayeredWindowAttributes(wHandle, RGB(77, 255, 53), 0, LWA_COLORKEY);
	mainWindow = new UIManager;
	mainWindow->closed = [](UIManager* This) -> bool {
		if (!IsInstallingAppx) {
			mainWindow = nullptr;
			PostQuitMessage(0);
		}
		return !IsInstallingAppx;
	};
	mainWindow->fromWindow(wHandle);
	Graphics* windowGfx = mainWindow->gfx;

	mainWindow->render = [](UIManager* This, Graphics* gfx) -> void {
		static BitmapImage* LightModeBackground = nullptr;
		static BitmapImage* DarkModeBackground = nullptr;
		if (Options::lightTheme) {
			if (!LightModeBackground)
				LightModeBackground = gfx->loadBitmap(IDB_PNG6, L"PNG");
		}
		else
			if (!DarkModeBackground)
				DarkModeBackground = gfx->loadBitmap(IDB_PNG1, L"PNG");

		RECT windowRectArea;
		GetClientRect(gfx->winHandle, &windowRectArea);
		gfx->drawBitmap(0, 0, (float)(windowRectArea.right - windowRectArea.left), (float)(windowRectArea.bottom - windowRectArea.top), Options::lightTheme ? LightModeBackground : DarkModeBackground, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
	};


	//DefaultLayout
	static constexpr float FadeInDuration = 0.2f;
	static constexpr float FadeOutDuration = 0.2f;

	{
		UIManager* window = mainWindow;
		window->elementsLock.lock();

		{ //OnixLogo
			ImageElement& element = (ImageElement&)window->addUiElement(new ImageElement);
			element.resourceIndex = (rand() % 100) == 3 ? 10 : 9;
			element.name = "OnixLogo";
			element.posX = (float)47.468;
			element.posY = (float)40.78;
			element.sizeX = (float)66.456;
			element.sizeY = (float)66.456;
		}

		{ //OnixClientText
			TextElement& element = (TextElement&)window->addUiElement(new TextElement);
			element.fontSize = (float)45.197;
			element.bold = true;
			if (Options::lightTheme)
				element.color = { (float)0, (float)0, (float)0, (float)1 };
			else
				element.color = { (float)1, (float)1, (float)1, (float)1 };
			element.text = "ONIX CLIENT";
			element.name = "OnixClientText";
			element.posX = (float)118.143f;
			element.posY = (float)31.646;
			element.sizeX = (float)0;
			element.sizeY = (float)0;
		}

		{ //LauncButton
			RoundRectElement& element = (RoundRectElement&)window->addUiElement(new RoundRectElement);
			element.roundness = (float)16.508;
			element.color = { (float)0, (float)0.681298, (float)0.994475, (float)1 };
			element.name = "LauncButton";
			element.posX = (float)47.468;
			element.posY = (float)129.747;
			element.sizeX = (float)306.962;
			element.sizeY = (float)117.089;

			static ID2D1LinearGradientBrush* linearGradientBrush;
			D2D1_GRADIENT_STOP stops[] = {
				{ 0.0f, D2D1::ColorF(0.f, 0.768627f, 0.984313f) },
				{ 1.0f, D2D1::ColorF(0.f, 0.596078f, 0.980392f) }
			};

			ID2D1GradientStopCollection* collection;

			HRESULT hr = window->gfx->target->CreateGradientStopCollection(stops, _countof(stops), &collection);
			hr = window->gfx->target->CreateLinearGradientBrush(D2D1::LinearGradientBrushProperties(D2D1::Point2F(element.posX, element.posY), D2D1::Point2F(element.posX + element.sizeX, element.posY + element.sizeY)), collection, &linearGradientBrush);
			collection->Release();
			window->gfx->giveObject(linearGradientBrush); //will call release for us when gfx die
			element.erender = [](UIElement* This) {
				Graphics* gfx = This->parent->gfx;

				gfx->target->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(This->posX, This->posY, This->posX + This->sizeX, This->posY + This->sizeY), ((RoundRectElement*)This)->roundness, ((RoundRectElement*)This)->roundness), linearGradientBrush);
			};
			element.emouseEnter = [](UIElement* This) {
				This->parent->currentAnimation = &UIAnimations::LAUNCH_HOVERED;
				This->parent->currentAnimation->start(FadeInDuration, This->parent);
			};
			element.emouseLeave = [](UIElement* This) {
				This->parent->currentAnimation = &UIAnimations::DEFAULT;
				This->parent->currentAnimation->start(FadeOutDuration, This->parent);
			};
			element.eclicked = [](UIElement* This) {
				static bool currentlyOpening = false;
				if (currentlyOpening) return Logger::log("Already injecting!", Logger::Level::Warn);
				currentlyOpening = true;
				CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)([](LPVOID) -> DWORD {

					Logger::log("=================================================");
					Logger::log("Launching game....");

					if (!IsReadyToLaunchTheClient) Logger::log("Not ready for launch, waiting.");
					while (!IsReadyToLaunchTheClient) Sleep(1);
					if (ShouldAvoidLaunching) {
						Logger::log("We have been told not to launch, sorry,", Logger::Level::Warn);
						Utils::showMessageBox("No Launching", "We have been told not to launch.");
						currentlyOpening = false;
						return 1;
					}

					Minecraft::Init();
					if (!MinecraftInstalled(false)) {
						Utils::showMessageBox("No Minecraft", "Minecraft is not installed, we cannot change something that does not exist.");
						Logger::log("no minecraft :/", Logger::Level::Error);
						currentlyOpening = false;
						return 0;
					} else if (Minecraft::isX86()) {
						Utils::showMessageBox("Architecture Error", "You are not on 64-bit/x64 minecraft!");
						Logger::log("Minecraft X86 is not supported.", Logger::Level::Error);
						currentlyOpening = false;
						return 0;
					}

					bool startedGame = false;
					if (!Minecraft::isOpen()) {
						Logger::log("Game is not started, Starting it");
						Minecraft::start();
						startedGame = true;
						if (!Minecraft::waitForStart(2000)) {
							Logger::log("Could not start minecraft bedrock", Logger::Level::Error);
							Utils::showMessageBox("Error", "Could not start Minecraft: Bedrock Edition!\nMaybe try starting manually?");
							currentlyOpening = false;
							return 0;
						}
						else
							Logger::log("Game now started");
					}
					else
						Logger::log("Game is already started");

					Logger::log("Updating DLL....");
					UpdateOnixClientDllFile(true);
					{
						if (!ClientSupportsMinecraftVersion || !std::filesystem::exists(Options::getDllPath())) {
							if (!ClientSupportsMinecraftVersion)
								if (Options::useCustomDll) {
									Logger::log("You dll does not support the minecraft version you have.", Logger::Level::Error);
									Utils::showMessageBox("Error", "The dll does not support your minecraft version!");
								}
								else if (!MinecraftInstalled(false)) {
									Logger::log("The latest version of onix client does not support your minecraft version", Logger::Level::Error);
									Utils::showMessageBox("Error", "The client does not support your minecraft version!");
								}
							if (!std::filesystem::exists(Options::getDllPath())) {
								Logger::log("The dll seems to not exist, either you have no internet or your antivirus deletes it", Logger::Level::Error);
								Utils::showMessageBox("Error", "The file is gone.\nThis is probably due to your antivirus / internet failure, Check logs (Launcher Info) for more info");
							}
							if (startedGame) {
								Minecraft::kill();
								Logger::log("Killing the game as we have no dll for it", Logger::Level::Error);
							}
							currentlyOpening = false;
							return 0;
						}
						else if (lastClientInformation && std::atof(latestGithubVersion.c_str()) < std::atof(lastClientInformation.clientVersion.c_str())) {
							Utils::showMessageBox("Error", "The client is outdated, please try again later.");
							Logger::log("Dll is outdated still, cannot inject");
							currentlyOpening = false;
							return 0;
						}
						Logger::log("=================================================");
						Logger::log("Injecting DLL....");
					}

					Utils::setUWPFileAccessRights(Options::getDllPath());

					std::string errorMessage;
					if (!Minecraft::injectClient(Options::getDllPath(), &errorMessage)) {
						Logger::log("The dll could not be injected!", Logger::Level::Error);
						Utils::showMessageBox("Injection Error", errorMessage.c_str());
					}
					else {
						Logger::log("The dll is injected!", Logger::Level::Info);
						if (Options::hideToTrayAfterLaunch) {
							if (settingsWindow)
								settingsWindow->destroyWindow();
							mainWindow->hideToTray();
							Minecraft::longWaitForClose();
							mainWindow->backFromTray();
						}else if (Options::minimizeAfterLaunch) {
							if (settingsWindow)
								settingsWindow->destroyWindow();
							ShowWindow(mainWindow->getHwnd(), SW_MINIMIZE);
						}
					}

					currentlyOpening = false;
					return 1;
					}), 0, 0, 0));
			};
		}

		{ //LaunchButtonText
			TextElement& element = (TextElement&)window->addUiElement(new TextElement);
			element.fontSize = (float)34.6;
			element.bold = true;
			element.color = { (float)1, (float)1, (float)1, (float)1 };
			element.text = "Launch Client";
			element.name = "LaunchButtonText";
			element.posX = (float)78.059;
			element.posY = (float)152.717;
			element.sizeX = (float)0;
			element.sizeY = (float)0;
		}

		{ //MinecraftVersionText
			TextElement& element = (TextElement&)window->addUiElement(new TextElement);
			element.fontSize = (float)17.542;
			element.bold = false;
			element.color = { (float)0.845304, (float)0.889747, (float)1, (float)1 };
			element.text = "Minecraft Version ?.??.??";
			element.name = "MinecraftVersionText";
			element.posX = (float)79.114;
			element.posY = (float)192.364;
			element.sizeX = (float)0;
			element.sizeY = (float)0;
			Minecraft::onChanged += []() {
				if (!mainWindow) return;
				TextElement* element = (TextElement*)mainWindow->getElement("MinecraftVersionText");
				if (!element) return;
				element->text = std::string("Minecraft Version ") + Minecraft::getVersion();
				Options::saveOptions();
			};
		}

		{ //SettingsButton
			RoundRectElement& element = (RoundRectElement&)window->addUiElement(new RoundRectElement);
			element.roundness = (float)16.766;
			if (Options::lightTheme)
				element.color = { 0.788235f, 0.788235f, 0.788235f, 1.f };
			else
				element.color = { (float)0.302, (float)0.346, (float)0.373, (float)1 };
			element.name = "SettingsButton";
			element.posX = (float)373.418;
			element.posY = (float)129.747;
			element.sizeX = (float)180.38;
			element.sizeY = (float)117.089;
			element.emouseEnter = [](UIElement* This) {
				This->parent->currentAnimation = &UIAnimations::SETTINGS_HOVERED;
				This->parent->currentAnimation->start(FadeInDuration, This->parent);
			};
			element.emouseLeave = [](UIElement* This) {
				This->parent->currentAnimation = &UIAnimations::DEFAULT;
				This->parent->currentAnimation->start(FadeOutDuration, This->parent);
			};
			element.eclicked = [](UIElement* This) {
				ShowSettingsWindow();
			};
		}

		{ //SettingsText
			TextElement& element = (TextElement&)window->addUiElement(new TextElement);
			element.fontSize = (float)32.272;
			element.bold = true;
			element.color = { (float)1, (float)1, (float)1, (float)1 };
			element.text = "Settings";
			element.name = "SettingsText";
			element.posX = (float)401.471;
			element.posY = (float)164.211;
			element.sizeX = (float)0;
			element.sizeY = (float)0;
		}

		{ //LauncherInfoButton
			RoundRectElement& element = (RoundRectElement&)window->addUiElement(new RoundRectElement);
			element.roundness = (float)18.834;
			if (Options::lightTheme)
				element.color = { 0.708235f, 0.708235f, 0.708235f, 1.f };
			else
				element.color = { (float)0.184, (float)0.208, (float)0.22, (float)1 };
			element.name = "LauncherInfoButton";
			element.posX = (float)59.072;
			element.posY = (float)286.617;
			element.sizeX = (float)129.747;
			element.sizeY = (float)34.81;
			element.emouseEnter = [](UIElement* This) {
				This->parent->currentAnimation = &UIAnimations::LAUNCHERINFO_HOVERED;
				This->parent->currentAnimation->start(FadeInDuration, This->parent);
			};
			element.emouseLeave = [](UIElement* This) {
				This->parent->currentAnimation = &UIAnimations::DEFAULT;
				This->parent->currentAnimation->start(FadeOutDuration, This->parent);
			};
			element.eclicked = [](UIElement* This) {
				static bool ConsoleShowingRn = false;
				if (ConsoleShowingRn)
					Logger::HideConsole();
				else
					Logger::ShowConsole();
				ConsoleShowingRn = !ConsoleShowingRn;
			};
		}

		{ //LauncherInfoText
			TextElement& element = (TextElement&)window->addUiElement(new TextElement);
			element.fontSize = (float)17.542;
			element.bold = false;
			element.color = { (float)1, (float)1, (float)1, (float)1 };
			element.text = "Launcher Info";
			element.name = "LauncherInfoText";
			element.posX = (float)70.588;
			element.posY = (float)291.169;
			element.sizeX = (float)0;
			element.sizeY = (float)0;
		}

		{ //DiscordInvite
			ImageElement& element = (ImageElement&)window->addUiElement(new ImageElement);
			element.resourceIndex = Options::lightTheme ? 14 : 5;
			element.name = "DiscordInvite";
			element.posX = (float)506.329;
			element.posY = (float)284.81;
			element.sizeX = (float)47.468;
			element.sizeY = (float)34.81;
			element.emouseEnter = [](UIElement* This) {
				This->parent->currentAnimation = &UIAnimations::DISCORDINVITE_HOVERED;
				This->parent->currentAnimation->start(FadeInDuration, This->parent);
			};
			element.emouseLeave = [](UIElement* This) {
				This->parent->currentAnimation = &UIAnimations::DEFAULT;
				This->parent->currentAnimation->start(FadeOutDuration, This->parent);
			};
			element.eclicked = [](UIElement* This) {
				MemoryBuffer discordUrlDownload = Utils::downloadFileInMemory("https://raw.githubusercontent.com/bernarddesfosse/onixclientautoupdate/main/discord.txt");
				if (discordUrlDownload) {
					char* discordUrl = (char*)malloc(discordUrlDownload.size + 16);
					if (!discordUrl)
						return (void)MessageBoxA(0, "Not enough memory!", "Failed to get discord invite.", MB_OK | MB_ICONERROR);
					memcpy(discordUrl + 13, discordUrlDownload.buffer, discordUrlDownload.size);
					memcpy(discordUrl, "explorer.exe ", 13);
					discordUrl[discordUrlDownload.size + 13] = 0;
					system(discordUrl);
					free(discordUrl);
				}
				else
					MessageBoxA(0, "Hey!\nCould not get the invite, check internet?/github is down?, sorry!", "Failed to get discord invite.", MB_OK | MB_ICONERROR);
			};
		}

		{ //OnixClientVersionText
			TextElement& element = (TextElement&)window->addUiElement(new TextElement);
			element.fontSize = (float)16.766;
			element.bold = false;
			if (Options::lightTheme)
				element.color = { 1.f - 0.731388f, 1.f - 0.773333f, 1.f - 0.812155f, (float)1 };
			else
				element.color = { (float)0.731388, (float)0.773333, (float)0.812155, (float)1 };
			element.text = "Version ?.??";
			element.name = "OnixClientVersionText";
			element.posX = (float)122.307;
			element.posY = (float)88.608;
			element.sizeX = (float)0;
			element.sizeY = (float)0;
		}

		{ //CloseButton
			ImageElement& element = (ImageElement&)window->addUiElement(new ImageElement);
			element.resourceIndex = Options::lightTheme ? 12 : 8;
			element.name = "CloseButton";
			element.posX = (float)556.962;
			element.posY = (float)18.987;
			element.sizeX = (float)22.152;
			element.sizeY = (float)22.152;
			element.emouseEnter = [](UIElement* This) {
				This->parent->currentAnimation = &UIAnimations::CLOSEBUTTON_HOVERED;
				This->parent->currentAnimation->start(FadeInDuration, This->parent);
			};
			element.emouseLeave = [](UIElement* This) {
				This->parent->currentAnimation = &UIAnimations::DEFAULT;
				This->parent->currentAnimation->start(FadeOutDuration, This->parent);
			};
			element.eclicked = [](UIElement* This) {
				Logger::HideConsole();
				PostQuitMessage(0);
			};
		}

		{ //MinimizeButton
			ImageElement& element = (ImageElement&)window->addUiElement(new ImageElement);
			element.resourceIndex = Options::lightTheme ? 13 : 7;
			element.name = "MinimizeButton";
			element.posX = (float)522.152;
			element.posY = (float)18.987;
			element.sizeX = (float)22.152;
			element.sizeY = (float)22.152;
			element.emouseEnter = [](UIElement* This) {
				This->parent->currentAnimation = &UIAnimations::HIDEBUTTON_HOVERED;
				This->parent->currentAnimation->start(FadeInDuration, This->parent);
			};
			element.emouseLeave = [](UIElement* This) {
				This->parent->currentAnimation = &UIAnimations::DEFAULT;
				This->parent->currentAnimation->start(FadeOutDuration, This->parent);
			};
			element.eclicked = [](UIElement* This) {
				ShowWindow(This->parent->getHwnd(), SW_MINIMIZE);
			};
		}

		Options::onThemeChanged += [](bool isLightTheme) {
			{
				TextElement* element = (TextElement*)mainWindow->getElement("OnixClientVersionText");
				if (element)
					if (isLightTheme)
						element->color = { 1.f - 0.731388f, 1.f - 0.773333f, 1.f - 0.812155f, (float)1 };
					else
						element->color = { (float)0.731388, (float)0.773333, (float)0.812155, (float)1 };
			}
			{
				TextElement* element = (TextElement*)mainWindow->getElement("OnixClientText");
				if (element)
					if (isLightTheme)
						element->color = { (float)0, (float)0, (float)0, (float)1 };
					else
						element->color = { (float)1, (float)1, (float)1, (float)1 };
			}
			{
				RoundRectElement* element = (RoundRectElement*)mainWindow->getElement("LauncherInfoButton");
				if (element)
					if (isLightTheme)
						element->color = { 0.708235f, 0.708235f, 0.708235f, 1.f };
					else
						element->color = { (float)0.184, (float)0.208, (float)0.22, (float)1 };
			}
			{
				RoundRectElement* element = (RoundRectElement*)mainWindow->getElement("SettingsButton");
				if (element)
					if (isLightTheme)
						element->color = { 0.788235f, 0.788235f, 0.788235f, 1.f };
					else
						element->color = { (float)0.302, (float)0.346, (float)0.373, (float)1 };
			}
			{
				ImageElement* element = (ImageElement*)mainWindow->getElement("CloseButton");
				if (element) element->resourceIndex = isLightTheme ? 12 : 8;
			}
			{
				ImageElement* element = (ImageElement*)mainWindow->getElement("MinimizeButton");
				if (element) element->resourceIndex = isLightTheme ? 13 : 7;
			}
			{
				ImageElement* element = (ImageElement*)mainWindow->getElement("DiscordInvite");
				if (element) element->resourceIndex = isLightTheme ? 14 : 5;
			}
			
			mainWindow->gfx->redraw = true;
		};

		window->elementsLock.unlock();
	}
	//icon
	HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	assert(hIcon != 0);
	SendMessage(wHandle, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)([](LPVOID) -> DWORD {
		Minecraft::Init();
		GetRidOfOtherInstances();
		//launcher update
		{
			DeleteFileA("OnixClientLauncherUpdater.bat");
			MemoryBuffer onixLauncherLatestVersion = Utils::downloadFileInMemory("https://raw.githubusercontent.com/bernarddesfosse/onixclientautoupdate/main/LatestLauncherVersion");
			if (!onixLauncherLatestVersion) {
				Logger::log("Unable to check for launcher updates!", Logger::Level::Warn);
				return 1;
			}
			char GithubVersion[48] = { 0 };
			memcpy_s(GithubVersion, sizeof(GithubVersion), onixLauncherLatestVersion.buffer, onixLauncherLatestVersion.size);
			{ //   \n removal
				for (int i = 0; i < 48; i++) {
					if (GithubVersion[i] == '\0') break;
					if (GithubVersion[i] == '\n') { GithubVersion[i] = '\0'; break; };
					if (GithubVersion[i] == '\r') { GithubVersion[i] = '\0'; break; };
				}
			}
			if (std::atof(LAUNCHER_VERSION) < std::atof(GithubVersion)) {
				ShouldAvoidLaunching = true;
				Logger::log("Found a new launcher version, downloading...");
				MemoryBuffer onixLauncherFile = Utils::downloadFileInMemory("https://github.com/bernarddesfosse/onixclientautoupdate/raw/main/OnixLauncher.exe");

				for (UIElement* elem : mainWindow->elements)
					if (elem->name != "OnixLogo" && elem->name != "OnixClientText")
						elem->visible = elem->enabled = false;
				TextElement& updateElement = (TextElement&)mainWindow->addUiElement(new TextElement);
				if (Options::lightTheme)
					updateElement.color = D2D1::ColorF(0.f,0.f,0.f,1.f);
				else
					updateElement.color = D2D1::ColorF(1.f,1.f,1.f,1.f);
				updateElement.posX = 50.f;
				updateElement.posY = 300.f;
				updateElement.fontSize = 24.f;
				updateElement.text = "Updating Onix Launcher, please wait.";
				updateElement.text.reserve(256);
				updateElement.name = "UpdateText";
				updateElement.name.reserve(256);
				updateElement.visible = true;
				updateElement.enabled = true;
				mainWindow->gfx->redraw = true;

				if (onixLauncherFile) {
					Logger::log("Finished downloading the new launcher.");
					onixLauncherFile.writeToFile("OnixLauncherNew.exe");
					if (std::filesystem::exists("OnixLauncherNew.exe")) {
						Logger::log("New launcher is now on disk");

						std::ofstream out("OnixClientLauncherUpdater.bat");
						char* updateCode = (char*)malloc(4096);
						if (out.is_open()) {
							constexpr const char* updateCodeFormat =
								R"(%c%c
cd %s
taskkill /f /im %s
timeout 1
del /Q /S %s || cmd.exe %s

move %s %s
start %s
start /min cmd.exe /c timeout 1 && del /Q /S %s &
)";
							char szFileName[MAX_PATH * 2];
							GetModuleFileNameA(NULL, szFileName, MAX_PATH * 2);
							char* szExecutableName = szFileName;
							for (int i = 0; szFileName[i] != '\0'; i++)
								if (szFileName[i] == '\\' || szFileName[i] == '/')
									szExecutableName = szFileName + i + 1;

							char szWorkingDirectory[MAX_PATH * 2];
							char szNewLauncherPath[MAX_PATH * 2];
							char szUpdaterPath[MAX_PATH * 2];
							GetCurrentDirectoryA(MAX_PATH * 2, szWorkingDirectory);
							strcpy_s(szNewLauncherPath, MAX_PATH * 2, szWorkingDirectory);
							strcat_s(szNewLauncherPath, MAX_PATH * 2, "\\OnixLauncherNew.exe");
							szUpdaterPath[0] = '/';
							szUpdaterPath[1] = 'c';
							szUpdaterPath[2] = ' ';
							strcpy_s(szUpdaterPath + 3, MAX_PATH * 2 - 3, szWorkingDirectory);
							strcat_s(szUpdaterPath, MAX_PATH * 2, "\\OnixClientLauncherUpdater.bat");


							sprintf_s(updateCode, 4096, updateCodeFormat, szWorkingDirectory[0], szWorkingDirectory[1], szWorkingDirectory, szExecutableName, szFileName, szUpdaterPath, szNewLauncherPath, szFileName, szFileName, szUpdaterPath + 3);
							out.write(updateCode, strlen(updateCode));
							out.close();
							STARTUPINFOA startInfo;
							memset(&startInfo, 0, sizeof(startInfo));
							startInfo.cb = sizeof(startInfo);
							startInfo.wShowWindow = SW_HIDE;
							PROCESS_INFORMATION procInfo;

							char explorerDir[MAX_PATH * 2] = { 0 };

							GetWindowsDirectoryA(explorerDir, MAX_PATH * 2);
							strcat_s(explorerDir, MAX_PATH * 2, "\\System32\\cmd.exe");

							if (!CreateProcessA(explorerDir, updateCode, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NEW_PROCESS_GROUP, NULL, szWorkingDirectory, &startInfo, &procInfo))
								Logger::log("Could not start update script", Logger::Level::Error);
							else {
								CloseHandle(procInfo.hProcess);
								CloseHandle(procInfo.hThread);
							}
							PostQuitMessage(0);
							IsReadyToLaunchTheClient = true;
							return 1;
						}
						Logger::log("Could not write update script to disk.", Logger::Level::Error);
					}
					else {
						Logger::log("Could not write new launcher to disk.", Logger::Level::Error);
					}
				}
				else
					Logger::log("Could not download the new launcher update.", Logger::Level::Warn);
				PostQuitMessage(0);
				IsReadyToLaunchTheClient = true;

				return 1;
			}
		}

		if (std::filesystem::exists(Minecraft::getMinecraftPath())) {
			UpdateOnixClientDllFile(false);
			MinecraftInstalled(false);
		} else {
			MinecraftInstalled(false);
			UpdateOnixClientDllFile(false);
		}
		
		IsReadyToLaunchTheClient = true;

		Logger::log("Starting RPC");
		CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DiscordManager::Start, 0, 0, 0));
		return 1;
	}), 0, 0, 0));

    ShowWindow(wHandle, cmdShow);
    SetCursor(wClass.hCursor);
    MSG message;
    message.message = WM_NULL;



	SetWindowTextA(wHandle, "Onix Launcher");
    while (GetMessage(&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
	DiscordManager::wantDeath = true;
	if (settingsWindow) {
		ShowWindow(settingsWindow->getHwnd(), SW_HIDE);
		settingsWindow->destroyWindow();
	}
	if (mainWindow) {
		ShowWindow(mainWindow->getHwnd(), SW_HIDE);
		mainWindow->destroyWindow();
	}
	DeleteObject(WindowColoration);

	Options::saveOptions();
	while (!DiscordManager::isDead != true);
	while (IsInstallingAppx) Sleep(1);

    UnregisterClass(wClass.lpszClassName, hInst);
	Logger::deinit();
	DiscordManager::Deinit();
    return 0;
}