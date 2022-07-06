#include "UIManager.h"
#include <cassert>
#include "Options.h"
#undef min
#undef max

#include "Logger.h"

static void DrawThread(UIManager* manager) {
	CoInitialize(0);
	std::chrono::steady_clock::time_point timePrevFrame = std::chrono::high_resolution_clock::now();


	while (!manager->gfx->wantsDeath) {
#ifdef NDEBUG
		if (!manager->gfx->redraw && manager->currentAnimation->isFinished()) {
			Sleep(5);
			timePrevFrame = std::chrono::high_resolution_clock::now();
			continue;
		}
#endif
		manager->gfx->beginDraw();
		manager->gfx->redraw = false;

		//timing
 		std::chrono::steady_clock::time_point timeRightNow = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float>(timeRightNow - timePrevFrame).count();
		timePrevFrame = timeRightNow;

		if (!manager->currentAnimation->isFinished()) {
			manager->currentAnimation->play(deltaTime, manager);
		}

		manager->elementsLock.lock();
		manager->render(manager,manager->gfx);
		for (UIElement* elem : manager->elements)
			if (elem->visible)
				elem->render(deltaTime);
		manager->elementsLock.unlock();
		manager->gfx->endDraw();
	}
	Sleep(1);
	manager->gfx->killed = true;
}

bool UIManager::fromWindow(HWND window) {
	hwnd = window;
	SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)this);
	SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)UIManager::WindowProcedure);

	//TRACKMOUSEEVENT evnt;
	//evnt.cbSize = sizeof(TRACKMOUSEEVENT);
	//evnt.dwFlags = TME_LEAVE;
	//evnt.hwndTrack = window;
	//evnt.dwHoverTime = 400;
	//TrackMouseEvent(&evnt);

    gfx = new Graphics;
	bool initialised = false;
	while (!initialised) {
		initialised = gfx->Init(window);
		if (!initialised) {
			delete gfx;
			gfx = new Graphics;
		}
	}
	gfx->redraw = true;

	CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DrawThread, this, 0, 0));
	return initialised;
}

UIElement* UIManager::getElement(const std::string& name) {
	for (UIElement* elem : elements) {
		if (elem->name == name)
			return elem;
	}
	return nullptr;
}


LRESULT UIManager::WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_QUIT || msg == WM_CLOSE) {
		if (closed(this)) {
			ShowWindow(hwnd, SW_HIDE);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)nullptr);
			this->elements.clear();
			gfx->wantsDeath = true;
			while (!gfx->killed);
			delete this->gfx;
			delete this;
			DestroyWindow(hwnd);
		}
		return 0;
	}
	if (msg == WM_NCCALCSIZE) { return 0; }

	switch (msg) {

	case WM_LBUTTONDOWN: {
		POINTS mp = MAKEPOINTS(lparam);
		if (this->mouseInside)
			this->mouseInside->clicked();
		return 0;
	}
	case WM_MOUSEMOVE: {
		
		return 0;
	}
	case WM_NCHITTEST: {
		POINTS mousept = MAKEPOINTS(lparam);
		POINT mouse = { mousept.x, mousept.y };
		bool foundAny = false;
		if (ScreenToClient(hwnd, &mouse)) {
			RECT windowRect;
			if (GetWindowRect(hwnd, &windowRect) && mouse.x > 0 && mouse.y > 0) {
				if (mouse.x < (windowRect.right - windowRect.left) && mouse.y < (windowRect.bottom - windowRect.top)) {
					float mouseX = (float)mouse.x, mouseY = (float)mouse.y;
	
					elementsLock.lock();
					for (UIElement* element : elements) {
						if (element->enabled == false) continue;
						if (mouseX >= element->posX && mouseY >= element->posY && mouseX <= (element->posX + element->sizeX) && mouseY <= (element->posY + element->sizeY)) {
							if (this->mouseInside != element) {
								if (this->mouseInside)
									this->mouseInside->mouseLeave();
	
								this->mouseInside = element;
								this->mouseInside->mouseEnter();
							}
							foundAny = true;
							break;
						}
					}
					elementsLock.unlock();
					if (!foundAny && this->mouseInside) {
						this->mouseInside->mouseLeave();
						this->mouseInside = nullptr;
					}
	
					if (foundAny)
						return HTCLIENT;
					else
						return HTCAPTION;
				}
			}
		}
	}
		break;
	case WM_MOUSELEAVE: {
		if (this->mouseInside) {
			this->mouseInside->mouseLeave();
			this->mouseInside = nullptr;
		}
		return 0; }
	case WM_ACTIVATEAPP: {
		this->gfx->redraw = true;
		return 0; }
	case 2694: {
		if (lparam == WM_LBUTTONDBLCLK) {
			this->backFromTray();
			break;
		}
		
	}

	//case WM_PAINT: return 0;
	}

	//std::string msgstr = std::to_string(msg);
	//OutputDebugStringA(msgstr.c_str());
	//OutputDebugStringA("\n");
	return DefWindowProc(hwnd, msg, wparam, lparam);
}


