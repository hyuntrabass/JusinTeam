#include "Dead.h"

#include "Effect_Manager.h"

CDead::CDead(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CDead::CDead(const CDead& rhs)
	: CMonster(rhs)
{
}

HRESULT CDead::Init_Prototype()
{
    return S_OK;
}

HRESULT CDead::Init(void* pArg)
{
	if (pArg)
	{
		DEAD_DESC Desc = *(DEAD_DESC*)pArg;
		m_eDead = Desc.eDead;

		switch (m_eDead)
		{
		case Client::CDead::VOID01:
			m_strModelTag = TEXT("Prototype_Model_Void01_Die");
			break;
		case Client::CDead::VOID05:
			m_strModelTag = TEXT("Prototype_Model_Void05_Die");
			break;
		case Client::CDead::VOID09:
			m_strModelTag = TEXT("Prototype_Model_Void09_Die");
			break;
		}

		if (FAILED(__super::Add_Components()))
		{
			return E_FAIL;
		}

		m_pTransformCom->Set_State(State::Pos, Desc.vPos);
	}

	m_Animation.iAnimIndex = 0;
	m_Animation.isLoop = false;
	m_Animation.fAnimSpeedRatio = 2.f;

	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	vPlayerPos.y = m_pTransformCom->Get_State(State::Pos).y;

	m_pTransformCom->LookAt(vPlayerPos);

	_mat m_EffectMatrix = _mat::CreateScale(4.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)) + _vec3(0.f, 1.f, 0.f));

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"BloodPop04");
	Info.pMatrix = &m_EffectMatrix;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

    return S_OK;
}

void CDead::Tick(_float fTimeDelta)
{
	m_fLifeTime += fTimeDelta;
	
	if (m_fLifeTime >= 30.f)
	{
		Kill();
	}

	if (m_pModelCom->IsAnimationFinished(0))
	{
		if (m_eDead == VOID01)
		{
			if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY)
			{

				m_DC = GetDC(g_hWnd);

				m_BackDC = CreateCompatibleDC(m_DC);

				m_hBackBit = CreateCompatibleBitmap(m_DC, g_iWinSizeX, g_iWinSizeY);

				m_hOldBackBit = (HBITMAP)SelectObject(m_BackDC, m_hBackBit);

				m_hVideo = MCIWndCreate(g_hWnd, NULL, WS_CHILD | WS_VISIBLE | MCIWNDF_NOPLAYBAR
					, L"../Bin/Resources/Video/Tutorial1.wmv");

				MCIWndSetVolume(g_hWnd, 1.f);

				MoveWindow(m_hVideo, 0, 0, g_iWinSizeX, g_iWinSizeY, FALSE);

				MCIWndPlay(m_hVideo);
				m_pGameInstance->Video_Start(11.f, true);
			}

		}
		m_pTransformCom->Go_Down(fTimeDelta * 0.05f);
	}


	m_pModelCom->Set_Animation(m_Animation);
}

void CDead::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CDead::Render()
{
	__super::Render();

    return S_OK;
}

CDead* CDead::Create(_dev pDevice, _context pContext)
{
	CDead* pInstance = new CDead(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CDead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDead::Clone(void* pArg)
{
	CDead* pInstance = new CDead(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CDead");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDead::Free()
{
	__super::Free();
	MCIWndClose(m_hVideo);
	SelectObject(m_BackDC, m_hOldBackBit); //DC에 원래 설정을 돌려줍니다.
	DeleteDC(m_BackDC);  // 메모리를 반환합니다.
	DeleteObject(m_hBackBit); // 메모리를 반환합니다.
	ReleaseDC(g_hWnd, m_DC); // 윈도우에 DC 해제를 요청합니다.
}
