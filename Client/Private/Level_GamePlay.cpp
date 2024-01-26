#include "Level_GamePlay.h"
#include "Level_Loading.h"
#include "Camera.h"
#include "Monster.h"
#include "NPC.h"
#include "Map.h"
#include "Player.h"
#include "Effect_Manager.h"
#include "UI_Manager.h"


CLevel_GamePlay::CLevel_GamePlay(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_GAMEPLAY);
	m_pGameInstance->StopAll();
	m_pGameInstance->PlayBGM(TEXT("Prologue_BGM_Loop"), 0.1f);
	m_pGameInstance->Play_Sound(TEXT("AMB_Voidness_Rain_Area_SFX_01"), 0.3f, true);

	CUI_Manager::Get_Instance()->Init();


	if (FAILED(Ready_Player()))
	{
		MSG_BOX("Failed to Ready Player");
		return E_FAIL;
	}


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
	if (FAILED(Ready_ModelTest()))
	{
		MSG_BOX("Failed to Ready ModelTest");
		return E_FAIL;
	}



	
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

	if (FAILED(Ready_Void01()))
	{
		MSG_BOX("Failed to Ready Void01");
		return E_FAIL;
	}

	if (FAILED(Ready_Void05()))
	{
		MSG_BOX("Failed to Ready Void05");
		return E_FAIL;
	}

	if (FAILED(Ready_Imp()))
	{
		MSG_BOX("Failed to Ready Imp");
		return E_FAIL;
	}

	if (FAILED(Ready_Void09()))
	{
		MSG_BOX("Failed to Ready Void09");
		return E_FAIL;
	}

	if (FAILED(Ready_Void20()))
	{
		MSG_BOX("Failed to Ready Void20");
		return E_FAIL;
	}

	if (FAILED(Ready_Void23()))
	{
		MSG_BOX("Failed to Ready Void23");
		return E_FAIL;
	}

	if (FAILED(Ready_Nastron07()))
	{
		MSG_BOX("Failed to Ready Nastron07");
		return E_FAIL;
	}

	// NPC
	//if (FAILED(Ready_Cat()))
	//{
	//	MSG_BOX("Failed to Ready Cat");
	//	return E_FAIL;
	//}

	//if (FAILED(Ready_Dog()))
	//{
	//	MSG_BOX("Failed to Ready Dog");
	//	return E_FAIL;
	//}

	//if (FAILED(Ready_NPC_Test()))
	//{
	//	MSG_BOX("Failed to Ready NPC");
	//	return E_FAIL;
	//}

	//if (FAILED(Ready_NPC_Dummy_Test()))
	//{
	//	MSG_BOX("Failed to Ready NPC_Dummy");
	//	return E_FAIL;
	//}

	// Boss
	if (FAILED(Ready_Groar_Boss()))
	{
		MSG_BOX("Failed to Ready Groar_Boss");
		return E_FAIL;
	}

	// Pet
	if (FAILED(Ready_Pet()))
	{
		MSG_BOX("Failed to Ready Pet");
		return E_FAIL;
	}

	// Map
	if (FAILED(Ready_Map()))
	{
		MSG_BOX("Failed to Ready Map");
		return E_FAIL;
	}
		// Environment
	if (FAILED(Ready_Environment()))
	{
		MSG_BOX("Failed to Ready Environment");
		return E_FAIL;
	}


	if (FAILED(Ready_UI()))
	{
		MSG_BOX("Failed to Ready UI");
		return E_FAIL;
	}


	if (FAILED(Ready_Object()))
	{
		MSG_BOX("Failed to Ready Object");
		return E_FAIL;
	}

	m_pGameInstance->Set_HellHeight(-5000.f);

	EffectInfo EffectDesc = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Rain");
	EffectDesc.pMatrix = &m_RainMatrix;
	EffectDesc.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(&EffectDesc);



	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	if (!CUI_Manager::Get_Instance()->Is_InvenActive())
	{
		m_RainMatrix = _mat::CreateTranslation(_vec3(m_pGameInstance->Get_CameraPos()));
	}
	//m_RainMatrix = _mat::CreateTranslation(_vec3(50.f, 3.f, 50.f));

	if (m_pGameInstance->Key_Down(DIK_NUMPAD9) or m_pGameInstance->Key_Down(DIK_PRIOR))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_VILLAGE))))
		{
			return;
		}

		return;
	}

	//m_pGameInstance->PhysXTick(fTimeDelta);

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
	LightDesc.vDirection = _float4(-1.f, -2.f,-1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);

	return m_pGameInstance->Add_Light(LEVEL_GAMEPLAY, TEXT("Light_Main"), LightDesc);
}

