#include "Graphics.h"
#include <wincodec.h>
#include <sstream>
#include <cstdlib>
//#include <fstream>
#include <assert.h>
#include "resource.h"


Graphics::Graphics() {
	factory = nullptr;
	target = nullptr;
	br = nullptr;

	dwFactory = nullptr;
	tFormat = nullptr;
	winHandle = nullptr;
}

Graphics::~Graphics() {
	this->ready = false;

	for (BitmapImage* b : bmps) 
		delete b;
	
	for (IUnknown* obj : cleanupList) 
		obj->Release();
	
	if (br) br->Release();
	br = nullptr;
	if (target) target->Release();
	target = nullptr;
	if (deviceContext) deviceContext->Release();
	deviceContext = nullptr;
	if (tFormat) tFormat->Release();
	tFormat = nullptr;

	if (dwFactory) dwFactory->Release();
	if (factory) factory->Release();
}
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
extern bool InstallFontResource();
bool Graphics::Init(HWND hwnd) {
	//Create Factorys
	
#ifndef NDEBUG
	D2D1_FACTORY_OPTIONS options;
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;

	HRESULT fail = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED,options,&factory);
#else
	HRESULT fail = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
#endif
	if (fail != S_OK) return false;

	fail = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(dwFactory), reinterpret_cast<IUnknown**>(&dwFactory));
	if (fail != S_OK) return false;

	//Render Target
	RECT rect;
	GetClientRect(hwnd, &rect);
	fail = factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rect.right, rect.bottom)), &target);
	if (fail != S_OK) return false;

	fail = target->QueryInterface(IID_PPV_ARGS(&deviceContext));
	if (fail != S_OK) return false;

	fail = target->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f), &br);
	if (fail != S_OK) return false;

	fail = dwFactory->CreateTextFormat(L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 16, L"", &tFormat);
	if (fail != S_OK) return false;
	
	//InstallFontResource();
	//customFontLoader = CustomFontLoader::make();
	//IDWriteFontCollection* collection = NULL;
	//fail = dwFactory->RegisterFontCollectionLoader(customFontLoader);
	//fail = dwFactory->CreateCustomFontCollection(customFontLoader, customFontLoader->identifier, customFontLoader->blobSize, &collection);

	//UINT32 ppinsl = sizeof(L"Poppins") / sizeof(wchar_t)-1;
	//UINT32 arinsl = sizeof(L"Arial") / sizeof(wchar_t)-1;
	//wchar_t fl[40];
	//fail = dwFactory->CreateTextFormat(L"shut up at that point ", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 16, L"", &tFormat);
	//UINT32 fnamel = tFormat->GetFontFamilyNameLength();
	//tFormat->GetFontFamilyName(fl, 40);
	//if (fail != S_OK) return false;
	//
	//{
	//	std::wofstream out("game1.txt");
	//	IDWriteFontCollection* fontCollection = collection;
	//	dwFactory->GetSystemFontCollection(&fontCollection, true);
	//	int familycount = (int)fontCollection->GetFontFamilyCount();
	//	static wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
	//	int defaultLocaleSuccess = GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);
	//
	//	for (int i = 0; i < familycount; i++) {
	//		IDWriteFontFamily* fontFamily;
	//		fontCollection->GetFontFamily(i, &fontFamily);
	//		IDWriteLocalizedStrings* familyNames;
	//		fontFamily->GetFamilyNames(&familyNames);
	//		int nameCount = (int)familyNames->GetCount();
	//		UINT32 index = 0;
	//		BOOL exist = false;
	//		familyNames->FindLocaleName(L"en-us", (UINT32*)&index, &exist);
	//		if (exist) {
	//			UINT32 lenght = 0;
	//			familyNames->GetStringLength(index, &lenght);
	//			wchar_t* nameStr = new wchar_t[lenght + 8];
	//			familyNames->GetString(index, nameStr, lenght + 1);
	//			out << nameStr << '\n';
	//			delete[] nameStr;
	//		}
	//		else if (defaultLocaleSuccess) {
	//			familyNames->FindLocaleName(localeName, (UINT32*)&index, &exist);
	//			if (exist) {
	//				UINT32 lenght = 0;
	//				familyNames->GetStringLength(index, &lenght);
	//				wchar_t* nameStr = new wchar_t[lenght + 8];
	//				familyNames->GetString(index, nameStr, lenght + 1);
	//				out << nameStr << '\n';
	//				delete[] nameStr;
	//			}
	//		}
	//	}
	//	out.close();
	//}




	Graphics::winHandle = hwnd;
	Graphics::ready = true;
	Graphics::redraw = true;
	return true;
}

