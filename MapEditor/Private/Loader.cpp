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

	case MapEditor::LEVEL_EDITOR:
		hr = Load_Editor();
		break;

	}

	if (FAILED(hr))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Load_Editor()
{
	m_strLoadingText = L"Editor : Loading Sounds";
	if (FAILED(m_pGameInstance->Init_SoundManager(SCH_MAX)))
	{
		return E_FAIL;
	}

	m_strLoadingText = L"Editor : Loading Texture";
#pragma region Texture

	// Effect Textures
	string strInputFilePath = "../Bin/Resources/Textures/Terrain/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Component_Texture_Terrain");

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CTexture::Create(m_pDevice, m_pContext, entry.path().wstring()))))
			{
				return E_FAIL;
			}
		}
	}

#pragma endregion

#pragma region UI
#pragma endregion

#pragma region Effect
	// Effect Textures
	strInputFilePath = "../Bin/Resources/Textures/Effect/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Component_Texture_Effect_") + entry.path().parent_path().stem().wstring() + TEXT("_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CTexture::Create(m_pDevice, m_pContext, entry.path().wstring()))))
			{
				return E_FAIL;
			}
		}
	}
#pragma endregion

#pragma endregion

	m_strLoadingText = L"Editor : Loading Object Model";
#pragma region Model
	_matrix Pivot = XMMatrixScaling(0.01f,0.01f,0.01f);
	//_matrix Pivot = XMMatrixRotationAxis(XMVectorSet(-1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));

	// Object Model
	strInputFilePath = "../Bin/Resources/StaticMesh/Object/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}

	//Pivot = XMMatrixScaling(1.f, 1.f, 1.f);
	// Map Model
	strInputFilePath = "../Bin/Resources/StaticMesh/Map/Midgard/Map1/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../Bin/Resources/StaticMesh/Map/Midgard/Map2/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../Bin/Resources/StaticMesh/Map/Midgard/Map3/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../Bin/Resources/StaticMesh/Map/Midgard/Map4/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../Bin/Resources/StaticMesh/Map/Midgard/Map5/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../Bin/Resources/StaticMesh/Map/Midgard/Map6/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../Bin/Resources/StaticMesh/Map/Midgard/Map7/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../Bin/Resources/StaticMesh/Map/Midgard/Map8/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../Bin/Resources/StaticMesh/Map/Midgard/Map9/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../Bin/Resources/StaticMesh/Map/Midgard/Map10/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../Bin/Resources/StaticMesh/Map/Midgard/Map11/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../Bin/Resources/StaticMesh/Map/Midgard/Map12/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../Bin/Resources/StaticMesh/Map/Midgard/Map13/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../Bin/Resources/StaticMesh/Map/Midgard/Map14/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../Bin/Resources/StaticMesh/Map/Midgard/Map15/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../Bin/Resources/StaticMesh/Map/Midgard/Map16/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}

#pragma endregion

	m_strLoadingText = L"Editor : Loading Monster Model";
#pragma region Model
	// Barlog
	strInputFilePath = "../Bin/Resources/AnimMesh/Monster/Barlog/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Barlog"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}

	// Furgoat
	strInputFilePath = "../Bin/Resources/AnimMesh/Monster/Furgoat/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Furgoat"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}

	// GiantBoss
	strInputFilePath = "../Bin/Resources/AnimMesh/Monster/GiantBoss/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_GiantBoss"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}

	// Nastron02
	strInputFilePath = "../Bin/Resources/AnimMesh/Monster/Nastron02/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Nastron02"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}

	// Nastron03
	strInputFilePath = "../Bin/Resources/AnimMesh/Monster/Nastron03/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Nastron03"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}

	// Orc02
	strInputFilePath = "../Bin/Resources/AnimMesh/Monster/Orc02/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Orc02"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}

	// Penguin
	strInputFilePath = "../Bin/Resources/AnimMesh/Monster/Penguin/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Penguin"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}

	// Rabbit
	strInputFilePath = "../Bin/Resources/AnimMesh/Monster/Rabbit/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Rabbit"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}

	// Thief04
	strInputFilePath = "../Bin/Resources/AnimMesh/Monster/Thief04/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Thief04"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}

	// Trilobite
	strInputFilePath = "../Bin/Resources/AnimMesh/Monster/Trilobite/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Trilobite"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}

	// TrilobiteA
	strInputFilePath = "../Bin/Resources/AnimMesh/Monster/TrilobiteA/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_TrilobiteA"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}

	// Void13
	strInputFilePath = "../Bin/Resources/AnimMesh/Monster/Void13/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Void13"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}

	// VoidDragon
	strInputFilePath = "../Bin/Resources/AnimMesh/Monster/VoidDragon/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_VoidDragon"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}

	// Void05
	strInputFilePath = "../Bin/Resources/AnimMesh/Monster/Void05/Mesh";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Void05"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}
#pragma endregion

#pragma region Map
	//m_strLoadingText = L"Editor : Loading Map Model";

	//strInputFilePath = "../Bin/Resources/StaticMesh/Common/Mesh";
	//for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	//{
	//	if (entry.is_regular_file())
	//	{
	//		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
	//		{
	//			return E_FAIL;
	//		}
	//	}
	//}
#pragma endregion

	

	m_strLoadingText = L"Editor : Loading Shader";
#pragma region Shader
#pragma endregion

	m_strLoadingText = L"Editor : Loading Prototype";
#pragma region Prototype
#pragma endregion

	m_strLoadingText = L"CreateCharacter : Loading Sounds";
	if (FAILED(m_pGameInstance->Init_SoundManager(SCH_MAX)))
	{
		return E_FAIL;
	}

	m_strLoadingText = L"Editor : Loading Texture";
#pragma region Texture

#pragma region UI

#pragma endregion

#pragma region Effect
#pragma endregion

#pragma endregion


	m_strLoadingText = L"Editor : Loading Prototype";
#pragma region Prototype
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Camera_Debug"), CCamera_Debug::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Camera_Main"), CCamera_Main::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Dummy"), CDummy::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Map"), CMap::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
#pragma endregion

	m_strLoadingText = L"Editor : Loading Complete!";
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
