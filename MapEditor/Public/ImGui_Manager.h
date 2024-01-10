#pragma once

#include "Base.h"
#include "MapEditor_Define.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(MapEditor)
class CImGui_Manager final : public CBase
{

private:
	CImGui_Manager(_dev pDevice, _context pContext);
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize_Prototype(const GRAPHIC_DESC& GraphicDesc);
	void Tick(_float fTimeDelta);
	void LateTick(_float fTimeDelta);
	HRESULT Render();

private:
	HRESULT ImGuiMenu();
	HRESULT ImGuizmoMenu();

	const char* Search_Files();

private:
	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pContext{ nullptr };
	CGameInstance* m_pGameInstance{ nullptr };
	HWND m_hWnd;
	_uint m_iWinSizeX = { 0 };
	_uint m_iWinSizeY = { 0 };

	POINT m_ptMouse = {};
	_float4 m_vMousePos = { 0.f,0.f,0.f,1.f };

public:
	static CImGui_Manager* Create(_dev pDevice, _context pContext, const GRAPHIC_DESC& GraphicDesc);
	virtual void Free() override;

};
END
