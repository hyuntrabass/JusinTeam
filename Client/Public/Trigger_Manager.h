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
	void Set_PlayCutScene(_bool isPlayCutScene) { m_isPlayCutScene = isPlayCutScene; }
	_bool Get_Limited() { return m_isLimited; }
	_bool Get_PlayCutScene() { return m_isPlayCutScene; }
	wstring Get_CutScene_Path() { return m_strFilePath; }
	
public:
	_bool Is_Coll_BossTrigger() { return m_isCollBossTrigger; }

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

private:
	_bool m_isColl{ false };
	_bool m_isLimited{ false };
	_bool m_isPlayCutScene{ false };
	wstring m_strFilePath{};

private:
	_bool m_isCollBossTrigger = { false };

private:
	vector<class CTrigger*> m_pTrigger{};

public:
	virtual void Free() override;
};

END