HRESULT CLevel_GamePlay::Ready_Player()
{
	// 플레이어 위치 세팅
	const TCHAR* pGetPath = TEXT("../Bin/Data/Prologue_Player_Pos.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MessageBox(g_hWnd, L"../Bin/Data/Prologue_Player_Pos.dat 파일이 없습니다..", L"불러오기 실패", MB_OK);
		return E_FAIL;
	}

	_vec4 Player_Pos{ 0.f };
	inFile.read(reinterpret_cast<char*>(&Player_Pos), sizeof(_vec4));

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	pPlayerTransform->Set_Position(_vec3(Player_Pos) + _vec3(0.f, 2.f, 0.f));


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Map()
{
	TERRAIN_INFO Terrain;
	Terrain.vTerrainSize = _uint2(300, 300);
	Terrain.isMesh = false;

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"), &Terrain)))
	{
		return E_FAIL;
	}

	const TCHAR* pGetPath = TEXT("../Bin/Data/Prologue_MapData.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("맵 데이터 파일 불러오기 실패.");
		return E_FAIL;
	}

	_uint MapListSize;
	inFile.read(reinterpret_cast<char*>(&MapListSize), sizeof(_uint));


	for (_uint i = 0; i < MapListSize; ++i)
	{
		_ulong MapPrototypeSize;
		inFile.read(reinterpret_cast<char*>(&MapPrototypeSize), sizeof(_ulong));

		wstring MapPrototype;
		MapPrototype.resize(MapPrototypeSize);
		inFile.read(reinterpret_cast<char*>(&MapPrototype[0]), MapPrototypeSize * sizeof(wchar_t));

		_mat MapWorldMat;
		inFile.read(reinterpret_cast<char*>(&MapWorldMat), sizeof(_mat));

		MapInfo MapInfo{};
		MapInfo.Prototype = MapPrototype;
		MapInfo.m_Matrix = MapWorldMat;

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Map"), TEXT("Prototype_GameObject_Map"), &MapInfo)))
		{
			MSG_BOX("맵 생성 실패");
			return E_FAIL;
		}
	}

	inFile.close();



	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Environment()
{

	const TCHAR* pGetPath = TEXT("../Bin/Data/Prologue_EnvirData.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("환경 데이터 파일 불러오기 실패.");
		return E_FAIL;
	}

	_uint EnvirListSize;
	inFile.read(reinterpret_cast<char*>(&EnvirListSize), sizeof(_uint));


	for (_uint i = 0; i < EnvirListSize; ++i)
	{
		_ulong EnvirPrototypeSize;
		inFile.read(reinterpret_cast<char*>(&EnvirPrototypeSize), sizeof(_ulong));

		wstring EnvirPrototype;
		EnvirPrototype.resize(EnvirPrototypeSize);
		inFile.read(reinterpret_cast<char*>(&EnvirPrototype[0]), EnvirPrototypeSize * sizeof(wchar_t));

		_mat EnvirWorldMat;
		inFile.read(reinterpret_cast<char*>(&EnvirWorldMat), sizeof(_mat));

		ObjectInfo EnvirInfo{};
		EnvirInfo.strPrototypeTag = EnvirPrototype;
		EnvirInfo.m_WorldMatrix = EnvirWorldMat;
		EnvirInfo.eObjectType = Object_Environment;

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Prologue_Envir"), TEXT("Prototype_GameObject_Prologue_Object"), &EnvirInfo)))
		{
			MSG_BOX("맵 생성 실패");
			return E_FAIL;
		}
	}
	return S_OK;

}


HRESULT CLevel_GamePlay::Ready_Object()
{

	const TCHAR* pGetPath = TEXT("../Bin/Data/Prologue_ObjectData.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("오브젝트 불러오기 실패.");
		return E_FAIL;
	}

	_uint ObjectListSize;
	inFile.read(reinterpret_cast<char*>(&ObjectListSize), sizeof(_uint));


	for (_uint i = 0; i < ObjectListSize; ++i)
	{
		_ulong ObjectPrototypeSize;
		inFile.read(reinterpret_cast<char*>(&ObjectPrototypeSize), sizeof(_ulong));

		wstring ObjectPrototype;
		ObjectPrototype.resize(ObjectPrototypeSize);
		inFile.read(reinterpret_cast<char*>(&ObjectPrototype[0]), ObjectPrototypeSize * sizeof(wchar_t));

		_mat ObjectWorldMat;
		inFile.read(reinterpret_cast<char*>(&ObjectWorldMat), sizeof(_mat));

		ObjectInfo ObjectInfo{};
		ObjectInfo.strPrototypeTag = ObjectPrototype;
		ObjectInfo.m_WorldMatrix = ObjectWorldMat;
		ObjectInfo.eObjectType = Object_Etc;

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Prologue_Object"), TEXT("Prototype_GameObject_Prologue_Object"), &ObjectInfo)))
		{
			MSG_BOX("오브젝트 생성 실패");
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_ModelTest()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_ModelTest"), TEXT("Prototype_GameObject_ModelTest"))))
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

HRESULT CLevel_GamePlay::Ready_NPC_Test()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_ItemMerchant"), TEXT("Prototype_GameObject_ItemMerchant"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_BlackSmith"), TEXT("Prototype_GameObject_BlackSmith"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Roskva"), TEXT("Prototype_GameObject_Roskva"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_NPC_Dummy_Test()
{
	NPC_INFO Info = {};
	Info.strNPCPrototype = TEXT("Prototype_Model_Dwarf_Male_002");

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	{
		return E_FAIL;
	}

	Info.strNPCPrototype = TEXT("Prototype_Model_Female_003");

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	{
		return E_FAIL;
	}

	Info.strNPCPrototype = TEXT("Prototype_Model_Female_004");

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	{
		return E_FAIL;
	}

	Info.strNPCPrototype = TEXT("Prototype_Model_Female_006");

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	{
		return E_FAIL;
	}

	Info.strNPCPrototype = TEXT("Prototype_Model_Male_013");

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	{
		return E_FAIL;
	}

	Info.strNPCPrototype = TEXT("Prototype_Model_Male_015");

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	{
		return E_FAIL;
	}

	Info.strNPCPrototype = TEXT("Prototype_Model_Male_018");

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	{
		return E_FAIL;
	}

	Info.strNPCPrototype = TEXT("Prototype_Model_Female_013");

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	{
		return E_FAIL;
	}

	Info.strNPCPrototype = TEXT("Prototype_Model_Female_027");

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	{
		return E_FAIL;
	}

	Info.strNPCPrototype = TEXT("Prototype_Model_Male_Chi");

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	{
		return E_FAIL;
	}

	Info.strNPCPrototype = TEXT("Prototype_Model_Male_016");

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	{
		return E_FAIL;
	}

	Info.strNPCPrototype = TEXT("Prototype_Model_Male_020");

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
	{
		return E_FAIL;
	}

	Info.strNPCPrototype = TEXT("Prototype_Model_Male_027");

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPC_Dummy"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
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

HRESULT CLevel_GamePlay::Ready_Pet()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Pet_Cat"), TEXT("Prototype_GameObject_Pet_Cat"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Pet_Dragon"), TEXT("Prototype_GameObject_Pet_Dragon"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_NpcvsMon()
{
	MonsterInfo Info{};
	const TCHAR* pGetPath = L"../Bin/Data/Prologue_MonsterData.dat";

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("../Bin/Data/Prologue_MonsterData.dat 몬스터 불러오기 실패.");
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

		if (Info.strMonsterPrototype == TEXT("Prototype_Model_NPCvsMon"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_NPCvsMon"), &Info)))
			{
				MSG_BOX("몬스터 생성 실패");
				return E_FAIL;
			}

		}
		else if (Info.strMonsterPrototype == TEXT("Prototype_Model_Void05"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Void05"), &Info)))
			{
				MSG_BOX("몬스터 생성 실패");
				return E_FAIL;
			}

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

HRESULT CLevel_GamePlay::Ready_Void01()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Void01"), TEXT("Prototype_GameObject_Void01"))))
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

HRESULT CLevel_GamePlay::Ready_Imp()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Imp"), TEXT("Prototype_GameObject_Imp"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Void09()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Void09"), TEXT("Prototype_GameObject_Void09"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Void20()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Void20"), TEXT("Prototype_GameObject_Void20"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Void23()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Void23"), TEXT("Prototype_GameObject_Void23"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Nastron07()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Nastron07"), TEXT("Prototype_GameObject_Nastron07"))))
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

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Inven"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Coin"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Diamond"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Menu"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Radar"))))
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
