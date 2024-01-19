#include "Loader.h"
#include "GameInstance.h"

#include "LoaderIncludes.h"

CLoader::CLoader(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);
}

_uint APIENTRY ThreadEntry(void* pArg)
{
	if (FAILED(CoInitializeEx(nullptr, 0)))
		return E_FAIL;

	CLoader* pLoader = static_cast<CLoader*>(pArg);

	if (FAILED(pLoader->Begin_Thread()))
	{
		CoUninitialize();
		return FALSE;
	}

	if (FAILED(pLoader->Loading_LevelResources()))
	{
		CoUninitialize();
		return FALSE;
	}

	if (FAILED(pLoader->End_Thread()))
	{
		CoUninitialize();
		return FALSE;
	}

	CoUninitialize();
	return TRUE;
}

HRESULT CLoader::Init(LEVEL_ID eNextLevel)
{
	m_eNextLevel = eNextLevel;

	InitializeCriticalSection(&m_Critical_Section);

	m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, ThreadEntry, this, 0, nullptr));

	if (not m_hThread)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Begin_Thread()
{
	EnterCriticalSection(&m_Critical_Section);

	return S_OK;
}

HRESULT CLoader::End_Thread()
{
	LeaveCriticalSection(&m_Critical_Section);

	return S_OK;
}

HRESULT CLoader::Show_LoadingText()
{
	SetWindowText(g_hWnd, m_strLoadingText.c_str());

	return S_OK;
}

_bool CLoader::isFinished()
{
	return m_isFinished;
}

HRESULT CLoader::Loading_LevelResources()
{
	HRESULT hr = 0;

	switch (m_eNextLevel)
	{
	case AnimTool::LEVEL_TOOL:
		hr = Load_Tool();
		break;
		//case Client::LEVEL_CREATECHARACTER:
		//	hr = Load_CreateCharacter();
		//	break;
		//case Client::LEVEL_FOREST:
		//	hr = Load_Tutorial();
		//	break;
		//case Client::LEVEL_VILLAGE:
		//	hr = Load_VillageStage();
		//	break;
		//case Client::LEVEL_CLOUD:
		//	hr = Load_CloudStage();
		//	break;
	}

	if (FAILED(hr))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Load_Logo()
{
	m_strLoadingText = L"Logo : Loading Sounds";
	/*if (FAILED(m_pGameInstance->Init_SoundManager(SCH_MAX)))
	{
		return E_FAIL;
	}*/

	m_strLoadingText = L"Logo : Loading Texture";
#pragma region Texture

#pragma region UI
#pragma endregion

#pragma region Effect
#pragma endregion

#pragma endregion

	m_strLoadingText = L"Logo : Loading Model";
#pragma region Model
#pragma endregion

	m_strLoadingText = L"Logo : Loading Shader";
#pragma region Shader
#pragma endregion

	m_strLoadingText = L"Logo : Loading Prototype";
#pragma region Prototype
#pragma endregion

	m_strLoadingText = L"Logo : Loading Complete!";
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Tool()
{
	m_strLoadingText = L"CreateCharacter : Loading Sounds";
	/*if (FAILED(m_pGameInstance->Init_SoundManager(SCH_MAX)))
	{
		return E_FAIL;
	}*/

	m_strLoadingText = L"Logo : Loading Texture";
#pragma region Texture

#pragma region UI
#pragma endregion

#pragma region Effect
	// Effect Textures
	string strInputFilePath = "../../Client/Bin/Resources/Textures/Effect/";
	_uint iTextureNumber{};
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Component_Texture_Effect_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CTexture::Create(m_pDevice, m_pContext, entry.path().wstring()))))
			{
				return E_FAIL;
			}
		}
	}
#pragma endregion

#pragma endregion

	m_strLoadingText = L"Logo : Loading Model";
