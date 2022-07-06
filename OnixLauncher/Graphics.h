#pragma once

#include <Windows.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <string>
#include <vector>
#include <thread>

class BitmapImage;
class Graphics {
private:
    ID2D1Factory* factory;
    ID2D1SolidColorBrush* br;

    IDWriteFactory* dwFactory;
    IDWriteTextFormat* tFormat;

    std::vector<BitmapImage*> bmps;
    std::vector<IUnknown*> cleanupList;

    //class CustomFontLoader* customFontLoader;
public:
   HWND winHandle;
   ID2D1HwndRenderTarget* target;
   ID2D1DeviceContext* deviceContext = nullptr;

   volatile bool redraw;
   volatile bool ready;
   volatile bool wantsDeath = false;
   volatile bool killed = false;

   ID2D1Factory* getFactory() { return factory; }
   IDWriteFactory* getDwriteFactory() { return dwFactory; }
   ID2D1SolidColorBrush* getBrush() { return br; }
   IDWriteTextFormat* getTextFormat() { return tFormat; }
   void giveObject(IUnknown* object) { cleanupList.push_back(object); }

    Graphics();
    ~Graphics();


    bool Init(HWND hwnd);

    void beginDraw() { target->BeginDraw(); }
    void endDraw();
    void clear(float r, float g, float b);

    void setColor(float r, float g, float b);
    void setColor(float r, float g, float b, float a);
    void setColor(float ValeurDeCouleur);
    void setColor(const D2D1_COLOR_F& couleur);
    void setFont(const WCHAR* fontName, float textSize, bool bold = false);
    D2D1_COLOR_F getColor() { return br->GetColor(); }

   
    void drawRoundedRectangle(float x1, float y1, float x2, float y2, float size, float width);
    void fillRoundedRectangle(float x1, float y1, float x2, float y2, float size);


    void drawLine(float x1, float y1, float x2, float y2, float r, float g, float b);
    void drawLine(float x1, float y1, float x2, float y2, float r, float g, float b, float thicc);
    void drawLine(float x1, float y1, float x2, float y2);
    void drawGradianLineX(float x1, float y1, float x2, float y2, float opacity);
    void drawGradianLineY(float x1, float y1, float x2, float y2, float opacity);

    void fillRectangle(float x1, float y1, float x2, float y2, float r, float g, float b);
    void fillRectangle(float x1, float y1, float x2, float y2);
    void drawRectangle(float x1, float y1, float x2, float y2, float r, float g, float b, float lineThickness);
    void drawRectangle(float x1, float y1, float x2, float y2, float lineThickness);


    void fillTriangle(float x1, float y1, float x2, float y2, float x3, float y3);

    void drawCircle(float x1, float y1, float sizex, float sizey, float fatness);
    void fillCircle(float x1, float y1, float sizex, float sizey);


    void drawText(float x, float y, const WCHAR* text);
    void drawText(float x, float y, const char*);
    void drawText(float x, float y, const std::string& text);
    void drawText(float x, float y, const std::wstring& text);


    //Bitmap stuff, not really gaming but will do 4 now
    BitmapImage* loadBitmapFromFile(const wchar_t* fileName);
    BitmapImage* getBitmapFromFile(const wchar_t* fileName);
    BitmapImage* loadBitmap(DWORD resourceName, PCWSTR resourceType); 
    BitmapImage* makeOpacityMask(float width, float height);
    BitmapImage* getImage(size_t index);
    void drawBitmap(float x1, float y1, float x2, float y2, int imgID, float opactity, D2D1_BITMAP_INTERPOLATION_MODE mode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    void drawBitmap(float x1, float y1, float x2, float y2, int imgID, D2D1_BITMAP_INTERPOLATION_MODE mode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    void drawBitmap(float x1, float y1, int imgID, D2D1_BITMAP_INTERPOLATION_MODE mode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    void drawBitmap(float x1, float y1, float x2, float y2, BitmapImage* img, float opactity, D2D1_BITMAP_INTERPOLATION_MODE mode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    void drawBitmap(float x1, float y1, float x2, float y2, BitmapImage* img, D2D1_BITMAP_INTERPOLATION_MODE mode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    void drawBitmap(float x1, float y1, BitmapImage* img, D2D1_BITMAP_INTERPOLATION_MODE mode = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
};











class BitmapImage {
public:
    DWORD creationMode = 0;
    DWORD resourceName = 0;
    const wchar_t* fileName_or_resourceType = 0;
    ID2D1Bitmap* bmp;

    BitmapImage(DWORD resourceName, PCWSTR resourceType, ID2D1HwndRenderTarget* target);
    BitmapImage(const wchar_t* fileName, ID2D1HwndRenderTarget* target);
    BitmapImage(ID2D1Bitmap* bmp) { this->bmp = bmp; }
    ~BitmapImage();

    BitmapImage* recreate(ID2D1HwndRenderTarget* RenderTarget);
};


//class CustomFontLoader : public IDWriteFontCollectionLoader {
//	ULONG refCount = 1;
//	CustomFontLoader() {  }
//public:
//#pragma region unknown
//	virtual HRESULT STDMETHODCALLTYPE QueryInterface(
//		/* [in] */ REFIID riid,
//		/* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) {
//		return E_NOTIMPL;
//	}
//
//	virtual ULONG STDMETHODCALLTYPE AddRef(void) {
//		return ++refCount;
//	}
//
//	virtual ULONG STDMETHODCALLTYPE Release(void) {
//		--refCount;
//		if (refCount == 0) {
//			delete this;
//			return 0;
//		}
//		return refCount;
//	}
//#pragma endregion
//	static inline const char identifier[19] = "myMemoryfontLoader";
//	static inline const UINT32 blobSize = sizeof(identifier);
//
//	STDMETHOD(CreateEnumeratorFromKey)(_In_ IDWriteFactory* factory, _In_reads_bytes_(collectionKeySize) void const* collectionKey, UINT32 collectionKeySize, _COM_Outptr_ IDWriteFontFileEnumerator** fontFileEnumerator);
//	static inline CustomFontLoader* make() { return new CustomFontLoader(); }
//
//	class Iterator : public IDWriteFontFileEnumerator {
//		ULONG refCount = 1;
//		Iterator(IDWriteFactory* factor) { this->factor = factor; }
//		int currentOne = 0;
//	public:
//		IDWriteFactory* factor;
//#pragma region unknown
//	public:
//		virtual HRESULT STDMETHODCALLTYPE QueryInterface(
//			/* [in] */ REFIID riid,
//			/* [iid_is][out] */ _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) {
//			return E_NOTIMPL;
//		}
//
//		virtual ULONG STDMETHODCALLTYPE AddRef(void) {
//			return ++refCount;
//		}
//
//		virtual ULONG STDMETHODCALLTYPE Release(void) {
//			--refCount;
//			if (refCount == 0) {
//				delete this;
//				return 0;
//			}
//			return refCount;
//		}
//#pragma endregion
//        STDMETHOD(MoveNext)(_Out_ BOOL* hasCurrentFile);
//        STDMETHOD(GetCurrentFontFile)(_COM_Outptr_ IDWriteFontFile** fontFile);
//
//		static inline Iterator* make(IDWriteFactory* factor) { return new Iterator(factor); }
//	};
//};