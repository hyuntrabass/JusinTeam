#include "Camera_Main.h"

CCamera_Main::CCamera_Main(_dev pDevice, _context pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Main::CCamera_Main(const CCamera_Main& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Main::Init_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Main::Init(void* pArg)
{
	//m_pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	//if (not m_pPlayerTransform)
	//{
	//	return E_FAIL;
	//}
	//Safe_AddRef(m_pPlayerTransform);

	//if (not m_pPlayerTransform)
	//{
	//	MSG_BOX("Can't Find Player!! : Camera Main");
	//}

	for (_int i = 0; i < CM_END; i++)
	{
		//카메라는 한번만 부르니까 각 모드 이닛용 ..
		m_bInitMode[i] = false;
	}

	if (not pArg)
	{
		MSG_BOX("null Arg : CCamera_Debug");
		return E_FAIL;
	}

	if (FAILED(__super::Init(pArg)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CCamera_Main::Tick(_float fTimeDelta)
{

	if (m_pGameInstance->Get_CameraModeIndex() != CM_MAIN)
		return;

	if (m_pGameInstance->Key_Down(DIK_P))
	{
		m_pGameInstance->Set_CameraModeIndex(CM_DEBUG);
	}

	m_pGameInstance->Set_CameraNF(_float2(m_fNear, m_fFar));

	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_SELECT)
	{
		Select_Mode(fTimeDelta);
	}
	else if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_CUSTOM)
	{
		Custom_Mode(fTimeDelta);
	}
	else
	{

		_long dwMouseMove;

		if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta / m_pGameInstance->Get_TimeRatio() * dwMouseMove * m_fMouseSensor);
		}

		if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(State::Right), fTimeDelta / m_pGameInstance->Get_TimeRatio() * dwMouseMove * m_fMouseSensor);
		}


		_float fRStickMove{};
		if (fRStickMove = m_pGameInstance->Gamepad_RStick().x)
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta / m_pGameInstance->Get_TimeRatio() * fRStickMove);
		}
		if (fRStickMove = m_pGameInstance->Gamepad_RStick().y)
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(State::Right), fTimeDelta / m_pGameInstance->Get_TimeRatio() * -fRStickMove);
		}

		if (m_pGameInstance->Get_MouseMove(MouseState::wheel) > 0)
		{
			m_fPlayerDistance -= 1.f;
		}
		else if (m_pGameInstance->Get_MouseMove(MouseState::wheel) < 0)
		{
			m_fPlayerDistance += 1.f;
		}

		// 	y = sin(x * 10.0f) * powf(0.5f, x)

		if (m_pGameInstance->Get_ShakeCam())
		{
			m_fShakeAcc = { 0.1f };
			m_pGameInstance->Set_ShakeCam(false);
		}

		_float fShakeAmount = sin(m_fShakeAcc * 15.f) * powf(0.5f, m_fShakeAcc) * 0.2f;
		
		/*m_pTransformCom->Set_State(State::Pos,
			m_pPlayerTransform->Get_CenterPos()
			- (m_pTransformCom->Get_State(State::Look) * m_fPlayerDistance)
			+ (m_pTransformCom->Get_State(State::Up) * m_fPlayerDistance * 0.25f));*/

		/*_vec4 vLook = m_pTransformCom->Get_State(State::Look);
		PxRaycastBuffer Buffer{};
		_vec4 vRayDir{};
		_vec4 vMyPos = m_pTransformCom->Get_State(State::Pos);
		_vec4 PlayerCenter = m_pPlayerTransform->Get_CenterPos();
		vRayDir = vMyPos - PlayerCenter;
		vRayDir.Normalize();
		_float fDist = XMVectorGetX(XMVector3Length(vRayDir)) - 0.4f;
		if (m_pGameInstance->Raycast(m_pPlayerTransform->Get_CenterPos() + vRayDir * 0.5f, vRayDir, fDist, Buffer))
		{
			m_pTransformCom->Set_State(State::Pos, PxVec3ToVector(Buffer.block.position, 1.f));
		}
		*/

		_vec4 vShakePos = m_pTransformCom->Get_State(State::Pos);
		vShakePos += XMVectorSet(fShakeAmount, -fShakeAmount, 0.f, 0.f);
		m_pTransformCom->Set_State(State::Pos, vShakePos);

		m_fShakeAcc += fTimeDelta * 10.f;
	}
	__super::Tick(fTimeDelta);
}

void CCamera_Main::Late_Tick(_float fTimeDelta)
{
}

void CCamera_Main::Camera_Zoom(_float fTimeDelta)
{
	_vec4 vCurrentPos = m_pTransformCom->Get_State(State::Pos);

	float lerpFactor = 0.1f;

	_vec3 vTargetPos = m_pGameInstance->Get_CameraTargetPos();
	vTargetPos.z = vTargetPos.z - 3.f;

	_vec4 vNewPos = XMVectorLerp(vCurrentPos, vTargetPos, lerpFactor);

	m_pTransformCom->Set_State(State::Pos, vNewPos);

}

