#include "Camera_Main.h"
#include "UI_Manager.h"
#include "FadeBox.h"

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
	for (_int i = 0; i < CM_END; i++)
	{
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
	{
		return;
	}

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
		if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY)
		{
			if (m_pPlayerTransform == nullptr)
			{
				m_pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
				if (not m_pPlayerTransform)
				{
					return;
				}
				Safe_AddRef(m_pPlayerTransform);
				m_pTransformCom->Set_State(State::Pos, _vec4(m_pPlayerTransform->Get_CenterPos() + _vec4(0.f, 3.f, 0.f, 0.f)));
				m_vOriCamPos = _vec4(m_pPlayerTransform->Get_CenterPos()) + _vec4(0.f, 3.f, 0.f, 0.f);
				m_AimZoomInTime = 2.f;
			}
		}
		if (m_pPlayerTransform == nullptr)
		{
			return;
		}
		if (m_pGameInstance->Key_Down(DIK_M))
		{
			if (m_pGameInstance->Get_CameraState() == CS_ZOOM)
			{
				return;
			}
			if (m_pGameInstance->Get_CurrentLevelIndex() > LEVEL_GAMEPLAY)
			{
				if (m_pGameInstance->Get_CameraState() != CS_WORLDMAP)
				{
					m_pGameInstance->Play_Sound(TEXT("WorldMap_Click"), 0.6f);

					CFadeBox::FADE_DESC Desc = {};
					Desc.eState = CFadeBox::FADEOUT;
					Desc.fDuration = 0.5f;
					if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FadeBox"), &Desc)))
					{
						return;
					}

					m_pGameInstance->Set_CameraState(CS_WORLDMAP);
					m_pTransformCom->Set_State(State::Pos, m_vMapPos);
					CUI_Manager::Get_Instance()->Set_FullScreenUI(true);
				}
				else
				{
					m_pGameInstance->Play_Sound(TEXT("WorldMap_Click"), 0.6f);
					CFadeBox::FADE_DESC Desc = {};
					Desc.eState = CFadeBox::FADEOUT;
					Desc.fDuration = 0.5f;
					if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FadeBox"), &Desc)))
					{
						return;
					}

					m_pGameInstance->Set_CameraState(CS_DEFAULT);
					CUI_Manager::Get_Instance()->Set_FullScreenUI(false);
				}
			}
		}

		if (m_pGameInstance->Get_CameraState() == CS_ZOOM)
		{
			ZOOM_Mode(fTimeDelta);
			__super::Tick(fTimeDelta);
			return;
		}
		if (m_pGameInstance->Get_CameraState() == CS_WORLDMAP)
		{
			WorldMap_Mode(fTimeDelta);
			__super::Tick(fTimeDelta);
			return;
		}
		if (m_pGameInstance->Get_CameraState() == CS_SKILLBOOK)
		{
			SkillBook_Mode(fTimeDelta);
			__super::Tick(fTimeDelta);
			return;
		}

		if (m_pGameInstance->Get_CameraState() == CS_SHOP)
		{
			Shop_Mode(fTimeDelta);
			__super::Tick(fTimeDelta);
			return;
		}
		if (m_pGameInstance->Get_CameraState() == CS_INVEN)
		{
			Inven_Mode(fTimeDelta);
			__super::Tick(fTimeDelta);
			return;
		}
		if (m_pGameInstance->Get_CameraState() == CS_ENDFULLSCREEN)
		{
			m_pGameInstance->Set_CameraState(CS_DEFAULT);

			CFadeBox::FADE_DESC Desc = {};
			Desc.eState = CFadeBox::FADEOUT;
			Desc.fDuration = 1.f;
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FadeBox"), &Desc)))
			{
				return;
			}
			_vec4 vPos = m_pPlayerTransform->Get_State(State::Pos);
			vPos.y += 3.f;
			vPos.z -= 5.f;
			m_pTransformCom->Set_State(State::Pos, vPos);
			m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(State::Pos));
			__super::Tick(fTimeDelta);
			return;
		}

		if (m_fShakeAcc > 2.f and m_pGameInstance->Get_ShakeCam())
		{
			m_fShakeAcc = m_pGameInstance->Get_ShakePower();
			m_pGameInstance->Set_ShakeCam(false);
		}
		if (m_pGameInstance->Get_FlyCam())
		{
			m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(State::Pos));
		}
		else if (!m_pGameInstance->Get_AimMode())
		{
			if (m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
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

			}


			if (!CUI_Manager::Get_Instance()->Is_Picking_UI())
			{
				if (m_pGameInstance->Get_MouseMove(MouseState::wheel) > 0)
				{
					if (m_fPlayerDistance > 2.f)
					{
						m_fPlayerDistance -= 0.8f;
					}

					if (m_fLerpTime >= 1.f)
					{
						m_fLerpTime = 0.f;
					}
				}
				else if (m_pGameInstance->Get_MouseMove(MouseState::wheel) < 0)
				{
					if (m_fPlayerDistance < 10.f)
					{
						m_fPlayerDistance += 0.8f;
					}

					if (m_fLerpTime >= 1.f)
					{
						m_fLerpTime = 0.f;
					}
				}
			}


			// 	y = sin(x * 10.0f) * powf(0.5f, x)

			_float fShakeAmount = sin(m_fShakeAcc * 15.f) * powf(0.5f, m_fShakeAcc) * 0.2f;
			if (m_fLerpTime < 1.f)
			{
				m_fLerpTime += (fTimeDelta * 3.f);
				m_fLerpDistance = Lerp(m_fFirstDistance, m_fPlayerDistance, m_fLerpTime);
			}
			else
			{
				m_fFirstDistance = m_fPlayerDistance;
				m_fLerpDistance = m_fFirstDistance;

			}

			m_AimZoomInTime += fTimeDelta * 1.5f;

			_float CamAttackZoom = m_fLerpDistance - m_pGameInstance->Get_CameraAttackZoom();

			_float vZoomY = 1.3f - (CamAttackZoom * 0.25f);
			_vec4 vCamPos = (m_pPlayerTransform->Get_CenterPos()) + _vec4(0.f, vZoomY, 0.f, 0.f)
				- (m_pTransformCom->Get_State(State::Look) * CamAttackZoom)
				+ (m_pTransformCom->Get_State(State::Up) * CamAttackZoom * 0.15f);

			m_vOriCamPos = XMVectorLerp(m_vOriCamPos, vCamPos, 0.3f);

			_vec4 OriCam{};
			if (m_AimZoomInTime < 1.f)
			{
				OriCam = XMVectorLerp(m_vAimCamPos, m_vOriCamPos, m_AimZoomInTime);
			}
			else
			{
				OriCam = m_vOriCamPos;
			}

			m_pTransformCom->Set_State(State::Pos, OriCam);


			_vec4 vLook = m_pTransformCom->Get_State(State::Look);
			PxRaycastBuffer Buffer{};
			_vec4 vRayDir{};
			_vec4 vMyPos = m_pTransformCom->Get_State(State::Pos);
			_vec4 PlayerCenter = m_pPlayerTransform->Get_CenterPos();
			_float ps = vMyPos.y - PlayerCenter.y;
			if (ps < 0.f)
			{
				ps = 0.f;
			}

			ps *= 0.2f;
			vRayDir = vMyPos - PlayerCenter;
			_float fDist = XMVectorGetX(XMVector3Length(vRayDir)) - 0.4f;
			vRayDir.Normalize();
			PxQueryFilterData Filter;
			Filter.data.word0 = ~COLGROUP_PLAYER;


			if (m_pGameInstance->Raycast(m_pPlayerTransform->Get_CenterPos() /*+ vRayDir * (0.5f + ps)*/, vRayDir, fDist, Buffer))
			{
				m_pTransformCom->Set_State(State::Pos, PxVec3ToVector(Buffer.block.position, 1.f));
			}

			_vec4 vShakePos = m_pTransformCom->Get_State(State::Pos);
			vShakePos += XMVectorSet(fShakeAmount, -fShakeAmount, 0.f, 0.f);
			m_pTransformCom->Set_State(State::Pos, vShakePos);
			m_fShakeAcc += fTimeDelta * 10.f / m_pGameInstance->Get_TimeRatio();
			m_AimZoomOutTime = 0.f;
		}
		else
		{
			m_fPlayerDistance = 7.f;
			_long dwMouseMove;
			if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
			{
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta / m_pGameInstance->Get_TimeRatio() * dwMouseMove * m_fMouseSensor);
			}

			if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::y))
			{
				m_pTransformCom->Turn(m_pTransformCom->Get_State(State::Right), fTimeDelta / m_pGameInstance->Get_TimeRatio() * dwMouseMove * m_fMouseSensor);
			}

			m_AimZoomOutTime += fTimeDelta * 4.f;
			_vec4 vMeLook = m_pTransformCom->Get_State(State::Look);
			_vec4 PlayerRight = m_pPlayerTransform->Get_State(State::Right).Get_Normalized();
			_vec4 PlayerUp = m_pPlayerTransform->Get_State(State::Up).Get_Normalized();
			_vec3 AimPos = m_pGameInstance->Get_AimPos();
			m_vAimCamPos = m_pPlayerTransform->Get_CenterPos() - (vMeLook * AimPos.z * 1.3f)
				+ (PlayerUp * AimPos.y * 0.5f) + (PlayerRight * AimPos.x);

			_vec4 OriCam{};
			if (m_AimZoomOutTime < 1.f)
			{
				OriCam = XMVectorLerp(m_vOriCamPos, m_vAimCamPos, m_AimZoomOutTime);
			}
			else
			{
				OriCam = m_vAimCamPos;
			}
			_float fShakeAmount = sin(m_fShakeAcc * 15.f) * powf(0.5f, m_fShakeAcc) * 0.2f;
			m_pTransformCom->Set_State(State::Pos, OriCam);
			_vec4 vShakePos = m_pTransformCom->Get_State(State::Pos);
			vShakePos += XMVectorSet(fShakeAmount, -fShakeAmount, 0.f, 0.f);
			m_pTransformCom->Set_State(State::Pos, vShakePos);
			m_fShakeAcc += fTimeDelta * 10.f / m_pGameInstance->Get_TimeRatio();
			m_AimZoomInTime = 0.f;

		}
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



