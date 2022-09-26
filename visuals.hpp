#ifndef VISUALS_HPP
#define VISUALS_HPP

#include "d3drender.h"
#include "lemon/detour.hpp"

class visuals {
	const std::unique_ptr<CD3DFont> fontNick_{ std::make_unique<decltype(fontNick_)::element_type>("Small Fonts", 7, FCR_BORDER) }, fontNickBar_{ std::make_unique<decltype(fontNickBar_)::element_type>("Small Fonts", 6, FCR_BORDER) }, render_{ std::make_unique<decltype(render_)::element_type>("Tahoma", 7, FCR_BORDER) };
	const std::unique_ptr<CD3DRender> box_{ std::make_unique<decltype(box_)::element_type>(128) };
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

	[[nodiscard]] const std::uint32_t* getDevice() const noexcept;
	[[nodiscard]] const HWND getHwnd() const noexcept;

	void imguiCreate(IDirect3DDevice9* pDevice) const;
	void imguiStyle() const;
	void imguiRender() const;
	void imguiDestroy() const;

	void imguiShowTooltip(const std::string& str) const;

	void imguiMenuSettings() const;
	void imguiMenuVisuals() const;
	void imguiMenuMisc() const;

	void d3Initialize(IDirect3DDevice9* pDevice) const;
	void d3Invalidate() const;
	void d3Render() const;

	void d3Menu() const;

	void fov() const;
	void line() const;
	void wallhack() const;
};

#endif