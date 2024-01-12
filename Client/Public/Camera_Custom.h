#pragma once
#include "Client_Define.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Custom final : public CCamera
{
private:
	CCamera_Custom(_dev pDevice, _context pContext);
	CCamera_Custom(const CCamera_Custom& rhs);
	virtual ~CCamera_Custom() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	void Camera_Default(_float fTimeDelta);
	void Camera_Zoom(_float fTimeDelta);
public:
	static CCamera_Custom* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END