_bool CCamera_Main::Inven_Mode(_float fTimeDelta)
{
	_vec4 vInvenPos = { -0.3f, 1001.2f, 2.1f, 1.f };
	m_pTransformCom->Set_State(State::Pos, vInvenPos);
	m_pTransformCom->LookAt_Dir(_vec4(-0.0467762f, -0.07077519f, -0.996398f, 0.f));

	return true;
}

void CCamera_Main::Shop_Mode(_float fTimeDelta)
{
	CTransform* pTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_VILLAGE, TEXT("Layer_ItemMerchant"), TEXT("Com_Transform"));
	_vec4 vPos = pTransform->Get_State(State::Pos);
	_vec4 vShopPos = _vec4(vPos.x + 0.05f, 1001.4f, vPos.z + 0.9f, 1.f);

	m_pTransformCom->Set_State(State::Pos, vShopPos);
	m_pTransformCom->LookAt_Dir(_vec4(-0.0949012339f, -0.0923041627f, -0.991196573f, 0.f));

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
	case CAMERA_STATE::CS_DEFAULT:
	{
		_vec4 vCurrentPos = m_pTransformCom->Get_State(State::Pos);
		_vec4 vTargetPos = _vec4(-0.03993677f, 1.398446296f, -5.207254f, 1.f);

		_vec4 vNewPos = XMVectorLerp(vCurrentPos, vTargetPos, 0.1f);
		m_pTransformCom->Set_State(State::Pos, vNewPos);


		_vec4 vCurLook = m_pTransformCom->Get_State(State::Look);
		_vec4 vTargetLook = _vec4(-0.049540625f, -0.10697676f, 0.993027f, 0.f);

		_vec4 vNewLook = XMVectorLerp(vCurLook, vTargetLook, 0.1f);
		m_pTransformCom->LookAt_Dir(vNewLook);
		m_bZoomEnd = false;
		break;
	}

	case CAMERA_STATE::CS_ZOOM:
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
			if (vNewLook == vTargetLook)
			{
				m_bZoomEnd = true;
			}
			else
				m_bZoomEnd = false;
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
			if (vNewLook == vTargetLook)
			{
				m_bZoomEnd = true;
			}
			else
				m_bZoomEnd = false;
		}

		break;
	}
	default:
		break;
	}


	_vec4 OriPos = m_pTransformCom->Get_State(State::Pos);
	m_CurrentTime = GetTickCount() * 0.001f;


	float swayX = (sin(m_CurrentTime * m_SwaySpeed) * m_SwayAmount) * 0.0008f;
	float swayY = (sin((m_CurrentTime + m_TimeOffset) * m_SwaySpeed) * m_SwayAmount) * 0.0008f;

	m_fSelectRotation += (fTimeDelta * 0.8f * m_iRotation);

	if (m_fSelectRotation > 2.f)
		m_iRotation *= -1;
	else if (m_fSelectRotation < -2.f)
	{
		m_iRotation *= -1;
	}


	_vec4 vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	_vec4 vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	_vec4 vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f);

	if (eState == CAMERA_STATE::CS_DEFAULT)
		m_pTransformCom->Rotation(_vec4(0.f, 0.f, 1.f, 0.f), m_fSelectRotation);
	else if (m_bZoomEnd)
		m_pTransformCom->Rotation(_vec4(0.f, 0.f, 1.f, 0.f), m_fSelectRotation);

	m_pTransformCom->Set_State(State::Pos, OriPos + (m_pTransformCom->Get_State(State::Right) * swayX)
		+ m_pTransformCom->Get_State(State::Up) * swayY);

}