void CCamera_Main::Select_Mode(_float fTimeDelta)
{
	if (!m_bInitMode[CM_SELECT])
	{
		m_pTransformCom->Set_State(State::Pos, _vec4(-0.03993677f, 1.398446296f, -5.207254f, 1.f));
		m_pTransformCom->LookAt_Dir(_vec4(-0.049540625f, -0.10697676f, 0.993027f, 0.f));

		m_bInitMode[CM_SELECT] = true;
		return;
	}

	CAMERA_STATE eState = (CAMERA_STATE)m_pGameInstance->Get_CameraState();
	switch (eState)
	{
	case CAMERA_STATE::CM_DEFAULT:
	{
		_vec4 vCurrentPos = m_pTransformCom->Get_State(State::Pos);
		_vec4 vTargetPos = _vec4(-0.03993677f, 1.398446296f, -5.207254f, 1.f);

		_vec4 vNewPos = XMVectorLerp(vCurrentPos, vTargetPos, 0.1f);
		m_pTransformCom->Set_State(State::Pos, vNewPos);


		_vec4 vCurLook = m_pTransformCom->Get_State(State::Look);
		_vec4 vTargetLook = _vec4(-0.049540625f, -0.10697676f, 0.993027f, 0.f);

		_vec4 vNewLook = XMVectorLerp(vCurLook, vTargetLook, 0.1f);
		m_pTransformCom->LookAt_Dir(vNewLook);
		break;
	}
	case CAMERA_STATE::CM_ZOOM:
	{

		_vec4 vCurLook = m_pTransformCom->Get_State(State::Look);
		_vec4 vTargetLook = {};
		if (m_pGameInstance->Have_TargetLook())
		{
			_vec4 vCurrentPos = m_pTransformCom->Get_State(State::Pos);
			_vec4 vTargetPos = m_pGameInstance->Get_CameraTargetPos();

			_float fLerpFactor = 0.1f;
			_float fZoomFactor = m_pGameInstance->Get_ZoomFactor();

			_vec4 vNewPos = XMVectorLerp(vCurrentPos, vTargetPos, fLerpFactor);
			m_pTransformCom->Set_State(State::Pos, vNewPos);

			vTargetLook = m_pGameInstance->Get_CameraTargetLook();
			_vec4 vNewLook = XMVectorLerp(vCurLook, vTargetLook, fLerpFactor);
			m_pTransformCom->LookAt_Dir(vNewLook);
		}
		else
		{
			_vec4 vCurrentPos = m_pTransformCom->Get_State(State::Pos);
			_vec4 vTargetPos = m_pGameInstance->Get_CameraTargetPos();

			_float fLerpFactor = 0.1f;
			_float fZoomFactor = m_pGameInstance->Get_ZoomFactor();

			vTargetPos.y = vTargetPos.y + 1.5f;
			vTargetPos.z = vTargetPos.z - fZoomFactor;

			_vec4 vNewPos = XMVectorLerp(vCurrentPos, vTargetPos, fLerpFactor);
			m_pTransformCom->Set_State(State::Pos, vNewPos);

			vTargetLook = m_pGameInstance->Get_CameraTargetPos();
			vTargetLook.y = 0.f;
			_vec4 vNewLook = XMVectorLerp(vCurLook, vTargetLook, fLerpFactor);
			m_pTransformCom->LookAt_Dir(vNewLook);
		}

		break;
	}
	default:
		break;
	}
}

void CCamera_Main::Custom_Mode(_float fTimeDelta)
{
	if (!m_bInitMode[CM_CUSTOM])
	{
		m_pTransformCom->Set_State(State::Pos, _vec4(-0.694085598f, 6.33564663f, 1.72800910f, 1.0f));
		m_pTransformCom->LookAt_Dir(_vec4(0.256537676, -0.0349416211f, -0.965888619f, 0.f));

		m_bInitMode[CM_CUSTOM] = true;
		return;
	}

	CAMERA_STATE eState = (CAMERA_STATE)m_pGameInstance->Get_CameraState();
	switch (eState)
	{
	case CAMERA_STATE::CM_DEFAULT:
	{
		_vec4 vCurrentPos = m_pTransformCom->Get_State(State::Pos);
		_vec4 vTargetPos = _vec4(-0.694085598f, 6.33564663f, 1.72800910f, 1.0f);

		_vec4 vNewPos = XMVectorLerp(vCurrentPos, vTargetPos, 0.1f);
		m_pTransformCom->Set_State(State::Pos, vNewPos);

		break;
	}
	case CAMERA_STATE::CM_ZOOM:
	{
		_vec4 vCurrentPos = m_pTransformCom->Get_State(State::Pos);

		_float fLerpFactor = 0.1f;

		_vec4 vTargetPos = m_pGameInstance->Get_CameraTargetPos();

		_float fZoomFactor = m_pGameInstance->Get_ZoomFactor();

		vTargetPos.y = vTargetPos.y + 1.5f;
		vTargetPos.z = vTargetPos.z - fZoomFactor;

		_vec4 vNewPos = XMVectorLerp(vCurrentPos, vTargetPos, fLerpFactor);
		m_pTransformCom->Set_State(State::Pos, vNewPos);
		break;
	}
	default:
		break;
	}
}

CCamera_Main* CCamera_Main::Create(_dev pDevice, _context pContext)
{
	CCamera_Main* pInstance = new CCamera_Main(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCamera_Main");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Main::Clone(void* pArg)
{
	CCamera_Main* pInstance = new CCamera_Main(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCamera_Main");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Main::Free()
{
	__super::Free();

	Safe_Release(m_pPlayerTransform);
}
