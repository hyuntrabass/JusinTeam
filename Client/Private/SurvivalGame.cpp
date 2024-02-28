#include "SurvivalGame.h"

#include "Launcher.h"
#include "Projectile.h"

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
	m_eCurPattern = PATTERN_INIT;

	return S_OK;
}

void CSurvivalGame::Tick(_float fTimeDelta)
{
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
	CLauncher::LAUNCHER_TYPE eType = { CLauncher::TYPE_END };

	if (m_ePrePattern != m_eCurPattern)
	{
		switch (m_eCurPattern)
		{
		case Client::CSurvivalGame::PATTERN_INIT:

			m_fTime = 0.f;
			m_iCount = 0;

			break;
		case Client::CSurvivalGame::PATTERN_RANDOM_MISSILE:
			break;
		case Client::CSurvivalGame::PATTERN_FLOOR:
			break;
		case Client::CSurvivalGame::PATTERN_GUIDED_MISSILE:
			break;
		case Client::CSurvivalGame::PATTERN_LASER:

			eType = CLauncher::TYPE_LASER;
			m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Launcher"), TEXT("Prototype_GameObject_Launcher"), &eType);

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
	m_fTime += fTimeDelta;

	random_device dev;
	_randNum RandomNumber(dev());

	switch (m_eCurPattern)
	{
	case Client::CSurvivalGame::PATTERN_INIT:

		if (m_pGameInstance->Key_Down(DIK_UP))
		{
			m_eCurPattern = PATTERN_LASER;
		}

		break;

	case Client::CSurvivalGame::PATTERN_RANDOM_MISSILE:

		if (m_fTime >= 3.f)
		{
			CLauncher::LAUNCHER_TYPE eType = CLauncher::TYPE_RANDOM_POS;
			m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Launcher"), TEXT("Prototype_GameObject_Launcher"), &eType);

			m_fTime = 0.f;
			++m_iCount;
		}

		if (m_iCount >= 3)
		{
			m_eCurPattern = PATTERN_INIT;
		}

		break;

	case Client::CSurvivalGame::PATTERN_FLOOR:

		if (m_fTime >= 1.f)
		{
			CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);

			CProjectile::PROJECTILE_DESC Desc = {};
			Desc.eType = CProjectile::TYPE_FLOOR;
			Desc.vStartPos = pPlayerTransform->Get_State(State::Pos);
			m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Projectile"), TEXT("Prototype_GameObject_Projectile"), &Desc);

			_randFloat Random = _randFloat(-1.f, 1.f);
			_vec3 vRandomDir = _vec3(Random(RandomNumber), 0.f, Random(RandomNumber)).Get_Normalized();

			Desc.eType = CProjectile::TYPE_FLOOR;
			Desc.vStartPos = CENTER_POS + (rand() % 8 + 1) * vRandomDir;
			m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Projectile"), TEXT("Prototype_GameObject_Projectile"), &Desc);
			Desc.vStartPos = CENTER_POS + (rand() % 8 + 1) * vRandomDir;
			m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Projectile"), TEXT("Prototype_GameObject_Projectile"), &Desc);

			m_fTime = 0.f;
			++m_iCount;
		}

		if (m_iCount >= 5)
		{
			m_eCurPattern = PATTERN_INIT;
		}

		break;

	case Client::CSurvivalGame::PATTERN_GUIDED_MISSILE:

		if (m_fTime >= 1.5f)
		{
			CProjectile::PROJECTILE_DESC Desc = {};
			Desc.eType = CProjectile::TYPE_GUIDED_MISSILE;

			for (size_t i = 0; i < 8; i++)
			{
				_randFloat Random = _randFloat(-1.f, 1.f);
				_vec3 vRandomDir = _vec3(Random(RandomNumber), 0.f, Random(RandomNumber)).Get_Normalized();

				Desc.vStartPos = _vec3(-2000.f, 0.f, 2000.f) + 10 * vRandomDir;
				m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Projectile"), TEXT("Prototype_GameObject_Projectile"), &Desc);
			}

			m_fTime = 0.f;
			++m_iCount;
		}

		if (m_iCount >= 10)
		{
			m_eCurPattern = PATTERN_INIT;
		}

		break;

	case Client::CSurvivalGame::PATTERN_LASER:

		if (m_pGameInstance->Key_Down(DIK_RIGHT))
		{
			m_eCurPattern = PATTERN_INIT;
		}

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
