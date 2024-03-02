#pragma once

#include "GameObject.h"
#include "Client_Define.h"
#include "Hook.h"
BEGIN(Client)

class CCescoGame final : public CGameObject
{
private:
	enum Phase
	{
		Phase1,
		Phase2,
		Phase3,
		Phase_End,
	};
private:
	CCescoGame(_dev pDevice, _context pContext);
	CCescoGame(const CCescoGame& rhs);
	virtual ~CCescoGame() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	void Tick_Phase(_float fTimeDelta);

private:
	CTransform* m_pPlayerTransform = { nullptr };
	list<class CVTFMonster*> m_Monsters;
	vector<class CHook*> m_vecHooks;
	class CHook* m_pCurrent_DraggingHook{};
	map<_uint, class CLog*> m_Logs;
	map<_uint, class CHive*> m_Hives;

private:
	_randNum m_RandomNumber;
	Phase m_eCurrentPhase{ Phase_End };
	vector<_vec3> m_SpawnPositions;
	vector<_vec3> m_HiveSpawnPositions;
	map<_int, _vec3> m_LarvaPositions;
	_uint m_iMonsterLimit{ 200 };
	_float m_fTimeLimit{ 1209999999.f };
	_float m_fMonsterSpawnTime{};
	_uint m_iMonsterSpawnCount{};

	//Hook
	_float m_fHookSpawnTime{};
	_bool m_bHadDragging{};
	_vec4 m_vHookPos{};
	_uint m_iDragging_EscapeCount{};
	_float m_fHookAttTime{};

	//Log
	_float m_fLogSpawnTime[4]{};

	//Poison
	_float m_fPosionSpawnTime{};

	//Hive
	_bool m_IsSpawnHive[2]{};
	_float m_fHiveSpawnTime[2]{};

	//Wasp
	_float m_fWaspSpawnTime[8]{};

private:
	HRESULT Create_CommonMonster(const wstring& strModelTag, _vec3 SpawnPosition, const wstring& strPrototypeTag);
	HRESULT Create_Hook();
	HRESULT Create_Larva();
	HRESULT Create_Log(_uint SpawnPositionIndex);
	HRESULT Create_Posion(_float fTimeDelta);
	HRESULT Create_Hive();

private:
	void Release_DeadObjects();

public:
	static CCescoGame* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END