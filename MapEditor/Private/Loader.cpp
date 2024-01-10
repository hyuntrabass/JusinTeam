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

#pragma region UI
#pragma endregion

#pragma region Effect
	// Effect Textures
	string strInputFilePath = "../Bin/Resources/Textures/Effect/";
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

	m_strLoadingText = L"Editor : Loading Model";
#pragma region Model
	_matrix Pivot = XMMatrixRotationAxis(XMVectorSet(-1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sphere"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/Common/Mesh/SM_EFF_Sphere_02.mo.hyuntrastatmesh", false, Pivot))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sphere_HorizentalUV"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/Common/Mesh/SM_EFF_Sphere_01.mo.hyuntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Ring_14"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/Common/Mesh/SM_EFF_Ring_14.mo.hyuntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Circle"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/Common/Mesh/SM_EFF_Circle_01.mo.hyuntrastatmesh"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Wood"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/Common/Mesh/SM_EFF_Tree_01.mo.hyuntrastatmesh"))))
	{
		return E_FAIL;
	}
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
	//m_strLoadingText = L"Editor : Loading ImGui";
	//if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Menu"), CMenu::Create(m_pDevice, m_pContext))))
	//{
	//	return E_FAIL;
	//}
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

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext))))
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