void Graphics::setColor(float r, float g, float b) {
	br->SetColor(D2D1::ColorF(r, g, b));
}
void Graphics::setColor(float r, float g, float b, float a) {
	br->SetColor(D2D1::ColorF(r, g, b, a));
}
void Graphics::setColor(float ValeurCouleur) {
	br->SetColor(D2D1::ColorF(ValeurCouleur, ValeurCouleur, ValeurCouleur));
}
void Graphics::setColor(const D2D1_COLOR_F& couleur) {
	br->SetColor(couleur);
}

void Graphics::setFont(const WCHAR* fontName, float textSize, bool bold) {
	tFormat->Release();
	tFormat = nullptr;
	dwFactory->CreateTextFormat(fontName, NULL, bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, textSize, L"", &tFormat);
}

void Graphics::endDraw() {
	HRESULT hr = target->EndDraw();
	if (hr != S_OK) {
		if (hr == D2DERR_RECREATE_TARGET) {
			std::vector<BitmapImage*> images = std::move(bmps);
			for (BitmapImage* img : images)
				if (img->bmp) img->bmp->Release();
			HWND hwnd = this->winHandle;
			this->~Graphics();
			this->Init(hwnd);
			for (BitmapImage* img : images)
				this->bmps.emplace_back(img->recreate(target));
		} else {
			std::string gotya = "Oopsies, i hope you like a black screen?: " + std::to_string(hr);
			OutputDebugStringA(gotya.c_str());
			OutputDebugStringA("\n");
		}
	}
}

void Graphics::clear(float r, float g, float b) {
	target->Clear(D2D1::ColorF(r, g, b));
}

void Graphics::drawLine(float x1, float y1, float x2, float y2, float r, float g, float b) {
	br->SetColor(D2D1::ColorF(r, g, b));
	target->DrawLine(D2D1::Point2(x1, y1), D2D1::Point2(x2, y2), br, 1.0f);
}
void Graphics::drawLine(float x1, float y1, float x2, float y2) {
	target->DrawLine(D2D1::Point2(x1, y1), D2D1::Point2(x2, y2), br, 1.0f);
}

void Graphics::drawLine(float x1, float y1, float x2, float y2, float r, float g, float b, float thicc) {
	br->SetColor(D2D1::ColorF(r, g, b));
	target->DrawLine(D2D1::Point2(x1, y1), D2D1::Point2(x2, y2), br, thicc);
}


void Graphics::drawGradianLineX(float x1, float y1, float x2, float y2, float opacity) {

	float colorJump = 255 / ((x2 - x1) / 2.f); //get value in rgb 255
	colorJump = colorJump / 255.f; //convert to float 0-1
	float colorValue = 0.f;
	float stop = (x2 - x1) / 2;
	stop = stop + x1;

		for (float i = x1; i < stop; i++) {
			br->SetColor(D2D1::ColorF(colorValue, colorValue, colorValue, opacity));

			target->DrawRectangle(D2D1::RectF(i, y1, i, y2), br);
			colorValue = colorValue + colorJump;
		}

		stop = x2 + 1.f;
		for (float i = ((x2 - x1)/2)+x1; i < stop; i++) {
			br->SetColor(D2D1::ColorF(colorValue, colorValue, colorValue, opacity));

			target->DrawRectangle(D2D1::RectF(i, y1, i, y2), br);
			colorValue = colorValue - colorJump;
		}
		//and this is drawing a gradient 
		//et donc pratique le jeu vidéal comme désiré
}
void Graphics::drawGradianLineY(float x1, float y1, float x2, float y2, float opacity) {

	float colorJump = 255 / ((y2 - y1) / 2.f); //get value in rgb 255
	colorJump = colorJump / 255.f; //convert to float 0-1
	float colorValue = 0.f;
	float stop = (y2 - y1) / 2;
	stop = stop + y1;

		for (float i = y1; i < stop; i++) {
			br->SetColor(D2D1::ColorF(colorValue, colorValue, colorValue, opacity));

			target->DrawRectangle(D2D1::RectF(x1, i, x2, i), br);
			colorValue = colorValue + colorJump;
		}

		stop = y2 + 1.f;
		for (float i = ((y2 - y1)/2)+y1; i < stop; i++) {
			br->SetColor(D2D1::ColorF(colorValue, colorValue, colorValue, opacity));

			target->DrawRectangle(D2D1::RectF(x1, i, x2, i), br);
			colorValue = colorValue - colorJump;
		}
		//and this is drawing a gradient 
		//et donc pratique le jeu vidéal comme désiré
}



