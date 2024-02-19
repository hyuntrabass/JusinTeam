#pragma once
#include "Client_Define.h"
#include "Base.h"
#include "GameInstance.h"

BEGIN(Client)

enum TeleportSpot
{
	TS_Dungeon,
	TS_Village,
	TS_END
};

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

	void Set_PlayCutScene(_bool isPlayCutScene) { m_isPlayCutScene = isPlayCutScene; }
	void Set_Inifinite(_bool isInfinite) { m_isInfinite = isInfinite; }
	void Set_SkyTextureIndex(const _uint iIndex);

	_bool Get_PlayCutScene() { return m_isPlayCutScene; }
	_bool Get_Limited() { return m_isLimited; }
	_bool Get_Infinite() { return m_isInfinite; }
	wstring Get_CutScene_Path() { return m_strFilePath; }
	const _uint& Get_SkyTextureIndex() const;
	_bool Get_InOut() { return m_isInVillage; }

	void Teleport(const TeleportSpot eSpot);

public:
	HRESULT Ready_Trigger_Village();
	
public:
	_bool Is_Coll_BossTrigger() { return m_isCollBossTrigger; }

public:
	void Set_StartSuicide() { m_bStartSuicide = true; }
	void Set_AfterSuicide() { m_bAfterSuicide = true; }
	void Set_BossStart() { m_bBossStart = true; }
	void LoopBroken() { m_isBreakLoop = false; }

	_bool Get_StartSuicide() { return m_bStartSuicide; }
	_bool Get_AfterSuicide() { return m_bAfterSuicide; }
	_bool Get_BossStart() { return m_bBossStart; }
	_bool Is_BreakLoop() { return m_isBreakLoop; }

private:
	CGameInstance* m_pGameInstance = { nullptr };

private:
	_bool m_isColl{ false };
	_bool m_isLimited{ false };
	_bool m_isPlayCutScene{ false };
	_bool m_isInfinite{ false };
	wstring m_strFilePath{};
	_bool m_isBreakLoop{};
	_bool m_isInVillage{};

private:
	_bool m_isCollBossTrigger = { false };

private: // Cutscene
	_bool m_bStartSuicide = { false };
	_bool m_bAfterSuicide = { false };
	_bool m_bBossStart = { false };

private:
	_int m_iSoundChannel = -1;

private:
	vector<class CTrigger*> m_pTrigger{};

	_uint m_iSkyTextureIndex{};

public:
	virtual void Free() override;
};

END