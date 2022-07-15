#include "UIManager.h"
#include "Animations.h"
#include "Logger.h"
#include "Options.h"
#include <cassert>	

UIManager* settingsWindow = nullptr;
extern DWORD UpdateOnixClientDllFile(bool);
extern bool IsReadyToLaunchTheClient;
extern std::atomic_bool CurrentlyUpdatingTheDll;
extern bool MinecraftInstalled(bool installLatest);




void ShowSettingsWindow() {
	if (settingsWindow) {
		SetForegroundWindow(settingsWindow->getHwnd());
		return;
	}

	RECT rect = { 0,0,600,380 };
	AdjustWindowRectEx(&rect, WS_POPUP | WS_VISIBLE, false, WS_EX_LAYERED);
	HWND wHandle = CreateWindowExW(WS_EX_LAYERED, L"Gaming", L"Settings", WS_POPUP | WS_VISIBLE, (GetSystemMetrics(SM_CXSCREEN) / 2) - ((rect.right - rect.left) / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - ((rect.bottom - rect.top) / 2), (rect.right - rect.left), (rect.bottom - rect.top), NULL, NULL, GetModuleHandleA(NULL), 0);
	if (!wHandle) return;
	SetLayeredWindowAttributes(wHandle, RGB(77, 255, 53), 0, LWA_COLORKEY);
	settingsWindow = new UIManager;
	settingsWindow->closed = [](UIManager* This) -> bool { settingsWindow = nullptr; return true; };
	settingsWindow->fromWindow(wHandle);
	SetWindowTextA(wHandle, "Settings");
	Graphics* windowGfx = settingsWindow->gfx;

	HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	SendMessage(wHandle, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

	
	settingsWindow->render = [](UIManager* This, Graphics* gfx) -> void {	
		static BitmapImage* LightModeBackground = nullptr;
		static BitmapImage* DarkModeBackground = nullptr;
		static Graphics* OldGfx = nullptr;
		if (gfx != OldGfx) {
			OldGfx = gfx;
			LightModeBackground = nullptr;
			DarkModeBackground = nullptr;
			if (Options::lightTheme) 
				LightModeBackground = gfx->loadBitmap(IDB_PNG11, L"PNG");
			else
				DarkModeBackground  = gfx->loadBitmap(IDB_PNG2, L"PNG");
		}

		if (Options::lightTheme)
			if (!LightModeBackground)
				LightModeBackground = gfx->loadBitmap(IDB_PNG11, L"PNG"); else;
		else
			if (!DarkModeBackground)
				DarkModeBackground = gfx->loadBitmap(IDB_PNG2, L"PNG");

		RECT windowRectArea;
		GetClientRect(gfx->winHandle, &windowRectArea);
		gfx->drawBitmap(0, 0, (float)(windowRectArea.right - windowRectArea.left), (float)(windowRectArea.bottom - windowRectArea.top), Options::lightTheme ? LightModeBackground : DarkModeBackground, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
	};

	UIManager* window = settingsWindow;

	{ //CloseButton
		SettingsImageElement& element = (SettingsImageElement&)window->addUiElement(new SettingsImageElement);
		element.resourceIndex = Options::lightTheme ? 12 : 8;
		element.name = "CloseButton";
		element.posX = (float)556.962;
		element.posY = (float)18.987;
		element.sizeX = (float)22.152;
		element.sizeY = (float)22.152;
		element.eclicked = [](UIElement* This) {
			SendMessageA(This->parent->getHwnd(), WM_CLOSE, 0, 0);
		};
	}

	{ //MinimizeButton
		SettingsImageElement& element = (SettingsImageElement&)window->addUiElement(new SettingsImageElement);
		element.resourceIndex = Options::lightTheme ? 13 : 7;
		element.name = "MinimizeButton";
		element.posX = (float)522.152;
		element.posY = (float)18.987;
		element.sizeX = (float)18.987;
		element.sizeY = (float)22.152;
		element.eclicked = [](UIElement* This) {
			ShowWindow(This->parent->getHwnd(), SW_MINIMIZE);
		};
	}

	{ //HideUsernameFromRpcCheckbox
		SettingsImageElement& element = (SettingsImageElement&)window->addUiElement(new SettingsImageElement);
		element.name = "HideUsernameFromRpcCheckbox";
		element.posX = (float)16.878;
		element.posY = (float)28.481;
		element.sizeX = (float)18.987;
		element.sizeY = (float)22.152;
		element.textualContent = L"Hide Username From RPC";
		element.togglePtr = &Options::hideMinecraftUsername;
		element.makeCheckbox();
		element.grow = 0.f;
		element.requestedTime = 0.f;
	}
	
	{ //HideWorldNamesFromRpcCheckbox
		SettingsImageElement& element = (SettingsImageElement&)window->addUiElement(new SettingsImageElement);
		element.name = "HideWorldNamesFromRpcCheckbox";
		element.posX = (float)16.878;
		element.posY = (float)61.538;
		element.sizeX = (float)18.987;
		element.sizeY = (float)22.152;
		element.textualContent = L"Hide World Name From RPC";
		element.togglePtr = &Options::hideWorldNames;
		element.makeCheckbox();
		element.grow = 0.f;
		element.requestedTime = 0.f;
	}
	
	{ //HideServerIpsFromRpcCheckbox
		SettingsImageElement& element = (SettingsImageElement&)window->addUiElement(new SettingsImageElement);
		element.name = "HideServerIpsFromRpcCheckbox";
		element.posX = (float)339.223;
		element.posY = (float)61.538;
		element.sizeX = (float)18.987;
		element.sizeY = (float)22.152;
		element.textualContent = L"Hide Server IP From RPC";
		element.togglePtr = &Options::hideServerIps;
		element.makeCheckbox();
		element.grow = 0.f;
		element.requestedTime = 0.f;
	}

	{ //DisableRpcCheckbox
		SettingsImageElement& element = (SettingsImageElement&)window->addUiElement(new SettingsImageElement);
		element.name = "DisableRpcCheckbox";
		element.posX = (float)339.223;
		element.posY = (float)28.481;
		element.sizeX = (float)18.987;
		element.sizeY = (float)22.152;
		element.textualContent = L"Disable RPC";
		element.togglePtr = &Options::disableRpc;
		element.makeCheckbox();
		element.grow = 0.f;
		element.requestedTime = 0.f;
	}
	{ //DisableRpcCheckbox
		SettingsImageElement& element = (SettingsImageElement&)window->addUiElement(new SettingsImageElement);
		element.name = "DisableInLauncherRpcCheckbox";
		element.posX = (float)144.223;
		element.posY = (float)96.481;
		element.sizeX = (float)18.987;
		element.sizeY = (float)22.152;
		element.textualContent = L"Disable \"In the Launcher\" RPC";
		element.togglePtr = &Options::noInLauncherRpc;
		element.makeCheckbox();
		element.grow = 0.f;
		element.requestedTime = 0.f;
	}

	{ //SelectMinecraftBedrockEditionDynamicLinkLibraryFile
		SettingsImageElement& element = (SettingsImageElement&)window->addUiElement(new SettingsImageElement);
		element.name = "SelectMinecraftBedrockEditionDynamicLinkLibraryFile";
		element.posX = (float)487.633;
		element.posY = (float)137.676;
		element.sizeX = (float)95.406;
		element.sizeY = (float)28.169;
		element.textualContent = L"Select Dll";
		element.togglePtr = &Options::disableRpc;
		element.makeButton();
		element.grow = 0.05f;
		element.requestedTime = 0.2f;
		element.eclicked = [](UIElement* This) {
			if (CurrentlyUpdatingTheDll) return;
			OPENFILENAMEA ofn{};
			char fileName[MAX_PATH * 2] = "";

			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = This->parent->getHwnd();
			ofn.lpstrFilter = "Dll Files (*.dll)\0*.dll\0";
			ofn.lpstrFile = fileName;
			ofn.nMaxFile = MAX_PATH * 2;
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
			ofn.lpstrDefExt = "";

			if (GetOpenFileNameA(&ofn)) {
				Options::useCustomDll = true;
				Options::customDllPath = fileName;
			}
			else
				Options::useCustomDll = false;
			Options::saveOptions();
			CloseHandle(CreateThread(0,0,(LPTHREAD_START_ROUTINE)UpdateOnixClientDllFile, 0, 0, 0));
			This->parent->gfx->redraw = true;
		};
	}

	{ //SelectedDllPath
		RoundRectElement& element = (RoundRectElement&)window->addUiElement(new RoundRectElement);
		element.roundness = (float)5;
		if (Options::lightTheme)
			element.color = { 1.f - 0.0994475f, 1.f - 0.0994475f, 1.f - 0.0994475f, (float)1 };
		else
			element.color = { (float)0.0994475, (float)0.0994475, (float)0.0994475, (float)1 };
		element.name = "SelectedDllPath";
		element.posX = (float)16.878;
		element.posY = (float)137.75;
		element.sizeX = (float)463.08;
		element.sizeY = (float)28.451;

		element.erender = [](UIElement* This) {
			SettingsImageElement* e = (SettingsImageElement*)This;
			Graphics* gfx = This->parent->gfx;
			IDWriteTextLayout* textLayout;
			if (Options::lightTheme)
				gfx->setColor(D2D1::ColorF(0.f, 0.f, 0.f, 1.f));
			else
				gfx->setColor(D2D1::ColorF(1.f, 1.f, 1.f, 1.f));
			gfx->setFont(L"Segoe UI", 12.f, false);
			std::wstring str(Options::customDllPath.begin(), Options::customDllPath.end());
			gfx->getDwriteFactory()->CreateTextLayout(str.c_str(), (UINT32)str.size(), gfx->getTextFormat(), e->sizeX, e->sizeY, &textLayout);
			DWRITE_TEXT_METRICS metrics;
			textLayout->GetMetrics(&metrics);
			assert(textLayout);
			gfx->target->DrawTextLayout(D2D1::Point2F(e->posX, ((e->sizeY / 2.f) - (metrics.height / 2.f)) + e->posY), textLayout, gfx->getBrush(), D2D1_DRAW_TEXT_OPTIONS::D2D1_DRAW_TEXT_OPTIONS_CLIP);
			textLayout->Release();
		};
	}

	{ //UseCustomDllCheckbox
		SettingsImageElement& element = (SettingsImageElement&)window->addUiElement(new SettingsImageElement);
		element.name = "UseCustomDllCheckbox";
		element.posX = (float)16.878;
		element.posY = (float)169.631;
		element.sizeX = (float)18.987;
		element.sizeY = (float)22.152;
		element.textualContent = L"Use Custom Dll";
		element.togglePtr = &Options::useCustomDll;
		element.makeCheckbox();
		element.grow = 0.f;
		element.requestedTime = 0.f;
		element.eclicked = [](UIElement* This) {
			if (CurrentlyUpdatingTheDll) return;
			SettingsImageElement* e = (SettingsImageElement*)This;
			*e->togglePtr = !*e->togglePtr;
			Options::saveOptions();
			This->parent->gfx->redraw = true;
			CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)UpdateOnixClientDllFile, 0, 0, 0));
		};
	}
	
	{ //LightThemeCheckbox
		SettingsImageElement& element = (SettingsImageElement&)window->addUiElement(new SettingsImageElement);
		element.name = "LightThemeCheckbox";
		element.posX = (float)16.878;
		element.posY = (float)240.538;
		element.sizeX = (float)18.987;
		element.sizeY = (float)22.152;
		element.textualContent = L"Light Theme";
		element.togglePtr = &Options::lightTheme;
		element.makeCheckbox();
		element.grow = 0.f;
		element.requestedTime = 0.f;
		element.eclicked = [](UIElement* This) {
			SettingsImageElement* e = (SettingsImageElement*)This;
			*e->togglePtr = !*e->togglePtr;
			Options::onThemeChanged(Options::lightTheme);
			Options::saveOptions();
			This->parent->gfx->redraw = true;
		};
	}

	{ //HideWorldNamesFromRpcCheckbox
		SettingsImageElement& element = (SettingsImageElement&)window->addUiElement(new SettingsImageElement);
		element.name = "AfterLaunch_ThrowToTray";
		element.posX = (float)16.878;
		element.posY = (float)210.631;
		element.sizeX = (float)18.987;
		element.sizeY = (float)22.152;
		element.textualContent = L"Hide to Tray After Launch";
		element.togglePtr = &Options::hideToTrayAfterLaunch;
		element.makeCheckbox();
		element.grow = 0.f;
		element.requestedTime = 0.f;
		element.eclicked = [](UIElement* This) {
			SettingsImageElement* e = (SettingsImageElement*)This;
			Options::hideToTrayAfterLaunch = !Options::hideToTrayAfterLaunch;
			if (Options::hideToTrayAfterLaunch) Options::minimizeAfterLaunch = false;
			Options::saveOptions();
			This->parent->gfx->redraw = true;
		};
	}

	{ //HideServerIpsFromRpcCheckbox
		SettingsImageElement& element = (SettingsImageElement&)window->addUiElement(new SettingsImageElement);
		element.name = "AfterLaunch_ThrowToTaskbar";
		element.posX = (float)339.223;
		element.posY = (float)210.631;
		element.sizeX = (float)18.987;
		element.sizeY = (float)22.152;
		element.textualContent = L"Minimize after Launch";
		element.togglePtr = &Options::minimizeAfterLaunch;
		element.makeCheckbox();
		element.grow = 0.f;
		element.requestedTime = 0.f;
		element.eclicked = [](UIElement* This) {
			SettingsImageElement* e = (SettingsImageElement*)This;
			Options::minimizeAfterLaunch = !Options::minimizeAfterLaunch;
			if (Options::minimizeAfterLaunch) Options::hideToTrayAfterLaunch = false;
			Options::saveOptions();
			This->parent->gfx->redraw = true;
		};
	}

	{ //UpdateToLatestMinecraftVersion
		SettingsImageElement& element = (SettingsImageElement&)window->addUiElement(new SettingsImageElement);
		element.name = "UpdateToLatestMinecraftVersion";
		element.posX = (float)70.74;
		element.posY = (float)337.621;
		element.sizeX = (float)456.592;
		element.sizeY = (float)35.37;
		element.textualContent = L"Update to latest supported Minecraft: Bedrock Edition version";
		element.togglePtr = &Options::disableRpc;
		element.makeButton();
		element.grow = 0.05f;
		element.requestedTime = 0.2f;
		element.eclicked = [](UIElement* This) {
			if (CurrentlyUpdatingTheDll) return;
			This->parent->destroyWindow();
			CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)[](LPVOID) -> DWORD {
				int result = (Utils::showMessageBox("Install latest supported?", "Are you sure you want to install the latest minecraft version that onix client supports?", "Yes", "No", "Cancel"));
				if (result != 1) return 0;
				bool installed = MinecraftInstalled(true);
				if (!installed)
					Utils::showMessageBox("Failed to install", "The installation failed, sorry.", "Ok");
				return 0;
				}, 0, 0, 0));
		};
	}

	Options::onThemeChanged += [](bool isLightTheme) {
		if (!settingsWindow) return;
		{
			RoundRectElement* element = (RoundRectElement*)settingsWindow->getElement("SelectedDllPath");
			if (element)
				if (isLightTheme)
					element->color = { 1.f-0.0994475f, 1.f - 0.0994475f, 1.f - 0.0994475f, (float)1 };
				else
					element->color = { (float)0.0994475, (float)0.0994475, (float)0.0994475, (float)1 };
		}

		{
			ImageElement* element = (ImageElement*)settingsWindow->getElement("CloseButton");
			if (element) element->resourceIndex = isLightTheme ? 12 : 8;
		}

		{
			ImageElement* element = (ImageElement*)settingsWindow->getElement("MinimizeButton");
			if (element) element->resourceIndex = isLightTheme ? 13 : 7;
		}

		settingsWindow->gfx->redraw = true;
	};

	settingsWindow->gfx->redraw = true;
}


