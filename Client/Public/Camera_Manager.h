#pragma once
#include "Client_Define.h"

BEGIN(Client)

class CCamera_Manager : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)
private:
	CCamera_Manager() = default;
	virtual ~CCamera_Manager() = default;

public:
	const _float& Get_CameraAttackZoom() { return m_fCameraAttackZoom; }
	const CAMERA_STATE& Get_CameraState()  const;
	const _float& Get_ZoomFactor() const;
	const _vec4& Get_CameraTargetPos() const;
	const _vec4& Get_CameraTargetLook();
	const _bool& Have_TargetLook() const;
	const _vec3& Get_AimPos() { return m_AimPos; }
	const _bool& Get_AimMode() { return m_AimMode; }
	_bool Get_FlyCam() { return m_bFlyCam; }
	_float Get_ShakePower() { return m_fShakePower; }
	// ī�޶󿡼� ����ŷ �ؾߵǴ��� �޴� �Լ�.
	const _bool& Get_ShakeCam() const;
	// ���� ī�޶� ���� ī�޶����� ����� ī�޶����� ��ȯ��. client define�� �̳� ����.
	const CAMERA_MODE& Get_CameraModeIndex() const;

	void Set_ZoomFactor(const _float fFactor);
	void Set_CameraState(const CAMERA_STATE& iIndex);
	void Set_CameraTargetPos(const _vec4& vPos);
	void Set_CameraTargetLook(const _vec4& vLook);
	void Set_Have_TargetLook(const _bool& bHaveLook);
	void Set_AimMode(_bool Aim, _vec3 AimPos = _vec3(0.6f, 1.7f, 1.4f));
	void Set_FlyCam(_bool Fly) { m_bFlyCam = Fly; }
	void Set_CameraAttackZoom(_float fAttackZoom) { m_fCameraAttackZoom = fAttackZoom; }
	// ī�޶� ����ũ ���. true ������ ī�޶� �ѹ� ��鸲.
	void Set_ShakeCam(const _bool& bShake, _float fShakePower = 0.1f);
	// ī�޶� ��带 ������. ī�޶󿡼� ����� ���� ����.
	void Set_CameraModeIndex(const CAMERA_MODE& iIndex);

private:
	CAMERA_MODE m_eCamMode{};
	CAMERA_STATE m_iCameraState{};
	_float m_fZoomFactor{ 3.f };
	_float m_fCameraAttackZoom{};
	_bool m_bTargetLook{ false };
	_vec4 m_vTarget{};
	_vec4 m_vTargetLook{};
	_bool m_AimMode{};
	_vec3 m_AimPos{};
	_bool m_bFlyCam{};
	_bool m_bShakeCamera{};
	_float m_fShakePower{};

public:
	virtual void Free() override;
};

END