#include "Camera_CutScene.h"

CCamera_CutScene::CCamera_CutScene(_dev pDevice, _context pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_CutScene::CCamera_CutScene(const CCamera_CutScene& rhs)
	: CCamera(rhs)
	//, m_pImGui_Manager(CImGui_Manager::Get_Instance())
{
	//Safe_AddRef(m_pImGui_Manager);
}

HRESULT CCamera_CutScene::Init_Prototype()
{
	return S_OK;
}

HRESULT CCamera_CutScene::Init(void* pArg)
{

	if (not m_pTransformCom)
	{
		MSG_BOX("Can't Find Player!! : Camera Main");
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

void CCamera_CutScene::Tick(_float fTimeDelta)
{

	if (m_pGameInstance->Get_CameraModeIndex() != CM_MAIN)
	{
		return;
	}
	m_pGameInstance->Set_CameraNF(_float2(m_fNear, m_fFar));

	if (m_pGameInstance->Key_Down(DIK_P))
	{
		m_pGameInstance->Set_CameraModeIndex(CM_DEBUG);
	}

	_long dwMouseMove;

	if (m_pGameInstance->Mouse_Pressing(DIM_MBUTTON))
	{
		if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta / m_pGameInstance->Get_TimeRatio() * dwMouseMove * m_fMouseSensor);
		}

		if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(State::Right), fTimeDelta / m_pGameInstance->Get_TimeRatio() * dwMouseMove * m_fMouseSensor);
		}
	}
	
	_float iSpeed;
	if (iSpeed = m_pGameInstance->Get_MouseMove(MouseState::wheel) > 0)
	{
		if(m_fSpeed < 100)
			m_fSpeed += iSpeed * 2;
		/*m_pTransformCom->Go_Straight(fTimeDelta);*/
	}
	else if (iSpeed = m_pGameInstance->Get_MouseMove(MouseState::wheel) < 0)
	{
		if(m_fSpeed >= 4)
			m_fSpeed += iSpeed * -2;
		//m_pTransformCom->Go_Backward(fTimeDelta);
	}

	// 	y = sin(x * 10.0f) * powf(0.5f, x)

	if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		m_pTransformCom->Go_Straight(fTimeDelta * m_fSpeed);
	}
	if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		m_pTransformCom->Go_Backward(fTimeDelta * m_fSpeed);
	}
	if (m_pGameInstance->Key_Pressing(DIK_A))
	{
		m_pTransformCom->Go_Left(fTimeDelta * m_fSpeed);
	}
	if (m_pGameInstance->Key_Pressing(DIK_D))
	{
		m_pTransformCom->Go_Right(fTimeDelta * m_fSpeed);
	}
	if (m_pGameInstance->Key_Pressing(DIK_Q))
	{
		m_pTransformCom->Go_Up(fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing(DIK_Z))
	{
		m_pTransformCom->Go_Down(fTimeDelta);
	}


	__super::Tick(fTimeDelta);
}

void CCamera_CutScene::Late_Tick(_float fTimeDelta)
{
}

CCamera_CutScene* CCamera_CutScene::Create(_dev pDevice, _context pContext)
{
	CCamera_CutScene* pInstance = new CCamera_CutScene(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCamera_CutScene");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_CutScene::Clone(void* pArg)
{
	CCamera_CutScene* pInstance = new CCamera_CutScene(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCamera_CutScene");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_CutScene::Free()
{
	__super::Free();
	//Safe_Release(m_pImGui_Manager);
	Safe_Release(m_pPlayerTransform);
}
