#include "SurvivalGame.h"

#include "Launcher.h"
#include "Projectile.h"
#include "Survival_Mon.h"
#include "Camera_Manager.h"
#include "Trigger_Manager.h"
#include "UI_Manager.h"

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
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_eCurStage = STAGE_INIT;
	m_eNextStage = STAGE01;

	return S_OK;
}

void CSurvivalGame::Tick(_float fTimeDelta)
{
	if (CTrigger_Manager::Get_Instance()->Get_CurrentSpot() != TS_SurvivalMap)
	{
		Kill();
	}

	if (!CUI_Manager::Get_Instance()->InfinityTower_UI(true, SURVIVAL))
	{
		return;
	}

	Init_Pattern(fTimeDelta);
	Tick_Pattern(fTimeDelta);

}

void CSurvivalGame::Late_Tick(_float fTimeDelta)
{
	//m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CSurvivalGame::Render()
{
	//m_pGameInstance->Render_Text(L"Font_Dialogue", m_strStage, _vec2(640.f, 100.f), 1.f, _vec4(1.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CSurvivalGame::Init_Pattern(_float fTimeDelta)
{
	CLauncher::LAUNCHER_TYPE eType = { CLauncher::TYPE_END };

	if (m_ePreStage != m_eCurStage)
	{
		switch (m_eCurStage)
		{
		case Client::CSurvivalGame::STAGE_INIT:

			m_fInitTime = 0.f;
			m_fTime[0] = 0.f;
			m_fTime[1] = 0.f;
			m_fTime[2] = 0.f;
			m_iCount = 0;

			break;

		case Client::CSurvivalGame::STAGE01:
			break;

		case Client::CSurvivalGame::STAGE02:

			break;
		case Client::CSurvivalGame::STAGE03:

			{
				for (size_t i = 0; i < 6; i++)
				{
					CLauncher::LAUNCHER_TYPE eType = CLauncher::TYPE_BARRICADE;
					m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Launcher"), TEXT("Prototype_GameObject_Launcher"), &eType);

				}

				CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
				pPlayerTransform->Set_FootPosition(CENTER_POS);

				m_pGameInstance->Play_Sound(TEXT("WD_3130_WideBring_01_SFX_02"));

			}

			break;
		case Client::CSurvivalGame::STAGE04:

			eType = CLauncher::TYPE_CANNON;
			m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Cannon"), TEXT("Prototype_GameObject_Launcher"), &eType);

			for (size_t i = 0; i < 4; i++)
			{
				CLauncher::LAUNCHER_TYPE eType = CLauncher::TYPE_BLUEGEM;
				m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_BlueGem"), TEXT("Prototype_GameObject_Launcher"), &eType);
			}

			break;
		case Client::CSurvivalGame::STAGE05:
			
			break;
		}

		m_ePreStage = m_eCurStage;
	}
}

void CSurvivalGame::Tick_Pattern(_float fTimeDelta)
{
	m_fTime[0] += fTimeDelta;
	m_fTime[1] += fTimeDelta;

	random_device dev;
	_randNum RandomNumber(dev());

	switch (m_eCurStage)
	{
	case Client::CSurvivalGame::STAGE_INIT:

		m_fInitTime += fTimeDelta;

		m_strStage = L"준비중";

		//if (m_pGameInstance->Key_Down(DIK_UP))
		//{
		//	m_eCurStage = STAGE05;
		//}

		if (m_bGameClear == true)
		{
			if (m_pGameInstance->Get_LayerSize(LEVEL_VILLAGE, TEXT("Layer_Survival_Mon")) <= 0)
			{
				CCamera_Manager::Get_Instance()->Set_RidingZoom(false);

				if (!CUI_Manager::Get_Instance()->InfinityTower_UI(false, SURVIVAL))
				{
					return;
				}
			}
		}

		if (m_pGameInstance->Key_Pressing(DIK_COMMA) && m_pGameInstance->Key_Pressing(DIK_PERIOD) && !m_bGameStart)
		{
			m_bGameStart = true;

			if (!CUI_Manager::Get_Instance()->InfinityTower_UI(true, SURVIVAL))
			{
				return;
			}
		}

		if (m_bGameStart == true)
		{
			CCamera_Manager::Get_Instance()->Set_RidingZoom(true);

			if (m_fInitTime >= 5.f)
			{
				m_eCurStage = m_eNextStage;
			}
		}

		break;

	case Client::CSurvivalGame::STAGE01:

		m_strStage = L"스테이지 1";

		//m_pGameInstance->Render_Text(L"Font_Dialogue", strStage, _vec2(640.f, 100.f), 1.f, _vec4(1.f, 0.f, 0.f, 1.f));

		if (m_fTime[0] >= 2.f)
		{
			CLauncher::LAUNCHER_TYPE eType = CLauncher::TYPE_RANDOM_POS;
			m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Launcher"), TEXT("Prototype_GameObject_Launcher"), &eType);

			m_fTime[0] = 0.f;
			++m_iCount;
		}

		if (m_iCount >= 10)
		{
			m_fTime[1] += fTimeDelta;

			if (m_fTime[1] >= 25.f)
			{
				m_eCurStage = STAGE_INIT;
				m_eNextStage = STAGE02;
			}
		}

		break;

	case Client::CSurvivalGame::STAGE02:

		m_strStage = L"스테이지 2";

		if (m_fTime[0] >= 1.f)
		{
			CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);

			CProjectile::PROJECTILE_DESC Desc = {};
			Desc.eType = CProjectile::TYPE_FLOOR;
			Desc.vStartPos = pPlayerTransform->Get_State(State::Pos);
			m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Projectile"), TEXT("Prototype_GameObject_Projectile"), &Desc);

			_randFloat Random = _randFloat(-1.f, 1.f);

			for (size_t i = 0; i < 10; i++)
			{
				_vec3 vRandomDir = _vec3(Random(RandomNumber), 0.f, Random(RandomNumber)).Get_Normalized();
				Desc.vStartPos = CENTER_POS + (rand() % 9 + 1) * vRandomDir;
				m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Projectile"), TEXT("Prototype_GameObject_Projectile"), &Desc);
			}

			m_fTime[0] = 0.f;
			++m_iCount;
		}

		if (m_iCount >= 20)
		{
			m_eCurStage = STAGE_INIT;
			m_eNextStage = STAGE03;
		}

		break;

	case Client::CSurvivalGame::STAGE03:

		m_strStage = L"스테이지 3";

		//if (m_fTime[0] >= 1.5f)
		//{
		//	CProjectile::PROJECTILE_DESC Desc = {};
		//	Desc.eType = CProjectile::TYPE_GUIDED_MISSILE;

		//	for (size_t i = 0; i < 8; i++)
		//	{
		//		_randFloat Random = _randFloat(-1.f, 1.f);
		//		_vec3 vRandomDir = _vec3(Random(RandomNumber), 0.f, Random(RandomNumber)).Get_Normalized();

		//		Desc.vStartPos = _vec3(-2000.f, 0.f, 2000.f) + 10 * vRandomDir;
		//		m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Projectile"), TEXT("Prototype_GameObject_Projectile"), &Desc);
		//	}

		//	m_fTime[0] = 0.f;
		//	++m_iCount;
		//}

		//if (m_iCount >= 4)
		//{
		//	m_eCurStage = STAGE_INIT;
		//}

		m_fTime[2] += fTimeDelta;

		if (m_fTime[2] >= 2.5f)
		{
			if (m_fTime[0] >= 1.f)
			{
				CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
				_vec3 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

				// 창 생성
				CProjectile::PROJECTILE_DESC Desc = {};
				Desc.eType = CProjectile::TYPE_SPEAR;
				Desc.pLauncherTransform = m_pTransformCom;
				Desc.vStartPos = vPlayerPos + _vec3(0.f, 4.f, 0.f);

				m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Projectile"), TEXT("Prototype_GameObject_Projectile"), &Desc);

				random_device dev;
				_randNum RandomNumber(dev());
				_randFloat RandomOffsetX = _randFloat(-3.f, 3.f);
				_randFloat RandomOffsetZ = _randFloat(-12.f, 12.f);

				for (size_t i = 0; i < 30; i++)
				{
					Desc.vStartPos = _vec3(CENTER_POS.x + RandomOffsetX(RandomNumber), CENTER_POS.y + 4.f, CENTER_POS.z + RandomOffsetZ(RandomNumber));

					m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Projectile"), TEXT("Prototype_GameObject_Projectile"), &Desc);
				}

				m_pGameInstance->Play_Sound(TEXT("WT_16125_ElectricProtection_SFX_01"));

				m_fTime[0] = 0.f;
				++m_iCount;
			}

		}
		if (m_iCount >= 15)
		{
			m_eCurStage = STAGE_INIT;
			m_eNextStage = STAGE04;
		}

		break;

	case Client::CSurvivalGame::STAGE04:

		//if (m_fTime[0] >= 15.f)
		if (m_pGameInstance->Get_LayerSize(LEVEL_VILLAGE, TEXT("Layer_Cannon")) <= 0)
		{
			m_eCurStage = STAGE_INIT;
			m_eNextStage = STAGE05;
		}

		m_strStage = L"스테이지 4";

		break;

	case Client::CSurvivalGame::STAGE05:

		m_strStage = L"스테이지 5";

		if (m_fTime[0] >= 0.3f)
		{
			CProjectile::PROJECTILE_DESC Desc = {};
			Desc.eType = CProjectile::TYPE_TANGHURU;

			_randFloat Random = _randFloat(-1.f, 1.f);
			_vec3 vRandomDir = _vec3(Random(RandomNumber), 0.f, Random(RandomNumber)).Get_Normalized();

			for (size_t i = 0; i < 6; i++)
			{
				Desc.vStartPos = CENTER_POS + vRandomDir * (i + 4 + i * 0.3f);
				m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Projectile"), TEXT("Prototype_GameObject_Projectile"), &Desc);
			}

			m_pGameInstance->Play_Sound(TEXT("BP_Buff_161900_CurseOfThunder_SFX_01"));

			++m_iCount;
			m_fTime[0] = 0.f;


		}

		if (m_fTime[1] >= 1.f)
		{
			CSurvival_Mon::SURVIVAL_MON_TYPE Type = CSurvival_Mon::TYPE_IMP;
			m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Survival_Mon"), TEXT("Prototype_GameObject_Survival_Mon"), &Type);

			m_fTime[1] = 0.f;
		}

		if (m_iCount >= 50)
		{
			m_eCurStage = STAGE_INIT;
			m_eNextStage = STAGE01;
			m_bGameStart = false;
			m_bGameClear = true;


		}

		break;
	}
}

HRESULT CSurvivalGame::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	return S_OK;
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

	Safe_Release(m_pRendererCom);
}
