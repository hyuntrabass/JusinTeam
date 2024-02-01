#include "Level_Village.h"
#include "Level_Loading.h"
#include "Camera.h"
#include "Monster.h"
#include "NPC.h"
#include "NPC_Dummy.h"
#include "Map.h"
#include "Player.h"


CLevel_Village::CLevel_Village(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Village::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_VILLAGE);
	m_pGameInstance->StopAll();
	m_pGameInstance->PlayBGM(TEXT("BGM_1st_Village"));

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


	// Map
	if (FAILED(Ready_Map()))
	{
		MSG_BOX("Failed to Ready Map");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_WorldMap"), TEXT("Prototype_GameObject_WorldMap"))))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Dungeon()))
	{
		MSG_BOX("Failed to Ready Dungeon");
		return E_FAIL;
	}
	//if (FAILED(Ready_UI()))
	//{
	//	MSG_BOX("Failed to Ready UI");
	//	return E_FAIL;
	//}


	if (FAILED(Ready_Object()))
	{
		MSG_BOX("Failed to Ready Object");
		return E_FAIL;
	}

	if (FAILED(Ready_Environment()))
	{
		MSG_BOX("Failed to Ready Environment");
		return E_FAIL;
	}

	//if (FAILED(Ready_NPC()))
	//{
	//	MSG_BOX("Failed to Ready NPC");
	//	return E_FAIL;
	//}

	if (FAILED(Ready_NPC_Dummy()))
	{
		MSG_BOX("Failed to Ready NPC_Dummy");
		return E_FAIL;
	}

	m_pGameInstance->Set_HellHeight(-5000.f);

	m_pGameInstance->Set_FogNF(_vec2(50.f, 2000.f));
	m_pGameInstance->Set_FogColor(_color(1.f));

	return S_OK;
}

void CLevel_Village::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_END))
	{
		In_To_Dungeon();
		return;
	}

	if (m_pGameInstance->Key_Down(DIK_HOME))
	{
		Ready_Player();
		return;
	}

	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
	{
		DestroyWindow(g_hWnd);
	}
}

HRESULT CLevel_Village::Render()
{
	return S_OK;
}

HRESULT CLevel_Village::Ready_Camera()
{
	if (not m_pGameInstance)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Village::Ready_Light()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(-1.f, -2.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);

	return m_pGameInstance->Add_Light(LEVEL_VILLAGE, TEXT("Light_Main"), LightDesc);
}

HRESULT CLevel_Village::Ready_Player()
{
	// �÷��̾� ��ġ ����
	const TCHAR* pGetPath = TEXT("../Bin/Data/Village_Player_Pos.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MessageBox(g_hWnd, L"../Bin/Data/Player_Pos.dat ������ ã�� ���߽��ϴ�.", L"���� �ε� ����", MB_OK);
		return E_FAIL;
	}

	_vec4 Player_Pos{ 0.f };
	inFile.read(reinterpret_cast<char*>(&Player_Pos), sizeof(_vec4));

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	pPlayerTransform->Set_Position(_vec3(Player_Pos) + _vec3(0.f, 2.f, 0.f));


	return S_OK;
}

