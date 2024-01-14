#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)

class CVoid05 final : public CMonster
{
private:
	CVoid05(_dev pDevice, _context pContext);
	CVoid05(const CVoid05& rhs);
	virtual ~CVoid05() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

public:
	static CVoid05* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END