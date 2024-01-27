#pragma once
#include "Client_Define.h"
#include "Base.h"
#include "GameInstance.h"

struct TriggerInfo
{
	_int iIndex{};
	_float fSize{};
	_mat WorldMat{};
};

BEGIN(Client)

class CTrigger_Manager final : public CBase
{
	DECLARE_SINGLETON(CTrigger_Manager)
private:
	CTrigger_Manager();
	virtual ~CTrigger_Manager() = default;

public:
	HRESULT Initialize_Prototype(const GRAPHIC_DESC& GraphicDesc);
	void Late_Tick(_float fTimeDelta);


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	HWND m_hWnd;
	_uint m_iWinSizeX = { 0 };
	_uint m_iWinSizeY = { 0 };

public:
	virtual void Free() override;
};

END