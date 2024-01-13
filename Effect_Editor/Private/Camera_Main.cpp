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
	if (not pArg)
	{
		MSG_BOX("null Arg : CCamera_Main");
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
	if (m_pGameInstance->Mouse_Pressing(DIM_MBUTTON))
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

	if (m_pGameInstance->Get_MouseMove(MouseState::wheel) > 0)
	{
		m_fTargetDistance -= 1.f;
		if (m_fTargetDistance < 0.f)
		{
			m_fTargetDistance = 0.5f;
		}
	}
	else if (m_pGameInstance->Get_MouseMove(MouseState::wheel) < 0)
	{
		m_fTargetDistance += 1.f;
	}

	m_pTransformCom->Set_State(State::Pos, _vec4(0.f, 0.f, 0.f, 1.f) - (m_pTransformCom->Get_State(State::Look) * m_fTargetDistance));

	__super::Tick(fTimeDelta);
}

void CCamera_Main::Late_Tick(_float fTimeDelta)
{
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
}
