#include "Level_Village.h"
#include "Level_Loading.h"
#include "Camera.h"
#include "Camera_CutScene.h"
#include "Monster.h"
#include "NPC.h"
#include "NPC_Dummy.h"
#include "Map.h"
#include "Trigger_Manager.h"
#include "VTFMonster.h"

CLevel_Village::CLevel_Village(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Village::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_VILLAGE);
	m_pGameInstance->StopAll();


	//if (FAILED(Ready_Player()))
	//{
	//	MSG_BOX("Failed to Ready Player");
	//	return E_FAIL;
	//}
	CTrigger_Manager::Get_Instance()->Teleport(TS_Village);

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

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_WorldMap"), TEXT("Prototype_GameObject_WorldMap"))))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Torch()))
	{
		MSG_BOX("Failed to Ready Torch");
		return E_FAIL;
	}

	if (FAILED(Ready_Interaction()))
	{
		MSG_BOX("Failed to Ready Interaction");
		return E_FAIL;
	}

	if (FAILED(Ready_Village_Monster()))
	{
		MSG_BOX("Failed to Ready Village Monster");
		return E_FAIL;
	}

	if (FAILED(Ready_Dungeon_Monster()))
	{
		MSG_BOX("Failed to Ready Dungeon Monster");
		return E_FAIL;
	}

	if (FAILED(Ready_Groar_Boss()))
	{
		MSG_BOX("Failed to Ready Groar");
		return E_FAIL;
	}

	if (FAILED(Ready_Human_Boss()))
	{
		MSG_BOX("Failed to Ready HumanBoss");
		return E_FAIL;
	}

	if (FAILED(Ready_NPC()))
	{
		MSG_BOX("Failed to Ready NPC");
		return E_FAIL;
	}

	if (FAILED(Ready_NPC_Dummy()))
	{
		MSG_BOX("Failed to Ready NPC_Dummy");
		return E_FAIL;
	}
	if (FAILED(Ready_Trigger()))
	{
		MSG_BOX("Failed to Ready Trigger");
		return E_FAIL;
	}

	if (FAILED(Ready_Test()))
	{
		MSG_BOX("Failed to Ready Test");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_BrickBar"), TEXT("Prototype_GameObject_BrickBar"))))
	{
		return E_FAIL;
	}


	if (FAILED(Ready_SescoGame()))
	{
		MSG_BOX("Failed to Ready SescoGame");
		return E_FAIL;
	}
	
	m_pGameInstance->Set_FogNF(_vec2(50.f, 2000.f));
	m_pGameInstance->Set_FogColor(_color(1.f));

	m_pGameInstance->PlayBGM(TEXT("BGM_1st_Village"));
	return S_OK;
}

void CLevel_Village::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_B))
	{

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_BrickBall"), TEXT("Prototype_GameObject_BrickBall"))))
		{
			return;
		}

	}
	if (m_pGameInstance->Key_Down(DIK_END))
	{
		CTrigger_Manager::Get_Instance()->Teleport(TS_Dungeon);
		return;
	}

	//m_pGameInstance->PhysXTick(fTimeDelta);
	if (m_pGameInstance->Key_Down(DIK_HOME))
	{
		CTrigger_Manager::Get_Instance()->Teleport(TS_Village);
		return;
	}

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

		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_Model_VTFMonster_Void19");

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_SescoGameObject"), TEXT("Prototype_GameObject_Void19_Object"), &VTFMonsterDesc)))
			return;

		return;
	}
	// Test
	if (m_pGameInstance->Key_Down(DIK_RSHIFT))
	{
		m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Dragon_Boss"), TEXT("Prototype_GameObject_Dragon_Boss"));
	}
	if (m_pGameInstance->Key_Down(DIK_UP))
	{
		m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Statue"), TEXT("Prototype_GameObject_Statue"));
	}
	if (m_pGameInstance->Key_Down(DIK_EQUALS))
	{
		m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Test"), TEXT("Prototype_GameObject_Void20"));
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
	LIGHT_DESC* Light = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, L"Light_Main");
	*Light = g_Light_Village;

	return S_OK;
}