void Graphics::drawText(float x, float y, const WCHAR* text) {
	D2D1_RECT_F r;
	r.left = x;
	r.top = y;
	r.bottom = target->GetSize().height;
	r.right = target->GetSize().width;


	target->DrawTextW(text, (UINT32)wcslen(text), tFormat, &r, br, D2D1_DRAW_TEXT_OPTIONS_NONE);
}
void Graphics::drawText(float x, float y, const char* text) {
	D2D1_RECT_F r;
	r.left = x;
	r.top = y;
	r.bottom = target->GetSize().height;
	r.right = target->GetSize().width;
	
	wchar_t* out = new wchar_t[strlen(text) + 1];
	short tLenght = (short)strlen(text);
	size_t outSize = 0;

	mbstowcs_s(&outSize, out, strlen(text) + 1, text, _TRUNCATE);
	target->DrawText(out, (UINT32)strlen(text), tFormat, &r, br, D2D1_DRAW_TEXT_OPTIONS_NONE);
	delete[] out;
}
void Graphics::drawText(float x, float y, const std::wstring& text) {
	D2D1_RECT_F r;
	r.left = x;
	r.top = y;
	r.bottom = target->GetSize().height;
	r.right = target->GetSize().width;

	target->DrawTextW(text.c_str(), (UINT32)text.size(), tFormat, &r, br, D2D1_DRAW_TEXT_OPTIONS_NONE);
}
void Graphics::drawText(float x, float y, const std::string& text) {
	D2D1_RECT_F r;
	r.left = x;
	r.top = y;
	r.bottom = target->GetSize().height;
	r.right = target->GetSize().width;

	std::wstring wide_string;
	const char* foffdude = text.c_str();
	while (*foffdude != 0) {
		wide_string.push_back(*foffdude);
		foffdude++;
	}

	target->DrawTextW(wide_string.c_str(), (UINT32)wide_string.size(), tFormat, &r, br, D2D1_DRAW_TEXT_OPTIONS_NONE);
}



void Graphics::fillRectangle(float x1, float y1, float x2, float y2, float r, float g, float b) {
	br->SetColor(D2D1::ColorF(r, g, b));

	target->FillRectangle(D2D1::RectF(x1, y1, x2, y2), br);
}

void Graphics::fillRectangle(float x1, float y1, float x2, float y2) {
	target->FillRectangle(D2D1::RectF(x1, y1, x2, y2), br);
}
void Graphics::drawRectangle(float x1, float y1, float x2, float y2, float r, float g, float b, float lineThickness) {
	br->SetColor(D2D1::ColorF(r, g, b));

	target->DrawRectangle(D2D1::RectF(x1, y1, x2, y2), br, lineThickness);
}

void Graphics::drawRectangle(float x1, float y1, float x2, float y2, float lineThickness) {
	target->DrawRectangle(D2D1::RectF(x1, y1, x2, y2), br, lineThickness);
}


void Graphics::drawRoundedRectangle(float x1, float y1, float x2, float y2, float size, float width) {
	target->DrawRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x1, y1, x2, y2), size, size), br, width);
}
void Graphics::fillRoundedRectangle(float x1, float y1, float x2, float y2, float size) {
	target->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(x1, y1, x2, y2), size, size), br);
}





void Graphics::fillTriangle(float x1, float y1, float x2, float y2, float x3, float y3) {

	ID2D1PathGeometry* geo;
	factory->CreatePathGeometry(&geo);
	if (geo == nullptr) return;
	ID2D1GeometrySink* sink = nullptr;
	geo->Open(&sink);

	sink->BeginFigure(D2D1::Point2F(x1, y1), D2D1_FIGURE_BEGIN_FILLED);

	sink->AddLine(D2D1::Point2F(x2, y2));
	sink->AddLine(D2D1::Point2F(x3, y3));
	//sink->AddLine(D2D1::Point2F(x1, x1));

	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	sink->Close();


	target->FillGeometry(geo, br);
	sink->Release();
	geo->Release();
}






































