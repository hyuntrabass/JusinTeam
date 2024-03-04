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
	list<class CGuard*> m_Guard;
	list<class CGuardTower*> m_GuardTower;



private:
	void Release_DeadObjects();

public:
	static CInfiltrationGame* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END