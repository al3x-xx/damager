#ifndef VISUALS_HPP
#define VISUALS_HPP

#include <memory>

#include "d3drender.h"
#include "lemon/detour.hpp"

class visuals {
	std::unique_ptr<CD3DFont> pFontNick_, pFontNickBar_, pRender_;
	std::unique_ptr<CD3DRender> pBox_;
public:
	visuals();
	~visuals();
private:
	using oReset = HRESULT(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	lemon::detour<oReset> hookReset_{ 0x0 };

	using oPresent = HRESULT(__stdcall*)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
	lemon::detour<oPresent> hookPresent_{ 0x0 };

	HRESULT hookedReset(oReset orig, IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	HRESULT hookedPresent(oPresent orig, IDirect3DDevice9* pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hdest, const RGNDATA* pDirtyRegion);

	std::uint32_t* getDevice();
	HWND getHWND();

	void imguiCreate(IDirect3DDevice9* pDevice);
	void imguiStyle();
	void imguiRender();
	void imguiDestroy();

	void imguiShowTooltip(const std::string& str);

	void imguiMenuSettings();
	void imguiMenuVisuals();
	void imguiMenuMisc();

	void d3Create();
	void d3Initialize(IDirect3DDevice9* pDevice);
	void d3Invalidate();
	void d3Render();

	void d3Menu();

	void fov();
	void line();
	void wallhack();
};

#endif // VISUALS_HPP