void CCamera_Main::Custom_Mode(_float fTimeDelta)
{
	if (!m_bInitMode[CM_CUSTOM])
	{
		m_pTransformCom->Set_State(State::Pos, _vec4(-0.694085598f, 6.33564663f, 1.72800910f, 1.0f));
		m_pTransformCom->LookAt_Dir(_vec4(0.256537676f, -0.0349416211f, -0.965888619f, 0.f));

		m_bInitMode[CM_CUSTOM] = true;
		return;
	}
	CAMERA_STATE eState = (CAMERA_STATE)m_pGameInstance->Get_CameraState();
	switch (eState)
	{
	case CAMERA_STATE::CS_DEFAULT:
	{
		_vec4 vCurrentPos = m_pTransformCom->Get_State(State::Pos);
		_vec4 vTargetPos = _vec4(-0.694085598f, 6.33564663f, 1.72800910f, 1.0f);

		_vec4 vNewPos = XMVectorLerp(vCurrentPos, vTargetPos, 0.1f);
		m_pTransformCom->Set_State(State::Pos, vNewPos);

		break;
	}
	case CAMERA_STATE::CS_ZOOM:
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

void CCamera_Main::ZOOM_Mode(_float fTimeDelta)
{
	_vec4 vCurLook = m_pTransformCom->Get_State(State::Look);;

	_vec4 vCurrentPos = m_pTransformCom->Get_State(State::Pos);
	_vec4 vTargetPos = m_pGameInstance->Get_CameraTargetPos();
	_vec4 vTargetLook = m_pGameInstance->Get_CameraTargetLook();
	_vec4 vPlayerRight = m_pPlayerTransform->Get_State(State::Right);
	_vec4 vPlayerUp = m_pPlayerTransform->Get_State(State::Up);
	_vec4 vPlayerLook = m_pPlayerTransform->Get_State(State::Look);

	_vec4 vPlayerPos = vTargetPos + vTargetLook * 1.5f;

	float swayX = (sin(m_CurrentTime * m_SwaySpeed) * m_SwayAmount) * 0.003f;
	float swayY = (sin((m_CurrentTime + m_TimeOffset) * m_SwaySpeed) * m_SwayAmount) * 0.003f;

	m_fCamChangeTime += fTimeDelta;
	if (m_fCamChangeTime > 5.f)
	{
		m_fCamChangeTime = 0;
		++m_iCamChange;
		if (m_iCamChange >= 3)
			m_iCamChange = 0;
	}
	if (m_iCamChange == 2)
	{
		m_pPlayerTransform->Set_State(State::Pos, vPlayerPos);
		m_pPlayerTransform->LookAt(vTargetPos + vPlayerRight * -0.1f);


		m_pTransformCom->Set_State(State::Pos, vPlayerPos +
			vPlayerRight * 1.4f +
			vPlayerUp * 1.2f -
			vPlayerLook * 0.2f);

		m_pTransformCom->LookAt(vTargetPos + vTargetLook + vTargetLook * -swayX + vPlayerUp * 1.5f + vPlayerUp * swayY);
	}
	else if (m_iCamChange == 1)
	{
		m_pPlayerTransform->Set_State(State::Pos, vPlayerPos);
		m_pPlayerTransform->LookAt(vTargetPos);


		m_pTransformCom->Set_State(State::Pos, vPlayerPos +
			vPlayerRight * -0.4f +
			vPlayerUp * 1.6f -
			vPlayerLook * 0.2f);

		m_pTransformCom->LookAt(vTargetPos + vTargetLook + vTargetLook * -swayX + vPlayerUp * 1.5f + vPlayerUp * swayY);
	}
	else
	{
		m_pPlayerTransform->Set_State(State::Pos, vPlayerPos);
		m_pPlayerTransform->LookAt(vTargetPos + vPlayerRight * -0.1f);


		m_pTransformCom->Set_State(State::Pos, vPlayerPos +
			vPlayerRight * 1.f +
			vPlayerUp * 1.8f -
			vPlayerLook * 0.47f);

		m_pTransformCom->LookAt(vTargetPos + vTargetLook + vTargetLook * -swayX + vPlayerUp * 1.7f + vPlayerUp * swayY);
	}
	m_CurrentTime = GetTickCount() * 0.001f;
	m_fSelectRotation += (fTimeDelta * 0.8f * m_iRotation);

	_vec4 OriPos = m_pTransformCom->Get_State(State::Pos);

	if (m_fSelectRotation > 2.f)
		m_iRotation *= -1;
	else if (m_fSelectRotation < -2.f)
	{
		m_iRotation *= -1;
	}

	m_pTransformCom->Set_State(State::Pos, OriPos + (m_pTransformCom->Get_State(State::Right) * swayX));

}

void CCamera_Main::SkillBook_Mode(_float fTimeDelta)
{
	_vec4 vShopPos = _vec4(2.38035f, 202.068f, -2.18231f, 1.f);

	m_pTransformCom->Set_State(State::Pos, vShopPos);

	m_pTransformCom->LookAt_Dir(_vec4(-0.514929f, -0.221083f, 0.828224f, 0.f));
}

void CCamera_Main::WorldMap_Mode(_float fTimeDelta)
{
	CTransform* worldmap = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_WorldMap"), TEXT("Com_Transform")));

	//m_pTransformCom->Set_State(State::Pos, worldmap->Get_State(State::Pos)+
	//	PlayerUp * 3);

	m_pTransformCom->LookAt_Dir(_vec4(0.0287023f, -0.910154294f, -0.413268f, 0.f));

	if (m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
	{
		_long dwMouseMove;
		if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
		{
			if (dwMouseMove < 0.f)
			{
				if (m_fMap_RightDistance > -11.f + (m_fLerp_LookDistance) * 0.8f)
					m_fMap_RightDistance -= (0.5f + (m_fLerp_LookDistance * 0.043f));
			}
			else if (dwMouseMove > 0.f)
			{
				if (m_fMap_RightDistance < 1.f + (m_fLerp_LookDistance) * -0.8f)
					m_fMap_RightDistance += (0.5f + (m_fLerp_LookDistance * 0.043f));
			}

		}

		if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::y))
		{

			if (dwMouseMove > 0.f)
			{
				if (m_fMap_UpDistance < 0.f + (m_fLerp_LookDistance) * -0.8f)
					m_fMap_UpDistance += (0.5f + (m_fLerp_LookDistance * 0.043f));
			}
			else if (dwMouseMove < 0.f)
			{

				if (m_fMap_UpDistance > -20.f + (m_fLerp_LookDistance) * 0.8f)
					m_fMap_UpDistance -= (0.5f + (m_fLerp_LookDistance * 0.043f));
			}

		}


	}
	m_fLerp_RightDistance = Lerp(m_fLerp_RightDistance, m_fMap_RightDistance, 0.2f);
	m_fLerp_UpDistance = Lerp(m_fLerp_UpDistance, m_fMap_UpDistance, 0.2f);
	m_fLerp_LookDistance = Lerp(m_fLerp_LookDistance, m_fMap_LookDistance, 0.4f);

	if (m_pGameInstance->Get_MouseMove(MouseState::wheel) > 0)
	{
		if (m_fMap_LookDistance > -6.f)
			m_fMap_LookDistance -= 0.7f;

		m_pTransformCom->Set_State(State::Pos, m_vMapPos - m_pTransformCom->Get_State(State::Look) * m_fLerp_LookDistance);
		m_fCurrentMapZoom = m_pTransformCom->Get_State(State::Pos).y;
	}
	else if (m_pGameInstance->Get_MouseMove(MouseState::wheel) < 0)
	{

		if (m_fMap_LookDistance < 0.f)
			m_fMap_LookDistance += 0.7f;
		_float m_LerpDistance{};

		m_pTransformCom->Set_State(State::Pos, m_vMapPos - m_pTransformCom->Get_State(State::Look) * m_fLerp_LookDistance);
		m_fCurrentMapZoom = m_pTransformCom->Get_State(State::Pos).y;
	}



	m_pTransformCom->Set_State(State::Pos, m_vMapPos -
		(m_pTransformCom->Get_State(State::Right) * (m_fLerp_RightDistance)) +
		(m_pTransformCom->Get_State(State::Up) * m_fLerp_UpDistance)
	);

	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
	vPos.y = m_fCurrentMapZoom;
	m_pTransformCom->Set_State(State::Pos, vPos);
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