BitmapImage* Graphics::loadBitmap(DWORD resourceName, PCWSTR resourceType) {

		//don't use it dosen't work
		BitmapImage* img = new BitmapImage(resourceName, resourceType, target);
		bmps.push_back(img);
		return img;
	}

BitmapImage* Graphics::makeOpacityMask(float width, float height) {
	ID2D1Bitmap1* d2dbmp;
	deviceContext->CreateBitmap(D2D1::SizeU((UINT32)width, (UINT32)height), 0, 0, D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_A8_UNORM, D2D1_ALPHA_MODE_STRAIGHT)), &d2dbmp);
	BitmapImage* img = new BitmapImage(d2dbmp);
	bmps.push_back(img);

	return img;
}

BitmapImage* Graphics::getImage(size_t index) {
	if (bmps.size() <= index)
	return nullptr;
	if (bmps.at(index) == nullptr)
	return nullptr;
	if (bmps.at(index)->bmp == nullptr)
	return nullptr;
	return bmps.at(index);
}


BitmapImage* Graphics::loadBitmapFromFile(const wchar_t* fileName) {
	BitmapImage* img = new BitmapImage(fileName, target);
		bmps.push_back(img);
		return img;
	}

	BitmapImage* Graphics::getBitmapFromFile(const wchar_t* fileName) {
		return new BitmapImage(fileName, target);
	}



	void Graphics::drawBitmap(float x1, float y1, float x2, float y2, int imgID, float opactity, D2D1_BITMAP_INTERPOLATION_MODE mode) {
		//Check for nulls
		if (bmps.size() <= imgID)
			return;
		if (bmps.at(imgID) == nullptr)
			return;
		if (bmps.at(imgID)->bmp == nullptr)
			return;

		target->DrawBitmap(bmps.at(imgID)->bmp, D2D1::RectF(x1, y1, x1 + x2, y1 + y2), opactity, mode, D2D1::RectF(0.0f, 0.0f, bmps.at(imgID)->bmp->GetSize().width, bmps.at(imgID)->bmp->GetSize().height));
	}
	void Graphics::drawBitmap(float x1, float y1, float x2, float y2, int imgID, D2D1_BITMAP_INTERPOLATION_MODE mode) {
		//Check for nulls
		if (bmps.size() <= imgID)
			return;
		if (bmps.at(imgID) == nullptr)
			return;
		if (bmps.at(imgID)->bmp == nullptr)
			return;

		target->DrawBitmap(bmps.at(imgID)->bmp, D2D1::RectF(x1, y1, x1 + x2, y1 + y2), 1.f, mode, D2D1::RectF(0.0f, 0.0f, bmps.at(imgID)->bmp->GetSize().width, bmps.at(imgID)->bmp->GetSize().height));
	}
	void Graphics::drawBitmap(float x1, float y1, int imgID, D2D1_BITMAP_INTERPOLATION_MODE mode) {
		//Check for invalid images
		if (bmps.size() <= imgID)
			return;
		if (bmps.at(imgID) == nullptr)
			return;
		if (bmps.at(imgID)->bmp == nullptr)
			return;

		target->DrawBitmap(bmps.at(imgID)->bmp, D2D1::RectF(x1, y1, x1 + bmps.at(imgID)->bmp->GetSize().width, y1 + bmps.at(imgID)->bmp->GetSize().height), 1.f, mode, D2D1::RectF(0.0f, 0.0f, bmps.at(imgID)->bmp->GetSize().width, bmps.at(imgID)->bmp->GetSize().height));
	}
	
	//with a bitmapimage*
	void Graphics::drawBitmap(float x1, float y1, float x2, float y2, BitmapImage* img, float opactity, D2D1_BITMAP_INTERPOLATION_MODE mode) {
		//Check for nulls
		if (img == nullptr)
			return;
		if (img->bmp == nullptr)
			return;

		target->DrawBitmap(img->bmp, D2D1::RectF(x1, y1, x1 + x2, y1 + y2), opactity, mode, D2D1::RectF(0.0f,0.0f, img->bmp->GetSize().width, img->bmp->GetSize().height));
	}
	void Graphics::drawBitmap(float x1, float y1, float x2, float y2, BitmapImage* img, D2D1_BITMAP_INTERPOLATION_MODE mode) {
		//Check for nulls
		if (img == nullptr)
			return;
		if (img->bmp == nullptr)
			return;

		target->DrawBitmap(img->bmp,D2D1::RectF(x1, y1, x1 + x2, y1 + y2),1.f, mode, D2D1::RectF(0.0f, 0.0f,	img->bmp->GetSize().width, img->bmp->GetSize().height));
	}
	void Graphics::drawBitmap(float x1, float y1, BitmapImage* img, D2D1_BITMAP_INTERPOLATION_MODE mode) {
		//Check for invalid images
		if (img == nullptr)
			return;
		if (img->bmp == nullptr)
			return;
		
		target->DrawBitmap(img->bmp, D2D1::RectF(x1, y1, x1 + img->bmp->GetSize().width, y1 + img->bmp->GetSize().height), 1.f, mode,D2D1::RectF(0.0f, 0.0f, img->bmp->GetSize().width, img->bmp->GetSize().height));
	}






	void Graphics::fillCircle(float x1, float y1, float x2, float y2) {
		D2D1_ELLIPSE uElipse;
		uElipse.point.x = x1;
		uElipse.point.y = y1;
		uElipse.radiusX = x2;
		uElipse.radiusY = y2;

		target->FillEllipse(uElipse, br);
	}

	void Graphics::drawCircle(float x1, float y1, float x2, float y2, float fatness) {
		D2D1_ELLIPSE uElipse;
		uElipse.point.x = x1;
		uElipse.point.y = y1;
		uElipse.radiusX = x2;
		uElipse.radiusY = y2;

		target->DrawEllipse(uElipse, br, fatness);
	}

















