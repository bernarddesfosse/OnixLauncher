#pragma once
#include <Windows.h>
#include <unordered_set>
#include <mutex>
#include "Graphics.h"
#include "resource.h"

struct UIElement {
    enum class Type {
        Base,
        Image,
        Text,
        RoundRect,
        SettingsImage,
    };


    float posX = 0, posY = 0;
    float sizeX = 50, sizeY = 50;
    bool visible = true, enabled = true;
    Type type = Type::Base;
    std::string name;

    void(*eclicked)(UIElement* This) = [](UIElement* This) {};
    void(*emouseEnter)(UIElement* This) = [](UIElement* This) {};
    void(*emouseLeave)(UIElement* This) = [](UIElement* This) {};
    void(*erender)(UIElement* This) = [](UIElement* This) {};

    virtual void clicked() { eclicked(this); }
    virtual void mouseEnter() { emouseEnter(this); }
    virtual void mouseLeave() { emouseLeave(this); }
    virtual void render(float delta) { erender(this); }

    class UIManager* parent = nullptr;

    bool operator==(const UIElement& other) const { return this->name == other.name; }
    bool operator!=(const UIElement& other) const { return this->name != other.name; }
};


class ImageElement : public UIElement {
public:
    int resourceIndex = 1;
    int lastResourceIndex = 0;
    BitmapImage* selfimg = nullptr;

    ImageElement() {
        type = Type::Image;
    }

    virtual void render(float dt) override;
};

class RoundRectElement : public UIElement {
public:
    float roundness = 5.f;
    D2D1_COLOR_F color = D2D1::ColorF(1.f, 1.f, 1.f, 1.f);

    RoundRectElement() {
        type = Type::RoundRect;
    }

    virtual void render(float dt) override;
};

class TextElement : public UIElement {
public:
    std::string text;
    D2D1_COLOR_F color = D2D1::ColorF(1.f, 1.f, 1.f, 1.f);
    float fontSize = 30;
    bool bold = false;

    TextElement() {
        type = Type::Text;
    }

    virtual void render(float dt) override;

};
class SettingsImageElement : public UIElement {
public:
	int resourceIndex = 1;
	int lastResourceIndex = 0;
	BitmapImage* selfimg = nullptr;
	float elapsedTime = 0.f;
	float requestedTime = 0.2f;
	bool playing = false;
	float grow = 0.5f;

	bool toggleSelfWaste;
	bool* togglePtr = &toggleSelfWaste;
	std::wstring textualContent;

	SettingsImageElement() {
		type = Type::SettingsImage;
	}

    void makeButton();
    void makeCheckbox();

	virtual void mouseEnter() override;
	virtual void mouseLeave() override;

    virtual void render(float dt) override;
};














struct UIAnimation {
    float requestedTime = 0.f;
    float elapsedTime = 0.f;
    bool finished = false;
    const char* name = "HeyLazyNameMe";

    float(*modifier)(float x) = [](float x) -> float { return x * x; };

    virtual void start(float Seconds, class UIManager* WindowManager) = 0;
    virtual void play(float deltaTime, class UIManager* manager) = 0;
    bool isFinished() { 
        return this==nullptr || finished; 
    }
};

struct LayoutAnimation : public UIAnimation {
    struct PrevState {
        float posX = 0.f, posY = 0.f;
        float sizeX = 0.f, sizeY = 0.f;
        float fontSize = 0.f;
    };
    struct Data {
        std::string elementName;
        float posX = 0.f, posY = 0.f;
        float sizeX = 0.f, sizeY = 0.f;
        float fontSize = 0.f;
        PrevState* previousState = nullptr;
    };
    std::vector<LayoutAnimation::Data> actions;
    std::vector<LayoutAnimation::PrevState> prevStates;

    float(*modifier)(float x) = [](float x) -> float { return x * x; };

    virtual void start(float Seconds, class UIManager* WindowManager) override;
    virtual void play(float deltaTime, class UIManager* manager) override;
};




class UIManager {
    HWND hwnd;
    NOTIFYICONDATA* trayIconData = nullptr;
public:
    std::mutex elementsLock;
    std::vector<UIElement*> elements;
    UIElement* mouseInside = nullptr;
    UIAnimation* currentAnimation;
    Graphics* gfx = nullptr;

    UIManager() = default;
    bool fromWindow(HWND window);

    UIElement* getElement(const std::string& name);
    char userdata[32];

    //events
    bool(*closed)(UIManager* This) = [](UIManager* This) { return false; };
    void(*render)(UIManager* This, Graphics* gfx) = [](UIManager* This, Graphics* gfxx) -> void {gfxx->clear(0, 0, 0); };

    LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    static LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    operator HWND() { return getHwnd(); }
    HWND getHwnd() { return hwnd; }

    void destroyWindow(); //this will make your pointer invalid
    void hideToTray();
    void backFromTray();

    UIElement& addUiElement(UIElement* elem) { elem->parent = this; elements.emplace_back(elem); return *elem; }
    UIElement& addUiElement() { UIElement* elem = new UIElement; elem->parent = this; elements.emplace_back(elem); return *elem; }
    UIElement& addUiElement(const std::string& name) { UIElement* elem = new UIElement; elem->parent = this; elem->name = name; elements.emplace_back(elem); return *elem; }
};

namespace std {
    template<>
    struct hash<UIElement> {
        size_t operator()(const UIElement& elem) const {
            return hash<std::string>()(elem.name);
        }
    };
    inline std::string to_string(UIElement::Type type) {
        switch (type) {
        case UIElement::Type::Base:
            return "Base";
        case UIElement::Type::Image:
            return "Image";
        case UIElement::Type::Text:
            return "Text";
        case UIElement::Type::RoundRect:
            return "RoundRect";
        default:
            return "Unknown";
        }
    }
}