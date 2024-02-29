#pragma once

#include "GameObject.h"
#include "Client_Define.h"
#include "Hook.h"
BEGIN(Client)

class CCescoGame final : public CGameObject
{
private:
	CCescoGame(_dev pDevice, _context pContext);
	CCescoGame(const CCescoGame& rhs);
	virtual ~CCescoGame() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	HRESULT Create_Hook();
private:
	CTransform* m_pPlayerTransform = { nullptr };
	list<class CVTFMonster*> m_Monsters;
	vector<_vec3> m_SpawnPositions;
	map<_int, _vec3> m_LarvaPositions;
	vector<CHook*> m_vecHooks;
	_randNum m_RandomNumber;
	CHook* m_pCurrent_DraggingHook{};

	_float m_fMonsterSpawnTime{};
	_uint m_iMonsterLimit{ 200 };
	_uint m_iMonsterSpawnCount{};
	_bool m_bHadDragging{};
	_float m_fTimeLimit{ 1209999999.f };
	_vec4 m_vHookPos{};
private:
	void Release_DeadObjects();

public:
	static CCescoGame* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END