#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CSurvivalGame final : public CGameObject
{
private:
	CSurvivalGame(_dev pDevice, _context pContext);
	CSurvivalGame(const CSurvivalGame& rhs);
	virtual ~CSurvivalGame() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CSurvivalGame* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END