LRESULT UIManager::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	UIManager* instance = (UIManager*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (!instance) return DefWindowProc(hwnd, msg, wparam, lparam);
	return instance->WinProc(hwnd, msg, wparam, lparam);
	return DefWindowProcA(hwnd, msg, wparam, lparam);
}

void UIManager::destroyWindow() {
	this->WinProc(this->getHwnd(), WM_CLOSE, 0, 0);
}

void UIManager::hideToTray() {
	if (this->trayIconData) return;
	ShowWindow(this->getHwnd(), SW_HIDE);
	this->trayIconData = new NOTIFYICONDATA;
	this->trayIconData->uVersion = NOTIFYICON_VERSION_4;
	this->trayIconData->cbSize = sizeof(*this->trayIconData);
	this->trayIconData->hIcon = LoadIcon(GetModuleHandleA(NULL), MAKEINTRESOURCE(IDI_ICON1));
	this->trayIconData->hWnd = this->getHwnd();
	this->trayIconData->uID = 100;
	this->trayIconData->uCallbackMessage = 2694;
	wcscpy_s(this->trayIconData->szTip, sizeof(this->trayIconData->szTip), L"Click to open the launcher again");
	this->trayIconData->uFlags = NIF_TIP | NIF_ICON | NIF_MESSAGE;
	this->trayIconData->dwInfoFlags = NIIF_NOSOUND;
	Shell_NotifyIcon(NIM_ADD, this->trayIconData);
	//Shell_NotifyIcon(NIM_SETVERSION, this->trayIconData);
}

void UIManager::backFromTray() {
	ShowWindow(this->getHwnd(), SW_SHOW);

	Shell_NotifyIcon(NIM_DELETE, this->trayIconData);
	delete this->trayIconData;
	this->trayIconData = nullptr;
}




void ImageElement::render(float dt) {
	if (resourceIndex != lastResourceIndex || !selfimg) {
		int resultIdx = 0;
		switch (resourceIndex) {
		case 0: resultIdx = IDB_PNG1; break;
		case 1: resultIdx = IDB_PNG1; break;
		case 2: resultIdx = IDB_PNG2; break;
		case 3: resultIdx = IDB_PNG3; break;
		case 4: resultIdx = IDB_PNG4; break;
		case 5: resultIdx = IDB_PNG5; break;
		case 6: resultIdx = IDB_PNG6; break;
		case 7: resultIdx = IDB_PNG7; break;
		case 8: resultIdx = IDB_PNG8; break;
		case 9: resultIdx = IDB_PNG9; break;
		case 10: resultIdx = IDB_PNG10; break;
		case 11: resultIdx = IDB_PNG11; break;
		case 12: resultIdx = IDB_PNG12; break;
		case 13: resultIdx = IDB_PNG13; break;
		case 14: resultIdx = IDB_PNG14; break;
		case 15: resultIdx = IDB_PNG15; break;
		case 16: resultIdx = IDB_PNG16; break;
		default:
			resultIdx = IDB_PNG1;
		}
		if (selfimg) {
			selfimg->bmp->Release();
			selfimg->bmp = 0;
		}
		selfimg = parent->gfx->loadBitmap(resultIdx, L"PNG");
		lastResourceIndex = resourceIndex;
	}

	parent->gfx->drawBitmap(posX, posY, sizeX, sizeY, selfimg);
	erender(this);
}

