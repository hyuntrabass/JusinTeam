#include "Level_Tower.h"
#include "Level_Loading.h"
#include "Camera.h"
#include "Camera_CutScene.h"
#include "Monster.h"
#include "NPC.h"
#include "NPC_Dummy.h"
#include "Guard.h"
#include "GuardTower.h"
#include "Map.h"
#include "Trigger_Manager.h"
#include "VTFMonster.h"
#include "Effect_Manager.h"

CLevel_Tower::CLevel_Tower(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Tower::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_TOWER);
	m_pGameInstance->StopAll();


	//CTrigger_Manager::Get_Instance()->Teleport(TS_Minigame);

	if (FAILED(Ready_Light()))
	{
		MSG_BOX("Failed to Ready Light");
		return E_FAIL;
	}

	// Map
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_WorldMap"), TEXT("Prototype_GameObject_WorldMap"))))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_DragonMap_Effect()))
	{
		MSG_BOX("Failed to Ready DragonMap Effect");
		return E_FAIL;
	}

	if (FAILED(Ready_Human_Boss()))
	{
		MSG_BOX("Failed to Ready HumanBoss");
		return E_FAIL;
	}

	if (FAILED(Ready_Trigger()))
	{
		MSG_BOX("Failed to Ready Trigger");
		return E_FAIL;
	}

	if (FAILED(Ready_Survival_Game()))
	{
		MSG_BOX("Failed to Ready Survival Game");
		return E_FAIL;
	}

	
	CUI_Manager::Get_Instance()->Set_FullScreenUI(true);
	CUI_Manager::Get_Instance()->Open_InfinityTower(true);
	m_pGameInstance->Set_FogNF(_vec2(50.f, 2000.f));
	m_pGameInstance->Set_FogColor(_color(1.f));

	//m_pGameInstance->PlayBGM(TEXT("BGM_1st_Village"));

	return S_OK;
}

void CLevel_Tower::Tick(_float fTimeDelta)
{
	//m_pGameInstance->PhysXTick(fTimeDelta);
	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
	{
		DestroyWindow(g_hWnd);
	}

	if (m_pGameInstance->Key_Down(DIK_NUMPAD1))
	{
		CTrigger_Manager::Get_Instance()->Teleport(TS_Minigame);
		return;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD4))
	{
		CTrigger_Manager::Get_Instance()->Teleport(TS_DragonMap);
		return;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD5))
	{
		CTrigger_Manager::Get_Instance()->Teleport(TS_BossRoom);
		return;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD6))
	{
		CTrigger_Manager::Get_Instance()->Teleport(TS_MiniDungeon);
		return;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD7))
	{
		CTrigger_Manager::Get_Instance()->Teleport(TS_SurvivalMap);
		return;
	}
	if (m_pGameInstance->Key_Down(DIK_0))
	{
		CTrigger_Manager::Get_Instance()->Teleport(TS_SescoMap);

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_CescoGame"), TEXT("Prototype_GameObject_CescoGame"))))
			return;

		return;
	}
	// Test
	if (m_pGameInstance->Key_Down(DIK_RSHIFT))
	{
		m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_Dragon_Boss"), TEXT("Prototype_GameObject_Dragon_Boss"));
	}
	//if (m_pGameInstance->Key_Down(DIK_UP))
	//{
	//	m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_Statue"), TEXT("Prototype_GameObject_Statue"));
	//}
	if (m_pGameInstance->Key_Down(DIK_EQUALS))
	{
		static _mat FountainMat = _mat::CreateScale(1.3f) * _mat::CreateRotationZ(XMConvertToRadians(90.f)) * _mat::CreateTranslation(_vec3(-25.292f, 17.321f, 116.395f));
		EffectInfo EffectDesc = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Waterfall_Dist");
		EffectDesc.pMatrix = &FountainMat;
		EffectDesc.isFollow = true;
		CEffect_Manager::Get_Instance()->Add_Layer_Effect(EffectDesc);
	}

	if (m_pGameInstance->Key_Down(DIK_NUMPADMINUS))
	{
		CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
		GuardInfo Desc{};
		Desc.mMatrix = pPlayerTransform->Get_World_Matrix();
		//_vec3 vPos = pPlayerTransform->Get_State(State::Pos);
		//vPos.y += 1.5f;s
		Desc.iIndex++;

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_Guard"), TEXT("Prototype_GameObject_Guard"), &Desc)))
		{
			return;
		}
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPADPLUS))
	{
		CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
		GuardTowerInfo Desc{};
		Desc.mMatrix = pPlayerTransform->Get_World_Matrix();
		Desc.iIndex++;

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_Guard"), TEXT("Prototype_GameObject_GuardTower"), &Desc)))
		{
			return;
		}
	}


}

HRESULT CLevel_Tower::Render()
{
	return S_OK;
}

HRESULT CLevel_Tower::Ready_Light()
{
	LIGHT_DESC* Light = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, L"Light_Main");
	*Light = g_Light_Village;

	return S_OK;
}