BitmapImage::BitmapImage(const wchar_t* fileName, ID2D1HwndRenderTarget* target) {
	creationMode = 2;
	this->fileName_or_resourceType = fileName;

	bmp = nullptr;
	HRESULT fail;

	//Create WIC factory
	IWICImagingFactory* wicFactory = nullptr;
	fail = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&wicFactory);
	if (fail != S_OK)
		PostQuitMessage(-1);

	//Create decoder
	IWICBitmapDecoder* wicDecoder = nullptr;
	fail = wicFactory->CreateDecoderFromFilename(fileName, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &wicDecoder);
	if (fail != S_OK)
		PostQuitMessage(-1);
	
	//get the first frame of the image 
	IWICBitmapFrameDecode* wicFrame = nullptr;
	fail = wicDecoder->GetFrame(0, &wicFrame);
	if (fail != S_OK)
		PostQuitMessage(-1);

	//Create a converter and setup converter
	IWICFormatConverter* wicConverter = nullptr;
	fail = wicFactory->CreateFormatConverter(&wicConverter);
	if (fail != S_OK)
		PostQuitMessage(-1);
	fail = wicConverter->Initialize(wicFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
	if (fail != S_OK)
		PostQuitMessage(-1);

	//Convert to d2d1 bitmap
	target->CreateBitmapFromWicBitmap(wicConverter, NULL, &bmp);

	wicFactory->Release();
	wicDecoder->Release();
	wicConverter->Release();
	wicFrame->Release();
}

BitmapImage::~BitmapImage() {
	if (bmp)
		bmp->Release();
	bmp = nullptr;
}
BitmapImage* BitmapImage::recreate(ID2D1HwndRenderTarget* pRenderTarget) {
	if        (this->creationMode == 1) {
		new (this) BitmapImage(this->resourceName, this->fileName_or_resourceType, pRenderTarget);
	} else if (this->creationMode == 2) {
		new (this) BitmapImage(this->fileName_or_resourceType, pRenderTarget);
	}
	return this;
}
#include "resource.h"

