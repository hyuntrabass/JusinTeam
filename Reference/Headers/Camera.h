#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	using Camera_Desc = struct tagCameraDesc
	{
		_float4 vCameraPos{}, vFocusPos{};
		_float fFovY{}, fAspect{}, fNear{}, fFar{};
	};
protected:
	CCamera(_dev pDevice, _context pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

protected:
	_float m_fFovY{}, m_fAspect{}, m_fNear{}, m_fFar{};

public:
	virtual CGameObject* Clone(void*) = 0;
	virtual void Free() override;
};

END