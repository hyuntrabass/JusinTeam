#pragma once
#include "Client_Define.h"
#include "Base.h"
#include "GameInstance.h"


BEGIN(Client)
class CTrigger;
class CTrigger_Manager final : public CBase
{
	DECLARE_SINGLETON(CTrigger_Manager)
private:
	CTrigger_Manager();
	virtual ~CTrigger_Manager() = default;

public:
	HRESULT Init();
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	HRESULT Add_Layer_Trigger(TriggerInfo& pInfo, const _bool isStaticLevel = false);
	void Create_Trigger(const wstring& strTriggerTag, _mat pMatrix);
	class CTrigger* Clone(TriggerInfo& pInfo);
	void Delete_Trigger(const void* pMatrix);

	void Clear(_uint iLevelIndex);


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };
	HWND m_hWnd;
	_uint m_iWinSizeX = { 0 };
	_uint m_iWinSizeY = { 0 };

private:
	map<const void*, class CTrigger*> m_pTrigger[LEVEL_END]{};


public:
	virtual void Free() override;
};

END