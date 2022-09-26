#ifndef VISUALS_HPP
#define VISUALS_HPP

#include "d3drender.h"
#include "lemon/detour.hpp"

class visuals {
	const std::unique_ptr<CD3DFont> pFontNick_{ std::make_unique<CD3DFont>("Small Fonts", 7, FCR_BORDER) }, pFontNickBar_{ std::make_unique<CD3DFont>("Small Fonts", 6, FCR_BORDER) }, pRender_{ std::make_unique<CD3DFont>("Tahoma", 7, FCR_BORDER) };
	const std::unique_ptr<CD3DRender> pBox_{ std::make_unique<CD3DRender>(128) };
public:
	visuals() noexcept;
	~visuals();
private:
	using oReset = HRESULT(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	lemon::detour<oReset> hookReset_{ 0x0 };

	using oPresent = HRESULT(__stdcall*)(IDirect3DDevice9*, const RECT*, const RECT*, HWND, const RGNDATA*);
	lemon::detour<oPresent> hookPresent_{ 0x0 };

	HRESULT hookedReset(oReset orig, IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	HRESULT hookedPresent(oPresent orig, IDirect3DDevice9* pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hdest, const RGNDATA* pDirtyRegion);

	[[nodiscard]] const std::uint32_t* getDevice() const noexcept;
	[[nodiscard]] const HWND getHwnd() const noexcept;

	void imguiCreate(IDirect3DDevice9* pDevice) const;
	void imguiStyle() const;
	void imguiRender() const noexcept;
	void imguiDestroy() const;

	void imguiShowTooltip(const std::string& str) const;

	void imguiMenuSettings() const noexcept;
	void imguiMenuVisuals() const noexcept;
	void imguiMenuMisc() const noexcept;

	void d3Initialize(IDirect3DDevice9* pDevice) const;
	void d3Invalidate() const;
	void d3Render() const noexcept;

	void d3Menu() const noexcept;

	void fov() const;
	void line() const;
	void wallhack() const noexcept;
};

#endif