#pragma region Model
	//Monster
	_matrix Pivot = XMMatrixScaling(0.02f, 0.02f, 0.02f); 
	strInputFilePath = "../../Client/Bin/Resources/AnimMesh/Monster/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension().string() != ".hyuntraanimmesh")
			{
				continue;
			}
			wstring strPrototypeTag = TEXT("Prototype_Model_Monster_") + to_wstring(m_iNumMonsterModels++);

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}
	/*if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Monster_0"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Balrog/Mesh/Balrog.hyuntraanimmesh"))))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension().string() != ".hyuntraanimmesh")
			{
				continue;
			}
			wstring strPrototypeTag = TEXT("Prototype_ModelMonster") + to_wstring(m_iNumMonsterModels++);

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}
	m_iNumMonsterModels++;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Monster_6"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Nastron03/Mesh/Nastron03.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}
	m_iNumMonsterModels++;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Monster_7"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Nott/Mesh/Nott.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}
	m_iNumMonsterModels++;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Monster_8"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/NPCvsMon/Mesh/NPCvsMon.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}
	m_iNumMonsterModels++;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Monster_9"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Orc02/Mesh/Orc02.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}
	m_iNumMonsterModels++;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Monster_10"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Penguin/Mesh/Penguin.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}
	m_iNumMonsterModels++;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Monster_11"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Rabbit/Mesh/Rabbit.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}
	m_iNumMonsterModels++;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Monster_12"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Skjaldmaer/Mesh/Skjaldmaer.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}
	m_iNumMonsterModels++;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Monster_13"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Skjaldmaer_A/Mesh/Skjaldmaer_A.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}
	m_iNumMonsterModels++;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Monster_14"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Thief04/Mesh/Thief04.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}
	m_iNumMonsterModels++;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Monster_15"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Trilobite/Mesh/Trilobite.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}
	m_iNumMonsterModels++;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Monster_16"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/TrilobiteA/Mesh/TrilobiteA.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}
	m_iNumMonsterModels++;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Monster_17"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/Void13/Mesh/Void13.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}
	m_iNumMonsterModels++;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Monster_18"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Monster/VoidDragon/Mesh/VoidDragon.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}
	m_iNumMonsterModels++;*/
	////Player
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Player_0"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Select_Model/Mesh/Select_Priest.hyuntraanimmesh", false, Pivot))))
	{
		return E_FAIL;
	}
	m_iNumPlayerModels++;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Player_1"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Select_Model/Mesh/Select_Rogue.hyuntraanimmesh", false, Pivot))))
	{
		return E_FAIL;
	}
	m_iNumPlayerModels++;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Player_2"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Select_Model/Mesh/Select_Sorceress.hyuntraanimmesh", false, Pivot))))
	{
		return E_FAIL;
	}
	m_iNumPlayerModels++;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_TOOL, TEXT("Prototype_Model_Player_3"), CModel::Create(m_pDevice, m_pContext, "../../Client/Bin/Resources/AnimMesh/Select_Model/Mesh/Select_Warrior.hyuntraanimmesh", false, Pivot))))
	{
		return E_FAIL;
	}
	m_iNumPlayerModels++;
#pragma endregion

	m_strLoadingText = L"Logo : Loading Shader";
#pragma region Shader
#pragma endregion

	m_strLoadingText = L"Logo : Loading Prototype";
#pragma region Prototype
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Camera_Debug"), CCamera_Debug::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Camera_Main"), CCamera_Main::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext, m_iNumMonsterModels, m_iNumPlayerModels))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_EffectDummy"), CEffect_Dummy::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

#pragma endregion

	CEffect_Manager::Get_Instance()->Read_EffectFile();

	m_strLoadingText = L"Logo : Loading Complete!";
	m_isFinished = true;

	return S_OK;
}

CLoader* CLoader::Create(_dev pDevice, _context pContext, LEVEL_ID eNextLevel)
{
	CLoader* pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Init(eNextLevel)))
	{
		MSG_BOX("Failed to Create : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);

	CloseHandle(m_hThread);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);
}