BitmapImage::BitmapImage(DWORD resourceName, PCWSTR resourceType, ID2D1HwndRenderTarget* pRenderTarget) {
	creationMode = 1;
	this->resourceName = resourceName;
	this->fileName_or_resourceType = resourceType;
	bmp = nullptr;
	IWICImagingFactory* wicFactory = nullptr;
	HRESULT fail = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&wicFactory);
	if (fail != S_OK)
		PostQuitMessage(-1);

	HRSRC resourceHandle = NULL;
	//for (int i = 1; i < 690; i++) {
	resourceHandle = FindResource(NULL, MAKEINTRESOURCEW(resourceName), L"PNG");
	//	assert(resourceHandle == 0);
		//if (resourceHandle != 0) {
		//	char msg[48];
		//	sprintf_s(msg, 48, "%i", i);
		//	OutputDebugStringA(msg);
		//	OutputDebugStringA("\n");
		//	break;
		//}
	//}
	assert(resourceHandle != 0);
	HGLOBAL resourceLoaded = LoadResource(NULL, resourceHandle);
	if (fail != S_OK)
		PostQuitMessage(-1);

	// Lock the resource to retrieve memory pointer.
	void* ResourceData = LockResource(resourceLoaded);
	fail = (ResourceData ? S_OK : E_FAIL);

	// Calculate the size.
	DWORD imageFileSize = SizeofResource(NULL, resourceHandle);
	fail = (imageFileSize ? S_OK : E_FAIL);

	//Create stream
	IWICStream* wicStream = nullptr;
	fail = wicFactory->CreateStream(&wicStream);
	if (fail != S_OK)
		PostQuitMessage(-1);
	fail = wicStream->InitializeFromMemory((BYTE*)ResourceData, imageFileSize);
	if (fail != S_OK)
		PostQuitMessage(-1);

	IWICBitmapDecoder* wicDecoder = nullptr;
	fail = wicFactory->CreateDecoderFromStream(wicStream, NULL, WICDecodeMetadataCacheOnLoad, &wicDecoder);
	if (fail != S_OK)
		PostQuitMessage(-1);

	//get the first frame of the image 
	IWICBitmapFrameDecode* wicFrame = nullptr;
	fail = wicDecoder->GetFrame(0, &wicFrame);
	if (fail != S_OK)
		PostQuitMessage(-1);

	//Create a converter and setup converter
	IWICFormatConverter* wicConverter = nullptr;
	fail = wicFactory->CreateFormatConverter(&wicConverter);
	if (fail != S_OK)
		PostQuitMessage(-1);

	fail = wicConverter->Initialize(wicFrame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut);
	if (fail != S_OK)
		PostQuitMessage(-1);

	//Convert to d2d1 bitmap
	fail = pRenderTarget->CreateBitmapFromWicBitmap(wicConverter, NULL, &bmp);
	if (fail != S_OK)
		PostQuitMessage(-1);

	wicDecoder->Release();
	wicStream->Release();
	wicFrame->Release();
	wicConverter->Release();
	wicFactory->Release();
	return;
}











//STDMETHODIMP_(HRESULT __stdcall) CustomFontLoader::CreateEnumeratorFromKey(IDWriteFactory* factory, void const* collectionKey, UINT32 collectionKeySize, IDWriteFontFileEnumerator** fontFileEnumerator) {
//	if (collectionKeySize == blobSize && !memcmp(identifier, collectionKey, blobSize)) {
//		*fontFileEnumerator = CustomFontLoader::Iterator::make(factory);
//		return S_OK;
//	}
//	return E_FAIL;
//}
//
//STDMETHODIMP_(HRESULT __stdcall) CustomFontLoader::Iterator::MoveNext(BOOL* hasCurrentFile) {
//		*hasCurrentFile = !(currentOne >= 2);
//		currentOne++;
//		return S_OK;
//}
//
//STDMETHODIMP_(HRESULT __stdcall) CustomFontLoader::Iterator::GetCurrentFontFile(IDWriteFontFile** fontFile) {
//	if (currentOne >= 3) return E_FAIL;
//	IDWriteFontFile* resultingResult;
//	_FILETIME ft;
//	HRESULT RE = factor->CreateFontFileReference(currentOne == 0 ? L"C:\\Users\\Onix\\Pictures\\Onox\\Launcher1\\Poppins-Regular.ttf" : L"C:\\Users\\Onix\\Pictures\\Onox\\Launcher1\\Poppins-Bold.ttf", &ft, &resultingResult);
//	if (RE == S_OK) *fontFile = resultingResult;
//	//factor->CreateCustomFontFileReference(myLoader::identifier, myLoader::blobSize,)
//	return RE;
//}