HRESULT CLevel_Tower::Ready_Guard()
{
	GuardInfo Info{};
	const TCHAR* pGetPath = L"../Bin/Data/MiniDungeon_NPCData.dat";

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MessageBox(g_hWnd, L"MiniDungeon_NPCData 파일을 찾지 못했습니다.", L"파일 로드 실패", MB_OK);
		return E_FAIL;
	}

	_uint MonsterListSize;
	inFile.read(reinterpret_cast<char*>(&MonsterListSize), sizeof(_uint));


	for (_uint i = 0; i < MonsterListSize; ++i)
	{
		_ulong MonsterPrototypeSize;
		inFile.read(reinterpret_cast<char*>(&MonsterPrototypeSize), sizeof(_ulong));

		wstring MonsterPrototype;
		MonsterPrototype.resize(MonsterPrototypeSize);
		inFile.read(reinterpret_cast<char*>(&MonsterPrototype[0]), MonsterPrototypeSize * sizeof(wchar_t));

		_mat MonsterWorldMat;
		inFile.read(reinterpret_cast<char*>(&MonsterWorldMat), sizeof(_mat));

		Info.mMatrix = MonsterWorldMat;
		Info.iIndex = i;

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Guard"), &Info)))
		{
			MessageBox(g_hWnd, L"파일 로드 실패", L"파일 로드", MB_OK);
			return E_FAIL;
		}

	}
	inFile.close();
	return S_OK;
}

HRESULT CLevel_Tower::Ready_Human_Boss()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Boss"), TEXT("Prototype_GameObject_Human_Boss"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Tower::Ready_SescoGame()
{
	/*for (_uint i = 0; i < 100; i++)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_Model_VTFMonster_Void19");

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_SescoGameObject"), TEXT("Prototype_GameObject_Void19_Object"), &VTFMonsterDesc)))
			return E_FAIL;
	}
	for (_uint i = 0; i < 100; i++)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_Model_VTFMonster_Muspelheim06");

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_SescoGameObject"), TEXT("Prototype_GameObject_Void19_Object"), &VTFMonsterDesc)))
			return E_FAIL;
	}
	for (_uint i = 0; i < 100; i++)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_Model_VTFMonster_Niflheim09");

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_SescoGameObject"), TEXT("Prototype_GameObject_Void19_Object"), &VTFMonsterDesc)))
			return E_FAIL;
	}
	for (_uint i = 0; i < 100; i++)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_Model_VTFMonster_Redant");

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_SescoGameObject"), TEXT("Prototype_GameObject_Void19_Object"), &VTFMonsterDesc)))
			return E_FAIL;
	}
	for (_uint i = 0; i < 100; i++)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_Model_VTFMonster_Scorpion");

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_SescoGameObject"), TEXT("Prototype_GameObject_Void19_Object"), &VTFMonsterDesc)))
			return E_FAIL;
	}
	for (_uint i = 0; i < 100; i++)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_Model_VTFMonster_Trilobites");

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_SescoGameObject"), TEXT("Prototype_GameObject_Void19_Object"), &VTFMonsterDesc)))
			return E_FAIL;
	}
	for (_uint i = 0; i < 100; i++)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_Model_VTFMonster_Trilobites_A");

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_SescoGameObject"), TEXT("Prototype_GameObject_Void19_Object"), &VTFMonsterDesc)))
			return E_FAIL;
	}*/
	return S_OK;
}

HRESULT CLevel_Tower::Ready_Survival_Game()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_SurvivalGame"), TEXT("Prototype_GameObject_SurvivalGame"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Tower::Ready_DragonMap_Effect()
{
	_mat SparkMat = _mat::CreateTranslation(_vec3(3000.f, -1.5f, 3000.f));
	EffectInfo EffectDesc = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"MapSpark");
	EffectDesc.pMatrix = &SparkMat;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(EffectDesc);
	SparkMat = _mat::CreateTranslation(_vec3(3000.f, 3.f, 3000.f));
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(EffectDesc);
	SparkMat = _mat::CreateTranslation(_vec3(3000.f, 1.5f, 3000.f));
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(EffectDesc);


	std::ifstream InFile(L"../Bin/Data/DragonMap_Effect.dat", std::ios::binary);

	if (InFile.is_open())
	{
		_uint EffectListSize;
		InFile.read(reinterpret_cast<char*>(&EffectListSize), sizeof(_uint));

		for (_uint i = 0; i < EffectListSize; ++i)
		{
			_ulong EffectNameSize;
			InFile.read(reinterpret_cast<char*>(&EffectNameSize), sizeof(_ulong));

			wstring EffectName;
			EffectName.resize(EffectNameSize);
			InFile.read(reinterpret_cast<char*>(&EffectName[0]), EffectNameSize * sizeof(wchar_t));

			_bool isFollow{ false };
			InFile.read(reinterpret_cast<char*>(&isFollow), sizeof(_bool));

			_float EffectSize{};
			InFile.read(reinterpret_cast<char*>(&EffectSize), sizeof(_float));

			_mat EffectWorldMat;
			InFile.read(reinterpret_cast<char*>(&EffectWorldMat), sizeof(_mat));

			EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(EffectName);
			Info.pMatrix = &EffectWorldMat;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
		}

		InFile.close();
	}


	return S_OK;
}

HRESULT CLevel_Tower::Ready_Trigger()
{
	return CTrigger_Manager::Get_Instance()->Ready_Trigger_Village();
}

CLevel_Tower* CLevel_Tower::Create(_dev pDevice, _context pContext)
{
	CLevel_Tower* pInstance = new CLevel_Tower(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to create : CLevel_Tower");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Tower::Free()
{
	__super::Free();
}
