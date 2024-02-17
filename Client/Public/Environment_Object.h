#pragma once
#include "Client_Define.h"
#include "Objects.h"

BEGIN(Client)

class CEnvironment_Object final : public CObjects
{
private:
	CEnvironment_Object(_dev pDevice, _context pContext);
	CEnvironment_Object(const CEnvironment_Object& rhs);
	virtual ~CEnvironment_Object() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	static CEnvironment_Object* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END