void RoundRectElement::render(float dt) {
	parent->gfx->setColor(color);
	parent->gfx->fillRoundedRectangle(posX, posY, posX + sizeX, posY + sizeY, roundness);
	erender(this);
}

void TextElement::render(float dt) {
	parent->gfx->setFont(L"Segoe UI", fontSize, bold);
	parent->gfx->setColor(color);
	parent->gfx->drawText(posX, posY, text);
	erender(this);
}



void LayoutAnimation::start(float time, class UIManager* manager) {
	this->requestedTime = time;
	this->elapsedTime = 0.f;
	finished = false;
	prevStates.clear();

	for (const auto& action : this->actions) {
		UIElement* element = manager->getElement(action.elementName);
		if (element) {
			PrevState rn;
			rn.posX = element->posX;
			rn.posY = element->posY;
			rn.sizeX = element->sizeX;
			rn.sizeY = element->sizeY;
			if (element->type == UIElement::Type::Text) {
				TextElement* textelem = (TextElement*)element;
				rn.fontSize = textelem->fontSize;
			}else  if (element->type == UIElement::Type::RoundRect) {
				RoundRectElement* roundRectElement = (RoundRectElement*)element;
				rn.fontSize = roundRectElement->roundness;
			}
			prevStates.emplace_back(rn);
		} else 
			prevStates.emplace_back();
	}

	for (int i = 0; i < (actions.size()); i++) {
		actions[i].previousState = &prevStates[i];
	}

}
#include "Logger.h"
void LayoutAnimation::play(float deltaTime, class UIManager* manager) {
	if (this->finished) return;
	this->elapsedTime += deltaTime;
	if (this->elapsedTime > this->requestedTime) this->elapsedTime = this->requestedTime;
	float progression = this->modifier(this->elapsedTime / this->requestedTime);
	if (progression > 1.f) progression = 1.f;

	for (int i = 0; i < actions.size(); i++) {
		auto& action = this->actions[i];
		UIElement* element = manager->getElement(action.elementName);
		if (!element || !action.previousState) {
			//Logger::logf("Could not find %s", Logger::Level::Warn, action.elementName.c_str());
			continue;
		}
		PrevState& prevState = *action.previousState;

		element->posX = prevState.posX < action.posX ? (((action.posX - prevState.posX) * progression) + prevState.posX) : ((((prevState.posX - action.posX) * (1.f - progression)) + action.posX));
		element->posY = prevState.posY < action.posY ? (((action.posY - prevState.posY) * progression) + prevState.posY) : ((((prevState.posY - action.posY) * (1.f - progression)) + action.posY));
		element->sizeX = prevState.sizeX < action.sizeX ? (((action.sizeX - prevState.sizeX) * progression) + prevState.sizeX) : ((((prevState.sizeX - action.sizeX) * (1.f - progression)) + action.sizeX));
		element->sizeY = prevState.sizeY < action.sizeY ? (((action.sizeY - prevState.sizeY) * progression) + prevState.sizeY) : ((((prevState.sizeY - action.sizeY) * (1.f - progression)) + action.sizeY) );

		if (element->type == UIElement::Type::Text) {
			TextElement* textelem = (TextElement*)element;
			textelem->fontSize = prevState.fontSize < action.fontSize ? (((action.fontSize - prevState.fontSize) * progression) + prevState.fontSize) : ((((prevState.fontSize - action.fontSize) * (1.f - progression)) + action.fontSize));
		} else if (element->type == UIElement::Type::RoundRect) {
			RoundRectElement* roundRectElement = (RoundRectElement*)element;
			roundRectElement->roundness = prevState.fontSize < action.fontSize ? (((action.fontSize - prevState.fontSize) * progression) + prevState.fontSize) : ((((prevState.fontSize - action.fontSize) * (1.f - progression)) + action.fontSize));
		}
	}

	finished = this->elapsedTime == this->requestedTime;
}