HRESULT CLevel_Village::Ready_Torch()
{
	const TCHAR* pGetPath = TEXT("../Bin/Data/Dungeon_Torch.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("Torch ������ ã�� ���߽��ϴ�.");
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
		ObjectInfo.m_iIndex = (_uint)DUNGEON;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Village_Object"), TEXT("Prototype_GameObject_Torch_Object"), &ObjectInfo)))
		{
			MSG_BOX("Torch �ҷ����� ����");
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CLevel_Village::Ready_Interaction()
{
	const TCHAR* pGetPath = TEXT("../Bin/Data/Village_InteractionData.dat");

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
		ObjectInfo.strPrototypeTag = ObjectPrototype;
		ObjectInfo.m_WorldMatrix = ObjectWorldMat;
		ObjectInfo.eObjectType = Object_Environment;
		ObjectInfo.m_iIndex = (_uint)FIELD;

		if (ObjectPrototype == L"Prototype_Model_OakTree" || ObjectPrototype == L"Prototype_Model_Herbs")
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Interaction_Object"), TEXT("Prototype_GameObject_Intraction_NonAnim_Object"), &ObjectInfo)))
			{
				MSG_BOX("ä��������Ʈ �ҷ����� ����");
				return E_FAIL;
			}
		}
		else if (ObjectPrototype == L"Prototype_Model_GoldStone" || ObjectPrototype == L"Prototype_Model_SaltStone")
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Interaction_Object"), TEXT("Prototype_GameObject_Intraction_Anim_Object"), &ObjectInfo)))
			{
				MSG_BOX("ä��������Ʈ �ҷ����� ����");
				return E_FAIL;
			}
		}
	}

	return S_OK;
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

HRESULT CLevel_Village::Ready_Village_Monster()
{
	MonsterInfo Info{};
	const TCHAR* pGetPath = L"../Bin/Data/Village_MonsterData.dat";

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("../Bin/Data/Village_MonsterData.dat ���� �ҷ����� ����.");
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

		if (Info.strMonsterPrototype == TEXT("Prototype_Model_Rabbit"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Rabbit"), TEXT("Prototype_GameObject_Rabbit"), &Info)))
			{
				MSG_BOX("Rabbit ���� ����");
				return E_FAIL;
			}

		}
		else if (Info.strMonsterPrototype == TEXT("Prototype_Model_Furgoat"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Goat"), TEXT("Prototype_GameObject_Goat"), &Info)))
			{
				MSG_BOX("Furgoat ���� ����");
				return E_FAIL;
			}

		}
	}

	return S_OK;
}

HRESULT CLevel_Village::Ready_Dungeon_Monster()
{
	MonsterInfo Info{};
	const TCHAR* pGetPath = L"../Bin/Data/Dungeon_MonsterData.dat";

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("../Bin/Data/Dungeon_MonsterData.dat ���� �ҷ����� ����.");
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

		if (Info.strMonsterPrototype == TEXT("Prototype_Model_TrilobiteA"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_TrilobiteA"), TEXT("Prototype_GameObject_TrilobiteA"), &Info)))
			{
				MSG_BOX("TrilobiteA ���� ����");
				return E_FAIL;
			}

		}
		else if (Info.strMonsterPrototype == TEXT("Prototype_Model_Nastron03"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Nastron03"), TEXT("Prototype_GameObject_Nastron03"), &Info)))
			{
				MSG_BOX("Nastron03 ���� ����");
				return E_FAIL;
			}

		}
		else if (Info.strMonsterPrototype == TEXT("Prototype_Model_Imp"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Imp"), TEXT("Prototype_GameObject_Imp"), &Info)))
			{
				MSG_BOX("Imp ���� ����");
				return E_FAIL;
			}

		}
		else if (Info.strMonsterPrototype == TEXT("Prototype_Model_Nastron07"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Nastron07"), TEXT("Prototype_GameObject_Nastron07"), &Info)))
			{
				MSG_BOX("Nastron07 ���� ����");
				return E_FAIL;
			}

		}
		else if (Info.strMonsterPrototype == TEXT("Prototype_Model_Void23"))
		{
			//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Void23"), TEXT("Prototype_GameObject_Void23"), &Info)))
			//{
			//	MSG_BOX("Void23 ���� ����");
			//	return E_FAIL;
			//}

		}

	}

	return S_OK;
}

HRESULT CLevel_Village::Ready_Groar_Boss()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Groar_Boss"), TEXT("Prototype_GameObject_Groar_Boss"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Village::Ready_Human_Boss()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Boss"), TEXT("Prototype_GameObject_Human_Boss"))))
	{
		return E_FAIL;
	}
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

		//if (Info.strNPCPrototype == TEXT("Prototype_Model_BlackSmith"))
		//{
		//	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_BlackSmith"), TEXT("Prototype_GameObject_BlackSmith"), &Info)))
		//	{
		//		MSG_BOX("BlackSmith ���� ����");
		//		return E_FAIL;
		//	}

		//}
		//else if (Info.strNPCPrototype == TEXT("Prototype_Model_ItemMerchant"))
		//{
		//	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_ItemMerchant"), TEXT("Prototype_GameObject_ItemMerchant"), &Info)))
		//	{
		//		MSG_BOX("ItemMerchant ���� ����");
		//		return E_FAIL;
		//	}

		//}
		//else if (Info.strNPCPrototype == TEXT("Prototype_Model_Roskva"))
		//{
		//	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Roskva"), TEXT("Prototype_GameObject_Roskva"), &Info)))
		//	{
		//		MSG_BOX("Roskva ���� ����");
		//		return E_FAIL;
		//	}

		//}
		//else if (Info.strNPCPrototype == TEXT("Prototype_Model_Cat"))
		//{
		//	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Cat"), TEXT("Prototype_GameObject_Cat"), &Info)))
		//	{
		//		MSG_BOX("Cat ���� ����");
		//		return E_FAIL;
		//	}

		//}
		//else if (Info.strNPCPrototype == TEXT("Prototype_Model_Dog"))
		//{
		//	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Dog"), TEXT("Prototype_GameObject_Dog"), &Info)))
		//	{
		//		MSG_BOX("Dog ���� ����");
		//		return E_FAIL;
		//	}

		//}

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

HRESULT CLevel_Village::Ready_SescoGame()
{
	/*for (_uint i = 0; i < 100; i++)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_Model_VTFMonster_Void19");

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_SescoGameObject"), TEXT("Prototype_GameObject_Void19_Object"), &VTFMonsterDesc)))
			return E_FAIL;
	}
	for (_uint i = 0; i < 100; i++)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_Model_VTFMonster_Muspelheim06");

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_SescoGameObject"), TEXT("Prototype_GameObject_Void19_Object"), &VTFMonsterDesc)))
			return E_FAIL;
	}
	for (_uint i = 0; i < 100; i++)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_Model_VTFMonster_Niflheim09");

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_SescoGameObject"), TEXT("Prototype_GameObject_Void19_Object"), &VTFMonsterDesc)))
			return E_FAIL;
	}
	for (_uint i = 0; i < 100; i++)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_Model_VTFMonster_Redant");

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_SescoGameObject"), TEXT("Prototype_GameObject_Void19_Object"), &VTFMonsterDesc)))
			return E_FAIL;
	}
	for (_uint i = 0; i < 100; i++)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_Model_VTFMonster_Scorpion");

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_SescoGameObject"), TEXT("Prototype_GameObject_Void19_Object"), &VTFMonsterDesc)))
			return E_FAIL;
	}
	for (_uint i = 0; i < 100; i++)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_Model_VTFMonster_Trilobites");

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_SescoGameObject"), TEXT("Prototype_GameObject_Void19_Object"), &VTFMonsterDesc)))
			return E_FAIL;
	}
	for (_uint i = 0; i < 100; i++)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_Model_VTFMonster_Trilobites_A");

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_SescoGameObject"), TEXT("Prototype_GameObject_Void19_Object"), &VTFMonsterDesc)))
			return E_FAIL;
	}*/
	return S_OK;
}

HRESULT CLevel_Village::Ready_Trigger()
{
	return CTrigger_Manager::Get_Instance()->Ready_Trigger_Village();
}

HRESULT CLevel_Village::Ready_Test()
{
	//if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Test"), TEXT("Prototype_GameObject_Nastron03"))))
	//{
	//	return E_FAIL;
	//}

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
