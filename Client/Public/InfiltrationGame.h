#pragma once

#include "GameObject.h"
#include "Client_Define.h"
BEGIN(Client)

class CInfiltrationGame final : public CGameObject
{
private:
	CInfiltrationGame(_dev pDevice, _context pContext);
	CInfiltrationGame(const CInfiltrationGame& rhs);
	virtual ~CInfiltrationGame() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	list<class CCheckPoint*> m_CheckPoint;
	vector<class CLever*> m_Lever;
	vector<list<class CGuard*>> m_GuardList;
	vector<list<class CGuardTower*>> m_GuardTowerList;

	class CWarning_Mark* m_pWarning = nullptr;

private:
	HRESULT Create_Guard(const TCHAR* pPath);
	HRESULT Create_CheckPoint();
	HRESULT Create_Lever();
	HRESULT Create_Door();
	void Reset_Play(_float fTimeDelta);
	void Release_DeadObjects();
	HRESULT Create_Teleport();

private:
	_bool m_isReset{ false };
	_mat m_CheckPointMatrix{};
	_float m_fResurrectionTime{ 0.f };
	_bool m_isTurnOff = false;
	_float m_fTime = 0.f;
	_bool m_Sound = false;

	_bool m_isDetected = false;

private:
	CTransform* m_pPlayerTransform = { nullptr };
	class CDoor* m_pDoor{ nullptr };
	class CMiniDungeon_Teleport* m_pTeleport{ nullptr };

public:
	static CInfiltrationGame* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END