void SettingsImageElement::makeButton() {
	this->resourceIndex = -1;
	this->lastResourceIndex = -1;
	this->erender = [](UIElement* This) {
		SettingsImageElement* e = (SettingsImageElement*)This;
		Graphics* gfx = e->parent->gfx;
		bool isHovered = e == e->parent->mouseInside;

		gfx->setColor(isHovered ? D2D1::ColorF(0.f, 0.549019f, 1.f) : D2D1::ColorF(0.090196f, 0.674509f, 1.f));
		gfx->fillRoundedRectangle(e->posX, e->posY, e->posX + e->sizeX, e->posY + e->sizeY, 10.f);

		gfx->setColor(isHovered ? D2D1::ColorF(0.376470f, 0.694117f, 1.f) : D2D1::ColorF(0.396078f, 0.835294f, 1.f));
		gfx->drawRoundedRectangle(e->posX, e->posY, e->posX + e->sizeX, e->posY + e->sizeY, 10.f, 2.f);

		IDWriteTextLayout* textLayout;
		gfx->setColor(D2D1::ColorF(1.f, 1.f, 1.f, 1.f));
		gfx->setFont(L"Segoe UI", 16.f, false);
		gfx->getDwriteFactory()->CreateTextLayout(e->textualContent.c_str(), (UINT32)e->textualContent.size(), gfx->getTextFormat(), e->sizeX, e->sizeY, &textLayout);
		textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		DWRITE_TEXT_METRICS metrics;
		textLayout->GetMetrics(&metrics);
		assert(textLayout);
		gfx->target->DrawTextLayout(D2D1::Point2F(e->posX, ((e->sizeY / 2.f) - (metrics.height / 2.f)) + e->posY), textLayout, gfx->getBrush(), D2D1_DRAW_TEXT_OPTIONS::D2D1_DRAW_TEXT_OPTIONS_CLIP);
		textLayout->Release();
	};
}

void SettingsImageElement::makeCheckbox() {
	this->resourceIndex = -1;
	this->lastResourceIndex = -1;
	this->erender = [](UIElement* This) {
		SettingsImageElement* e = (SettingsImageElement*)This;
		Graphics* gfx = e->parent->gfx;
		bool isHovered = e == e->parent->mouseInside;

		gfx->setColor(isHovered ? D2D1::ColorF(0.f, 0.549019f, 1.f) : (*e->togglePtr ? D2D1::ColorF(0.090196f, 0.674509f, 1.f) : D2D1::ColorF(1.f, 1.f, 1.f, 0.2f)));
		gfx->fillRoundedRectangle(e->posX, e->posY, e->posX + e->sizeY, e->posY + e->sizeY, 5.f);

		gfx->setColor(isHovered ? D2D1::ColorF(0.376470f, 0.694117f, 1.f) : (*e->togglePtr ? D2D1::ColorF(0.396078f, 0.835294f, 1.f) : D2D1::ColorF(0.301960f, 0.356862f, 0.392156f)));
		gfx->drawRoundedRectangle(e->posX, e->posY, e->posX + e->sizeY, e->posY + e->sizeY, 5.f, 2.f);

		IDWriteTextLayout* textLayout;
		if (Options::lightTheme)
			gfx->setColor(D2D1::ColorF(0.f, 0.f, 0.f, 1.f));
		else
			gfx->setColor(D2D1::ColorF(1.f, 1.f, 1.f, 1.f));
		gfx->setFont(L"Segoe UI", 20.f, false);
		gfx->getDwriteFactory()->CreateTextLayout(e->textualContent.c_str(), (UINT32)e->textualContent.size(), gfx->getTextFormat(), 500.f, e->sizeY, &textLayout);
		DWRITE_TEXT_METRICS metrics;
		textLayout->GetMetrics(&metrics);
		assert(textLayout);
		gfx->target->DrawTextLayout(D2D1::Point2F(e->posX + e->sizeY + 5.f, ((e->sizeY / 2.f) - (metrics.height / 2.f)) + e->posY), textLayout, gfx->getBrush(), D2D1_DRAW_TEXT_OPTIONS::D2D1_DRAW_TEXT_OPTIONS_NONE);
		textLayout->Release();
	};
	this->eclicked = [](UIElement* This) {
		SettingsImageElement* e = (SettingsImageElement*)This;
		*e->togglePtr = !*e->togglePtr;
		Options::saveOptions();
		This->parent->gfx->redraw = true;
	};
}

