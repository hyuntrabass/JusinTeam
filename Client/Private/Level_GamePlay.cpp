#include "Level_GamePlay.h"
#include "Camera.h"
#include "Monster.h"
#include "NPC_Dummy.h"

CLevel_GamePlay::CLevel_GamePlay(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_GAMEPLAY);
	/*
	if (FAILED(Ready_Player()))
	{
		MSG_BOX("Failed to Ready Player");
		return E_FAIL;
	}
	*/

	if (FAILED(Ready_Camera()))
	{
		MSG_BOX("Failed to Ready Camera");
		return E_FAIL;
	}

	if (FAILED(Ready_Light()))
	{
		MSG_BOX("Failed to Ready Light");
		return E_FAIL;
	}

	// Monster
	if (FAILED(Ready_Void05()))
	{
		MSG_BOX("Failed to Ready Void05");
		return E_FAIL;
	}

	// Monster Parse
	//if (FAILED(Ready_Monster()))
	//{
	//	MSG_BOX("Failed to Ready Monster");
	//	return E_FAIL;
	//}

	if (FAILED(Ready_Rabbit()))
	{
		MSG_BOX("Failed to Ready Rabbit");
		return E_FAIL;
	}

	if (FAILED(Ready_Goat()))
	{
		MSG_BOX("Failed to Ready Goat");
		return E_FAIL;
	}

	if (FAILED(Ready_Nastron03()))
	{
		MSG_BOX("Failed to Ready Nastron03");
		return E_FAIL;
	}

	if (FAILED(Ready_NPCvsMon()))
	{
		MSG_BOX("Failed to Ready NPCvsMon");
		return E_FAIL;
	}

	if (FAILED(Ready_Thief04()))
	{
		MSG_BOX("Failed to Ready Thief04");
		return E_FAIL;
	}

	if (FAILED(Ready_TrilobiteA()))
	{
		MSG_BOX("Failed to Ready TrilobiteA");
		return E_FAIL;
	}

	// NPC
	if (FAILED(Ready_Cat()))
	{
		MSG_BOX("Failed to Ready Cat");
		return E_FAIL;
	}

	if (FAILED(Ready_Dog()))
	{
		MSG_BOX("Failed to Ready Dog");
		return E_FAIL;
	}

	if (FAILED(Ready_NPC_Dummy()))
	{
		MSG_BOX("Failed to Ready NPC_Dummy");
		return E_FAIL;
	}

	// Boss
	if (FAILED(Ready_Groar_Boss()))
	{
		MSG_BOX("Failed to Ready Groar_Boss");
		return E_FAIL;
	}

	// Map
	if (FAILED(Ready_Map()))
	{
		MSG_BOX("Failed to Ready Map");
		return E_FAIL;
	}
	

	if (FAILED(Ready_UI()))
	{
		MSG_BOX("Failed to Ready UI");
		return E_FAIL;
	}

	m_pGameInstance->Set_HellHeight(-5000.f);

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
	{
		DestroyWindow(g_hWnd);
	}
}

HRESULT CLevel_GamePlay::Render()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Camera()
{
	if (not m_pGameInstance)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Light()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(-1.f, -2.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);

	return m_pGameInstance->Add_Light(LEVEL_GAMEPLAY, TEXT("Light_Main"), LightDesc);
}

HRESULT CLevel_GamePlay::Ready_Player()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Map()
{
	_uint2 vTerrainSize{ 50, 50 };
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"), &vTerrainSize)))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Void05()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Void05"), TEXT("Prototype_GameObject_Void05"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Cat()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Cat"), TEXT("Prototype_GameObject_Cat"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Dog()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Dog"), TEXT("Prototype_GameObject_Dog"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_NPC_Dummy()
{
	NPC_TYPE eType = ITEM_MERCHANT;

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &eType)))
	{
		return E_FAIL;
	}

	eType = SKILL_MERCHANT;

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &eType)))
	{
		return E_FAIL;
	}

	eType = GROAR;

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &eType)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Groar_Boss()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Groar_Boss"), TEXT("Prototype_GameObject_Groar_Boss"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Monster()
{
	MonsterInfo Info{};
	const TCHAR* pGetPath = L"../Bin/Data/MonsterData.dat";

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MessageBox(g_hWnd, L"파일을 찾지 못했습니다.", L"파일 로드 실패", MB_OK);
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

		Info.strMonsterPrototype = MonsterPrototype;
		Info.MonsterWorldMat = MonsterWorldMat;

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Monster"), MonsterPrototype, &Info)))
		{
			MessageBox(g_hWnd, L"파일 로드 실패", L"파일 로드", MB_OK);
				return E_FAIL;
		}

	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Rabbit()
{
	for (size_t i = 0; i < 1; i++)
	{
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Rabbit"), TEXT("Prototype_GameObject_Rabbit"))))
		{
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Goat()
{
	for (size_t i = 0; i < 1; i++)
	{
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Goat"), TEXT("Prototype_GameObject_Goat"))))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Nastron03()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Nastron03"), TEXT("Prototype_GameObject_Nastron03"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_NPCvsMon()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPCvsMon"), TEXT("Prototype_GameObject_NPCvsMon"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Thief04()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Thief04"), TEXT("Prototype_GameObject_Thief04"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_TrilobiteA()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_TrilobiteA"), TEXT("Prototype_GameObject_TrilobiteA"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_UI()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Player_HP"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Player_MP"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Player_Job"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_SkillBlock"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_ItemBlock"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_ExpBar"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(_dev pDevice, _context pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to create : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}
