#pragma once
#include "MapEditor_Define.h"
#include "Camera.h"

BEGIN(MapEditor)

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
	CTransform* m_pPlayerTransform{ nullptr };
	_bool m_isMoveMode{ true };
	_float m_fMouseSensor{ 0.08f };
	_float m_fSpeed{ 10.f };
	_float m_fPlayerDistance{ 4.f };
	_float m_fShakeAcc{ 100.f };

private:
	_float m_fSceneTimer{};
	_uint m_iSceneIndex{};
	CUTSCENE* m_pScene{ nullptr };

public:
	static CCamera_Custom* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END