void SettingsImageElement::mouseEnter() {
	if (playing)
		elapsedTime = 1.f - elapsedTime;
	else
		elapsedTime = 0.f;
	playing = true;
	parent->gfx->redraw = true;
}

void SettingsImageElement::mouseLeave() {
	if (playing)
		elapsedTime = 1.f - elapsedTime;
	else
		elapsedTime = 0.f;
	playing = true;
	parent->gfx->redraw = true;
}

void SettingsImageElement::render(float dt) {
	float progress = this->parent->mouseInside == this ? 1.f : 0.f;
	if (requestedTime <= 0.f) playing = false;
	if (playing) {
		elapsedTime += dt;
		if (elapsedTime > requestedTime) {
			elapsedTime = requestedTime;
			playing = false;
		}
		progress = powf(elapsedTime / requestedTime, 2);

		if (this->parent->mouseInside != this)
			progress = 1.f - progress;
	}
	parent->gfx->redraw = parent->gfx->redraw || playing;

	D2D1::Matrix3x2F animation = D2D1::Matrix3x2F::Scale(D2D1::SizeF(1.f + (grow * progress), 1.f + (grow * progress)), D2D1::Point2F(posX + (sizeX / 2.f), posY + (sizeY / 2.f)));
	D2D1::Matrix3x2F oldTransform;
	this->parent->gfx->target->GetTransform(&oldTransform);
	this->parent->gfx->target->SetTransform(animation);

	if (resourceIndex != lastResourceIndex || !selfimg) {
		int resultIdx = 0;
		switch (resourceIndex) {
		case -1: resourceIndex = -1;  break;
		case 0: resultIdx = IDB_PNG1; break;
		case 1: resultIdx = IDB_PNG1; break;
		case 2: resultIdx = IDB_PNG2; break;
		case 3: resultIdx = IDB_PNG3; break;
		case 4: resultIdx = IDB_PNG4; break;
		case 5: resultIdx = IDB_PNG5; break;
		case 6: resultIdx = IDB_PNG6; break;
		case 7: resultIdx = IDB_PNG7; break;
		case 8: resultIdx = IDB_PNG8; break;
		case 9: resultIdx = IDB_PNG9; break;
		case 10: resultIdx = IDB_PNG10; break;
		case 11: resultIdx = IDB_PNG11; break;
		case 12: resultIdx = IDB_PNG12; break;
		case 13: resultIdx = IDB_PNG13; break;
		case 14: resultIdx = IDB_PNG14; break;
		case 15: resultIdx = IDB_PNG15; break;
		case 16: resultIdx = IDB_PNG16; break;
		default:
			resultIdx = IDB_PNG1;
		}
		if (selfimg) {
			selfimg->bmp->Release();
			selfimg->bmp = 0;
		}
		if (resourceIndex != -1)
			selfimg = parent->gfx->loadBitmap(resultIdx, L"PNG");
		else
			selfimg = 0;
		lastResourceIndex = resourceIndex;
	}

	parent->gfx->drawBitmap(posX, posY, sizeX, sizeY, selfimg);
	erender(this);

	this->parent->gfx->target->SetTransform(oldTransform);
}
