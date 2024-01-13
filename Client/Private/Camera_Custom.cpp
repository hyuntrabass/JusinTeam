#include "Camera_Custom.h"

CCamera_Custom::CCamera_Custom(_dev pDevice, _context pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Custom::CCamera_Custom(const CCamera_Custom& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Custom::Init_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Custom::Init(void* pArg)
{
	if (not pArg)
	{
		MSG_BOX("null Arg : CCamera_Debug");
		return E_FAIL;
	}

	if (FAILED(__super::Init(pArg)))
	{
		return E_FAIL;
	}

	m_pGameInstance->Set_CameraNF(_float2(m_fNear, m_fFar));
	_vec4 vCamPos = { 0.f, 0.f, -1.f, 1.f };
	m_pTransformCom->Set_State(State::Pos, vCamPos);

	return S_OK;
}

void CCamera_Custom::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Get_CameraModeIndex() != CM_CUSTOM)
	{
		return;
	}

	CAMERA_STATE eState = (CAMERA_STATE)m_pGameInstance->Get_CameraState();
	switch (eState)
	{
	case CAMERA_STATE::CM_DEFAULT:
		Camera_Default(fTimeDelta);
		break;

	case CAMERA_STATE::CM_ZOOM:
		Camera_Zoom(fTimeDelta);
		break;

	}
}

void CCamera_Custom::Late_Tick(_float fTimeDelta)
{
}

void CCamera_Custom::Camera_Default(_float fTimeDelta)
{
}

void CCamera_Custom::Camera_Zoom(_float fTimeDelta)
{
	_vector vCurrentPos = m_pTransformCom->Get_State(State::Pos);

	float lerpFactor = 0.1f;

	_vec3 vTargetPos = m_pGameInstance->Get_CameraTargetPos();
	//m_pTransformCom->Look_At(vTargetPos);

	//vTargetPos = XMVectorSetY(vTargetPos, XMVectorGetY(vTargetPos) + 2.8f);
	vTargetPos = XMVectorSetZ(vTargetPos, XMVectorGetZ(vTargetPos) - 3.f);

	_vector vNewPos = XMVectorLerp(vCurrentPos, vTargetPos, lerpFactor);

	m_pTransformCom->Set_State(State::Pos, vNewPos);
}

CCamera_Custom* CCamera_Custom::Create(_dev pDevice, _context pContext)
{
	CCamera_Custom* pInstance = new CCamera_Custom(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCamera_Custom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Custom::Clone(void* pArg)
{
	CCamera_Custom* pInstance = new CCamera_Custom(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCamera_Custom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Custom::Free()
{
	__super::Free();

}
