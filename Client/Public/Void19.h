#pragma once

#include "VTFMonster.h"
#include "Client_Define.h"

BEGIN(Client)

class CVoid19 final : public CVTFMonster
{
private:
	CVoid19(_dev pDevice, _context pContext);
	CVoid19(const CVoid19& rhs);
	virtual ~CVoid19() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Instance() override;

public:
	static CVoid19* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END