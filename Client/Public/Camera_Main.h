#pragma once
#include "Client_Define.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Main final : public CCamera
{
private:
	CCamera_Main(_dev pDevice, _context pContext);
	CCamera_Main(const CCamera_Main& rhs);
	virtual ~CCamera_Main() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

public:
	void Camera_Zoom(_float fTimeDelta);
private:
	CTransform* m_pPlayerTransform{ nullptr };
	_bool m_isMoveMode{ true };
	_float m_fMouseSensor{ 0.08f };
	_float m_fSpeed{ 10.f };
	_float m_fPlayerDistance{ 4.f };
	_float m_fShakeAcc{ 100.f };
	_bool  m_bInitMode[CAMERA_MODE::CM_END];
	_float m_fLerpTime{};
	_float m_fLerpDistance{4.f};
	_float m_fFirstDistance{4.f};
	_vec4 m_vAimCamPos{};
	_vec4 m_vOriCamPos{};
	_float m_AimZoomInTime{};
	_float m_AimZoomOutTime{};
	_float m_fSelectRotation{};
	_int m_iRotation{1};
	float m_SwayAmount = 3.0f;  // 출렁임의 크기를 결정하는 값. 원하는 크기로 조절 가능
	float m_SwaySpeed = 3.0f;   // 출렁임의 속도를 결정하는 값. 원하는 속도로 조절 가능
	float m_TimeOffset = 0.5f;  // 두 출렁임 사이의 시간 차. 원하는 시간 차로 조절 가능
	float m_CurrentTime{}; // 현재 시간

private:
	void Select_Mode(_float fTimeDelta);
	void Custom_Mode(_float fTimeDelta);

private:
	_float m_fSceneTimer{};
	_uint m_iSceneIndex{};
	CUTSCENE* m_pScene{ nullptr };
	_bool m_bSelect{};
	_bool m_bZoomEnd{};
public:
	static CCamera_Main* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END