HRESULT CLevel_Village::Ready_Map()
{
	const TCHAR* pGetPath = TEXT("../Bin/Data/Village_MapData.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MessageBox(g_hWnd, L"�� ������ ã�� ���߽��ϴ�.", L"���� �ε� ����", MB_OK);
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

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Map"), TEXT("Prototype_GameObject_Village_Map"), &MapInfo)))
		{
			MessageBox(g_hWnd, L"�� �ҷ����� ����", L"���� �ε�", MB_OK);
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CLevel_Village::Ready_Dungeon()
{
	const TCHAR* pGetPath = TEXT("../Bin/Data/Dungeon.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("���� ������ ���� �ҷ����� ����.");
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

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Map"), TEXT("Prototype_GameObject_Dungeon"), &MapInfo)))
		{
			MSG_BOX("���� ���� ����");
			return E_FAIL;
		}
	}

	return S_OK;
}


HRESULT CLevel_Village::Ready_Object()
{

	const TCHAR* pGetPath = TEXT("../Bin/Data/Village_ObjectData.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("������Ʈ ������ ã�� ���߽��ϴ�.");
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
		ObjectInfo.eObjectType = Object_Building;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Village_Object"), TEXT("Prototype_GameObject_Village_Etc_Object"), &ObjectInfo)))
		{
			MSG_BOX("������Ʈ �ҷ����� ����");
			return E_FAIL;
		}
	}
	return S_OK;
}


HRESULT CLevel_Village::Ready_Environment()
{

	const TCHAR* pGetPath = TEXT("../Bin/Data/Village_EnvirData.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("ȯ�������Ʈ ������ ã�� ���߽��ϴ�.");
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
		ObjectInfo.strPrototypeTag = ObjectPrototype ;
		ObjectInfo.m_WorldMatrix = ObjectWorldMat;
		ObjectInfo.eObjectType = Object_Environment;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Envir_Object"), TEXT("Prototype_GameObject_Village_Envir_Object"), &ObjectInfo)))
		{
			MSG_BOX("ȯ�������Ʈ �ҷ����� ����");
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CLevel_Village::In_To_Dungeon()
{
	// �÷��̾� ��ġ ����
	const TCHAR* pGetPath = TEXT("../Bin/Data/DungeonPos.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MessageBox(g_hWnd, L"../Bin/Data/DungeonPos.dat ������ ã�� ���߽��ϴ�.", L"���� �ε� ����", MB_OK);
		return E_FAIL;
	}

	_vec4 Player_Pos{ 0.f };
	inFile.read(reinterpret_cast<char*>(&Player_Pos), sizeof(_vec4));

	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	pPlayerTransform->Set_Position(_vec3(Player_Pos) + _vec3(0.f, 2.f, 0.f));


	return S_OK;

	return E_NOTIMPL;
}


HRESULT CLevel_Village::Ready_NpcvsMon()
{
	MonsterInfo Info{};
	const TCHAR* pGetPath = L"../Bin/Data/Prologue_MonsterData.dat";

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MessageBox(g_hWnd, L"../Bin/Data/Prologue_MonsterData.dat ������ ã�� ���߽��ϴ�.", L"���� �ε� ����", MB_OK);
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

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Monster"), MonsterPrototype, &Info)))
		{
			MessageBox(g_hWnd, L"���� �ε� ����", L"���� �ε�", MB_OK);
			return E_FAIL;
		}

	}
	return S_OK;
}

HRESULT CLevel_Village::Ready_NPC()
{
	NPC_INFO Info{};
	const TCHAR* pGetPath = L"../Bin/Data/Village_NPCData.dat";

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("../Bin/Data/Village_NPCData.dat ���� �ҷ����� ����.");
		return E_FAIL;
	}

	_uint NPCListSize;
	inFile.read(reinterpret_cast<char*>(&NPCListSize), sizeof(_uint));

	for (_uint i = 0; i < NPCListSize; ++i)
	{
		_ulong NPCPrototypeSize;
		inFile.read(reinterpret_cast<char*>(&NPCPrototypeSize), sizeof(_ulong));

		wstring NPCPrototype;
		NPCPrototype.resize(NPCPrototypeSize);
		inFile.read(reinterpret_cast<char*>(&NPCPrototype[0]), NPCPrototypeSize * sizeof(wchar_t));

		_mat NPCWorldMat;
		inFile.read(reinterpret_cast<char*>(&NPCWorldMat), sizeof(_mat));

		Info.strNPCPrototype = NPCPrototype;
		Info.NPCWorldMat = NPCWorldMat;

		if (Info.strNPCPrototype == TEXT("Prototype_Model_BlackSmith"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_BlackSmith"), TEXT("Prototype_GameObject_BlackSmith"), &Info)))
			{
				MSG_BOX("BlackSmith ���� ����");
				return E_FAIL;
			}

		}
		else if (Info.strNPCPrototype == TEXT("Prototype_Model_ItemMerchant"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_ItemMerchant"), TEXT("Prototype_GameObject_ItemMerchant"), &Info)))
			{
				MSG_BOX("ItemMerchant ���� ����");
				return E_FAIL;
			}

		}
		else if (Info.strNPCPrototype == TEXT("Prototype_Model_Roskva"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Roskva"), TEXT("Prototype_GameObject_Roskva"), &Info)))
			{
				MSG_BOX("Roskva ���� ����");
				return E_FAIL;
			}

		}
		else if (Info.strNPCPrototype == TEXT("Prototype_Model_Cat"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Cat"), TEXT("Prototype_GameObject_Cat"), &Info)))
			{
				MSG_BOX("Cat ���� ����");
				return E_FAIL;
			}

		}
		else if (Info.strNPCPrototype == TEXT("Prototype_Model_Dog"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Dog"), TEXT("Prototype_GameObject_Dog"), &Info)))
			{
				MSG_BOX("Dog ���� ����");
				return E_FAIL;
			}

		}

	}

	return S_OK;
}

HRESULT CLevel_Village::Ready_NPC_Dummy()
{
	NPC_INFO Info{};
	const TCHAR* pGetPath = L"../Bin/Data/Village_NPCDummyData.dat";

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("../Bin/Data/Village_NPCDummyData.dat �ҷ����� ����.");
		return E_FAIL;
	}

	_uint NPCListSize;
	inFile.read(reinterpret_cast<char*>(&NPCListSize), sizeof(_uint));

	for (_uint i = 0; i < NPCListSize; ++i)
	{
		_ulong NPCPrototypeSize;
		inFile.read(reinterpret_cast<char*>(&NPCPrototypeSize), sizeof(_ulong));

		wstring NPCPrototype;
		NPCPrototype.resize(NPCPrototypeSize);
		inFile.read(reinterpret_cast<char*>(&NPCPrototype[0]), NPCPrototypeSize * sizeof(wchar_t));

		_mat NPCWorldMat;
		inFile.read(reinterpret_cast<char*>(&NPCWorldMat), sizeof(_mat));

		Info.strNPCPrototype = NPCPrototype;
		Info.NPCWorldMat = NPCWorldMat;

		if (Info.strNPCPrototype == TEXT("Prototype_Model_BlackSmith"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_BlackSmith"), TEXT("Prototype_GameObject_BlackSmith"), &Info)))
			{
				MSG_BOX("BlackSmith ���� ����");
				return E_FAIL;
			}

		}
		else if (Info.strNPCPrototype == TEXT("Prototype_Model_ItemMerchant"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_ItemMerchant"), TEXT("Prototype_GameObject_ItemMerchant"), &Info)))
			{
				MSG_BOX("ItemMerchant ���� ����");
				return E_FAIL;
			}

		}
		else if (Info.strNPCPrototype == TEXT("Prototype_Model_Roskva"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Roskva"), TEXT("Prototype_GameObject_Roskva"), &Info)))
			{
				MSG_BOX("Roskva ���� ����");
				return E_FAIL;
			}

		}
		else if (Info.strNPCPrototype == TEXT("Prototype_Model_Cat"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Cat"), TEXT("Prototype_GameObject_Cat"), &Info)))
			{
				MSG_BOX("Cat ���� ����");
				return E_FAIL;
			}

		}
		else if (Info.strNPCPrototype == TEXT("Prototype_Model_Dog"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Dog"), TEXT("Prototype_GameObject_Dog"), &Info)))
			{
				MSG_BOX("Dog ���� ����");
				return E_FAIL;
			}

		}

		string strInputFilePath = "../../Client/Bin/Resources/AnimMesh/NPC/NPC_Dummy/Mesh/";
		for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
		{
			if (entry.is_regular_file())
			{
				if (!entry.exists())
				{
					return S_OK;
				}

				wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

				if (Info.strNPCPrototype == strPrototypeTag)
				{
					if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_") + entry.path().stem().wstring(), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
					{
						wstring MSG = entry.path().stem().wstring() + L"���� ����";
						MessageBox(nullptr, MSG.c_str(), L"error", MB_OK);
						return E_FAIL;
					}
				}
			}
		}

		//if (Info.strNPCPrototype == TEXT("Prototype_Model_Dwarf_Male_002"))
		//{
		//	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Dwarf_Male_002"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
		//	{
		//		MSG_BOX("Dwarf_Male_002 ���� ����");
		//		return E_FAIL;
		//	}
		//}

		//else if (Info.strNPCPrototype == TEXT("Prototype_Model_Female_003"))
		//{
		//	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Female_003"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
		//	{
		//		MSG_BOX("Female_003 ���� ����");
		//		return E_FAIL;
		//	}
		//}

		//else if (Info.strNPCPrototype == TEXT("Prototype_Model_Female_004"))
		//{
		//	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Female_004"), TEXT("Prototype_GameObject_NPC_Dummy"), &Info)))
		//	{
		//		MSG_BOX("Female_004 ���� ����");
		//		return E_FAIL;
		//	}
		//}
	}

	return S_OK;
}

HRESULT CLevel_Village::Ready_UI()
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
	/*'
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_Pop_QuestIn"))))
	{
		return E_FAIL;
	}
	*/

	return S_OK;
}

CLevel_Village* CLevel_Village::Create(_dev pDevice, _context pContext)
{
	CLevel_Village* pInstance = new CLevel_Village(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to create : CLevel_Village");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Village::Free()
{
	__super::Free();
}
