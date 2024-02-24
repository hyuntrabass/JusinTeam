#include "SurvivalGame.h"

#include "Launcher.h"

CSurvivalGame::CSurvivalGame(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CSurvivalGame::CSurvivalGame(const CSurvivalGame& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSurvivalGame::Init_Prototype()
{
    return S_OK;
}

HRESULT CSurvivalGame::Init(void* pArg)
{
	m_eCurPattern = PATTERN_RANDOM_MISSILE;

    return S_OK;
}

void CSurvivalGame::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_LEFT))
	{
		Kill();
	}

	if (m_pGameInstance->Key_Down(DIK_UP))
	{
		for (size_t i = 0; i < 5; i++)
		{
			CLauncher::LAUNCHER_TYPE eType = CLauncher::TYPE_RANDOM_POS;
			m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Launcher"), TEXT("Prototype_GameObject_Launcher"), &eType);
		}
	}

	Init_Pattern(fTimeDelta);
	Tick_Pattern(fTimeDelta);
}

void CSurvivalGame::Late_Tick(_float fTimeDelta)
{
}

HRESULT CSurvivalGame::Render()
{
    return S_OK;
}

void CSurvivalGame::Init_Pattern(_float fTimeDelta)
{
	if (m_ePrePattern != m_eCurPattern)
	{
		switch (m_eCurPattern)
		{
		case Client::CSurvivalGame::PATTERN_INIT:
			
			m_fTime = 0.f;

			break;
		case Client::CSurvivalGame::PATTERN_RANDOM_MISSILE:
			break;
		case Client::CSurvivalGame::PATTERN_FLOOR:
			break;
		case Client::CSurvivalGame::PATTERN_GUIDED_MISSILE:
			break;
		case Client::CSurvivalGame::PATTERN_LASER:
			break;
		case Client::CSurvivalGame::PATTERN_PIZZA:
			break;
		case Client::CSurvivalGame::PATTERN_TANGHURU:
			break;
		case Client::CSurvivalGame::PATTERN_SUICIDE_MONSTER:
			break;
		}

		m_ePrePattern = m_eCurPattern;
	}
}

void CSurvivalGame::Tick_Pattern(_float fTimeDelta)
{
	switch (m_eCurPattern)
	{
	case Client::CSurvivalGame::PATTERN_INIT:
		break;
	case Client::CSurvivalGame::PATTERN_RANDOM_MISSILE:
		break;
	case Client::CSurvivalGame::PATTERN_FLOOR:
		break;
	case Client::CSurvivalGame::PATTERN_GUIDED_MISSILE:
		break;
	case Client::CSurvivalGame::PATTERN_LASER:
		break;
	case Client::CSurvivalGame::PATTERN_PIZZA:
		break;
	case Client::CSurvivalGame::PATTERN_TANGHURU:
		break;
	case Client::CSurvivalGame::PATTERN_SUICIDE_MONSTER:
		break;
	}
}

CSurvivalGame* CSurvivalGame::Create(_dev pDevice, _context pContext)
{
	CSurvivalGame* pInstance = new CSurvivalGame(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSurvivalGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSurvivalGame::Clone(void* pArg)
{
	CSurvivalGame* pInstance = new CSurvivalGame(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSurvivalGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSurvivalGame::Free()
{
	__super::Free();
}
