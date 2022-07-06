#include "UIManager.h"
#include "Utils.h"
#include "Options.h"

UIManager* lastMsgBox = nullptr;
extern UIManager* mainWindow;

int ShowGamingMessageBox(const char* titlebar, const char* message, const char* button1, const char* button2, const char* button3) {

    UIManager* messagebox;
    RECT rect = { 0,0,480,200 };
    AdjustWindowRectEx(&rect, WS_POPUP | WS_VISIBLE, false, WS_EX_LAYERED);
    HWND wHandle = CreateWindowExW(WS_EX_LAYERED, L"Gaming", L"Settings", WS_POPUP | WS_VISIBLE, (GetSystemMetrics(SM_CXSCREEN) / 2) - ((rect.right - rect.left) / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - ((rect.bottom - rect.top) / 2), (rect.right - rect.left), (rect.bottom - rect.top), NULL, NULL, GetModuleHandleA(NULL), 0);
    SetWindowPos(wHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    if (!wHandle) return 0;
    SetLayeredWindowAttributes(wHandle, RGB(77, 255, 53), 0, LWA_COLORKEY);
    messagebox = new UIManager;
    messagebox->closed = [](UIManager* This) -> bool { return true; };
    messagebox->fromWindow(wHandle);
    SetWindowTextA(wHandle, "Message");

    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
    SendMessage(wHandle, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

    Graphics* windowGfx = messagebox->gfx;
   // if (Options::lightTheme)
   //     windowGfx->loadBitmap(IDB_PNG15, L"PNG");
   // else
   //     windowGfx->loadBitmap(IDB_PNG16, L"PNG");
    //Sleep(500);

   
    messagebox->render = [](UIManager* This, Graphics* gfx) -> void {
        gfx->clear(0.f, 0.f, 0.f);
        RECT windowRectArea;
        GetClientRect(gfx->winHandle, &windowRectArea);

        static BitmapImage* image = nullptr;
        static Graphics* lastGfx = nullptr;
        if (lastGfx != gfx || !image || !image->bmp) {
            lastGfx = gfx;
            image = nullptr;
            if (Options::lightTheme)
                image = gfx->loadBitmap(IDB_PNG15, L"PNG");
            else
                image = gfx->loadBitmap(IDB_PNG16, L"PNG");
        }

       
        gfx->drawBitmap(0, 0, (float)(windowRectArea.right - windowRectArea.left), (float)(windowRectArea.bottom - windowRectArea.top), 0);
       //if (Options::lightTheme)
       //    gfx->setColor(D2D1::ColorF(1.f, 1.f, 1.f, 1.f));
       //else
       //    gfx->setColor(D2D1::ColorF(0.109803f, 0.117647f, 0.133333f));
       // gfx->fillRectangle(0, 0, (float)(windowRectArea.right - windowRectArea.left), (float)(windowRectArea.bottom - windowRectArea.top));
       //
       //if (Options::lightTheme)
       //    gfx->setColor(D2D1::ColorF(0.788235f, 0.788235f, 0.788235f));
       //else
       //    gfx->setColor(D2D1::ColorF(0.301960f, 0.345098f, 0.372549f));
       //gfx->fillRectangle(0, 0, (float)(windowRectArea.right - windowRectArea.left), 40.f);
    };

    bool we_are_done_here = false;
    bool windowClosed = false;
    int returnValue = 0;
    UIManager* window = messagebox;
    *(bool**)&messagebox->userdata[0] = &we_are_done_here;
    *(bool**)&messagebox->userdata[8] = &windowClosed;
    *(int**)&messagebox->userdata[16] = &returnValue;

    window->closed = [](UIManager* This) -> bool {
        if (**(bool**)&This->userdata[0] == false) {
            **(int**)&This->userdata[16] = 0;
            **(bool**)&This->userdata[0] = true;
        }
        **(bool**)&This->userdata[8] = true;
        return true;
    };

    { //CloseButton
        SettingsImageElement& element = (SettingsImageElement&)window->addUiElement(new SettingsImageElement);
        element.resourceIndex = Options::lightTheme ? 12 : 8;
        element.name = "CloseButton";
        element.posX = (float)445.176;
        element.posY = (float)9.579;
        element.sizeX = (float)20.152;
        element.sizeY = (float)20.152;
        element.eclicked = [](UIElement* This) {
            ShowWindow(This->parent->getHwnd(), SW_HIDE);
            This->parent->destroyWindow();
        };
    }

    { //TitleText
        TextElement& element = (TextElement&)window->addUiElement(new TextElement);
        element.fontSize = (float)30;
        element.bold = true;
        if (Options::lightTheme)
            element.color = { (float)0, (float)0, (float)0, (float)1 };
        else
            element.color = { (float)1, (float)1, (float)1, (float)1 };
        element.text = titlebar;
        element.name = "TitleText";
        element.posX = (float)10.621;
        element.posY = (float)0;
        element.sizeX = (float)50;
        element.sizeY = (float)50;
    }

    { //MessageText
        TextElement& element = (TextElement&)window->addUiElement(new TextElement);
        element.fontSize = (float)21.471;
        element.bold = false;
        if (Options::lightTheme)
            element.color = { (float)0, (float)0, (float)0, (float)1 };
        else
            element.color = { (float)1, (float)1, (float)1, (float)1 };
        element.text = message;
        element.name = "MessageText";
        element.posX = (float)7.394;
        element.posY = (float)42.475;
        element.sizeX = (float)50;
        element.sizeY = (float)50;
    }

    if (button1) { //FirstButton
        SettingsImageElement& element = (SettingsImageElement&)window->addUiElement(new SettingsImageElement);
        element.resourceIndex = 8;
        element.name = "FirstButton";
        element.posX = (float)321.543;
        element.posY = (float)164.875;
        element.sizeX = (float)147.91;
        element.sizeY = (float)27.451;
        element.grow = 0.1f;
        std::string buttonContent = button1;
        new (&element.textualContent) std::wstring(buttonContent.begin(), buttonContent.end());
        element.makeButton();
        element.eclicked = [](UIElement* This) {
            ShowWindow(This->parent->getHwnd(), SW_HIDE);
            **(int**)&This->parent->userdata[16] = 1;
            **(bool**)&This->parent->userdata[0] = true;
        };
    }
    if (button2) { //SecondButton
        SettingsImageElement& element = (SettingsImageElement&)window->addUiElement(new SettingsImageElement);
        element.resourceIndex = 8;
        element.name = "SecondButton";
        element.posX = (float)163.987;
        element.posY = (float)164.875;
        element.sizeX = (float)147.91;
        element.sizeY = (float)27.451;
        element.grow = 0.1f;
        std::string buttonContent = button2;
        new (&element.textualContent) std::wstring(buttonContent.begin(), buttonContent.end());
        element.makeButton();
        element.eclicked = [](UIElement* This) {
            ShowWindow(This->parent->getHwnd(), SW_HIDE);
            **(int**)&This->parent->userdata[16] = 2;
            **(bool**)&This->parent->userdata[0] = true;
        };
    }
    if (button3) { //ThirdButton
        SettingsImageElement& element = (SettingsImageElement&)window->addUiElement(new SettingsImageElement);
        element.resourceIndex = 8;
        element.name = "ThirdButton";
        element.posX = (float)7.843;
        element.posY = (float)164.875;
        element.sizeX = (float)147.91;
        element.sizeY = (float)27.451;
        element.grow = 0.1f;
        std::string buttonContent = button3;
        new (&element.textualContent) std::wstring(buttonContent.begin(), buttonContent.end());
        element.makeButton();
        element.eclicked = [](UIElement* This) {
            ShowWindow(This->parent->getHwnd(), SW_HIDE);
            **(int**)&This->parent->userdata[16] = 3;
            **(bool**)&This->parent->userdata[0] = true;
        };
    }

    auto ThemeChangedThing = [&messagebox](bool isLight) {
        messagebox->gfx->redraw = true;
    };

    while (lastMsgBox) Sleep(1);
    lastMsgBox = messagebox;
    messagebox->gfx->redraw = true;
    SetForegroundWindow(messagebox->getHwnd());
    while (!we_are_done_here) {
        Sleep(1);
        MSG message;
        if (mainWindow && GetForegroundWindow() == mainWindow->getHwnd())
            SetForegroundWindow(messagebox->getHwnd());
        while (PeekMessageA(&message, messagebox->getHwnd(), 0, 0, 1)) {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }
    }
    lastMsgBox = 0;
    if (!windowClosed) messagebox->destroyWindow();

    return returnValue;
}