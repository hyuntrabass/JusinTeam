#pragma once
#include "Client_Define.h"
#include "Base.h"
#include "GameInstance.h"


BEGIN(Client)
class CTrigger_Manager final : public CBase
{
	DECLARE_SINGLETON(CTrigger_Manager)
private:
	CTrigger_Manager();
	virtual ~CTrigger_Manager() = default;

public:
	HRESULT Init();
	void Tick(_float fTimeDelta);

	void Limited_CutScene(_bool isLimited);

	void Set_Trigger(class CTrigger* pTrigger);
	void Set_CutSceneCamera(class CCamera_CutScene* pCutScene);
	_bool Get_Limited() { return m_isLimited; }

	HRESULT CutScene_Registration(const wstring& strDataPath);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

private:
	_bool m_isColl{ false };
	_bool m_isLimited{ false };


private:
	vector<class CTrigger*> m_pTrigger{};
	class CCamera_CutScene* m_pCutScene{ nullptr };

public:
	virtual void Free() override;
};

END