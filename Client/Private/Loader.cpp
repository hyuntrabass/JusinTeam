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
	case Client::LEVEL_LOGO:
		hr = Load_Logo();
		break;
	case Client::LEVEL_SELECT:
		hr = Load_Select();
		break;
	case Client::LEVEL_CUSTOM:
		hr = Load_Custom();
		break;
	case Client::LEVEL_GAMEPLAY:
		hr = Load_GamePlay();
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
	if (FAILED(m_pGameInstance->Init_SoundManager(SCH_MAX)))
	{
		return E_FAIL;
	}

	m_strLoadingText = L"Logo : Loading Texture";
#pragma region Texture

#pragma region UI


	string strInputFilePath = "../Bin/Resources/Textures/UI/Logo";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Component_Texture_UI_Logo_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_LOGO, strPrototypeTag, CTexture::Create(m_pDevice, m_pContext, entry.path().wstring()))))
			{
				return E_FAIL;
			}
		}
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_FadeBox"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/FadeBox.png")))))
	{
		return E_FAIL;
	}
#pragma endregion


#pragma region Effect
	// Effect Textures
	strInputFilePath = "../Bin/Resources/Textures/Effect/";
	_uint iTextureNumber{};
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Component_Texture_Effect_") + to_wstring(iTextureNumber++);

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
	_uint iIndex{};
	strInputFilePath = "../Bin/Resources/AnimMesh/Player/test/face/";
	_mat PivotMat = _mat::CreateScale(0.4f) * _mat::CreateRotationX(XMConvertToRadians(-90.f)) * _mat::CreateRotationY(XMConvertToRadians(180.f));
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + to_wstring(PT_FACE) + L"" + to_wstring(iIndex++);
			string strFilePath = entry.path().filename().string();
			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath, false, PivotMat))))
			{
				return E_FAIL;
			}
		}
	}
	iIndex = 0;;
	strInputFilePath = "../Bin/Resources/AnimMesh/Player/test/hair/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + to_wstring(PT_HAIR) + L"" + to_wstring(iIndex++);
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath, false, PivotMat))))
			{
				return E_FAIL;
			}
		}
	}
	iIndex = 0;;
	strInputFilePath = "../Bin/Resources/AnimMesh/Player/test/body/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_") + to_wstring(PT_BODY) + L"" + to_wstring(iIndex++);
			string strFilePath = entry.path().filename().string();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, strInputFilePath + strFilePath, false, PivotMat))))
			{
				return E_FAIL;
			}
		}
	}

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

	m_strLoadingText = L"Logo : Loading Shader";
#pragma region Shader
#pragma endregion

	m_strLoadingText = L"Logo : Loading Prototype";

#pragma region Prototype
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Background"), CBackGround::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Logo"), CLogo::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Logo_Odin"), COdin::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Logo_Dust"), CLogo_Dust::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Logo_Stone"), CLogo_Stone::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Logo_Text"), CLogo_Text::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_EffectDummy"), CEffect_Dummy::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_FadeBox"), CFadeBox::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}


#pragma endregion

	CEffect_Manager::Get_Instance()->Read_EffectFile();

	m_strLoadingText = L"Logo : Loading Complete!";
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Select()
{
	m_strLoadingText = L"Select : Loading Texture";
#pragma region Texture
	/*
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, TEXT("Prototype_Model_Void05"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/Void05/Mesh/Void05.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}
	*/

#pragma region UI
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, TEXT("Prototype_Component_Texture_UI_Select_Bg_PlayerSelectTitle"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Select/Bg_PlayerSelectTitle.png")))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, TEXT("Prototype_Component_Texture_UI_Select_Mask_Rps"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Select/Mask_Rps.png")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Select_BG_BoxEfc_WhiteBlur"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Select/BG_BoxEfc_WhiteBlur.png")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, TEXT("Prototype_Component_Texture_UI_Select_Warrior"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Select/Warrior.png")))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, TEXT("Prototype_Component_Texture_UI_Select_Soceress"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Select/Sorceress.png")))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, TEXT("Prototype_Component_Texture_UI_Select_Priest"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Select/Priest.png")))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, TEXT("Prototype_Component_Texture_UI_Select_Rogue"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Select/Rogue.png")))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Back"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Select/titleback.png")))))
	{
		return E_FAIL;
	
	}
	/*

	string strInputFilePath = "../Bin/Resources/Textures/UI/Select";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Component_Texture_UI_Select_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, strPrototypeTag, CTexture::Create(m_pDevice, m_pContext, entry.path().wstring()))))
			{
				return E_FAIL;
			}
		}
	}*/
#pragma endregion

	/*
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
*/

	m_strLoadingText = L"Select : Loading Model";
#pragma region Model
	_mat PivotMat;
	PivotMat = _mat::CreateScale(0.006f) * _mat::CreateRotationX(XMConvertToRadians(90.f));

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, TEXT("Prototype_Model_Select_Map"),
														CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/Select_Map/Mesh/map.hyuntrastatmesh", false, PivotMat))))
	{
		return E_FAIL;
	}

	/*if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sky"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/Select_Map/Mesh/map.hyuntrastatmesh", false, PivotMat))))
	{
		return E_FAIL;
	}*/

	string strInputFilePath{};
	strInputFilePath = "../Bin/Resources/AnimMesh/Select_Model/Mesh/";
	int iIndex{};
	PivotMat = _mat::CreateScale(0.01f) * _mat::CreateRotationY(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, TEXT("Prototype_Model_Select0"), CModel::Create(m_pDevice, m_pContext, strInputFilePath + "Select_Priest.hyuntraanimmesh", false, PivotMat))))
	{
		return E_FAIL;
	}

	PivotMat = _mat::CreateScale(0.01f) * _mat::CreateRotationY(XMConvertToRadians(30.f));
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, TEXT("Prototype_Model_Select1"), CModel::Create(m_pDevice, m_pContext, strInputFilePath + "Select_Rogue.hyuntraanimmesh", false, PivotMat))))
	{
		return E_FAIL;
	}

	PivotMat = _mat::CreateScale(0.01f);
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, TEXT("Prototype_Model_Select2"), CModel::Create(m_pDevice, m_pContext, strInputFilePath + "Select_Sorceress.hyuntraanimmesh", false, PivotMat))))
	{
		return E_FAIL;
	}

	PivotMat = _mat::CreateScale(0.01f) * _mat::CreateRotationY(XMConvertToRadians(-20.f));
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, TEXT("Prototype_Model_Select3"), CModel::Create(m_pDevice, m_pContext, strInputFilePath + "Select_Warrior.hyuntraanimmesh", false, PivotMat))))
	{
		return E_FAIL;
	}

	strInputFilePath = "../Bin/Resources/AnimMesh/Select_Npc/Mesh/";
	iIndex = 0;
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{

			wstring strPrototypeTag = TEXT("Prototype_Model_Select_Npc") + to_wstring(iIndex++);
			if (iIndex == 2)
			{
				PivotMat = _mat::CreateScale(0.01f) * _mat::CreateRotationY(XMConvertToRadians(-40.f));
				if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, PivotMat))))
				{
					return E_FAIL;
				}

			}
			else if (iIndex == 1)
			{
				PivotMat = _mat::CreateScale(0.01f) * _mat::CreateRotationY(XMConvertToRadians(-90.f));
				if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, PivotMat))))
				{
					return E_FAIL;
				}

			}
			else if (iIndex == 3)
			{
				PivotMat = _mat::CreateScale(0.01f) * _mat::CreateRotationY(XMConvertToRadians(90.f));
				if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, PivotMat))))
				{
					return E_FAIL;
				}

			}
			else
			{
				PivotMat = _mat::CreateScale(0.008f) * _mat::CreateRotationY(XMConvertToRadians(-150.f));
				if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, PivotMat))))
				{
					return E_FAIL;
				}
			}


		}
	}
#pragma endregion

	m_strLoadingText = L"Select : Loading Shader";
#pragma region Shader
#pragma endregion

	m_strLoadingText = L"Select : Loading Prototype";

#pragma region Prototype
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Select"), CSelect::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_CharacterSelect"), CCharacterSelect::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_TextButton"), CTextButton::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_TextButtonColor"), CTextButtonColor::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Select_Npc"), CSelect_Npc::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_SelectDesc"), CSelectDesc::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Select_Model"), CSelect_Model::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Select_Map"), CSelect_Map::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Sky"), CSky::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Camera_Debug"), CCamera_Debug::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}




	//나중에 옮길 예정
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BackGround_Mask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/BackGround/BackGround.png")))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_BackGround_Mask"), CBackGround_Mask::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}


#pragma endregion

	m_strLoadingText = L"Select : Loading Complete!";
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Custom()
{
	m_strLoadingText = L"Custom : Loading Texture";
#pragma region Texture

#pragma region UI
	string strInputFilePath = "../Bin/Resources/Textures/UI/Custom";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Component_Texture_UI_Custom_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_CUSTOM, strPrototypeTag, CTexture::Create(m_pDevice, m_pContext, entry.path().wstring()))))
			{
				return E_FAIL;
			}
		}
	}
#pragma endregion


	m_strLoadingText = L"Custom : Loading Model";
#pragma region Model

#pragma endregion

	m_strLoadingText = L"Custom : Loading Shader";
#pragma region Shader
#pragma endregion

	m_strLoadingText = L"Custom : Loading Prototype";

#pragma region Prototype
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Custom"), CCustom::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Body_Parts"), CBodyPart::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
#pragma endregion

	m_strLoadingText = L"Custom : Loading Complete!";
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_GamePlay()
{
	m_strLoadingText = L"CreateCharacter : Loading Sounds";
	//if (FAILED(m_pGameInstance->Init_SoundManager(SCH_MAX)))
	//{
	//	return E_FAIL;
	//}

	m_strLoadingText = L"Logo : Loading Texture";
#pragma region Texture

#pragma region UI
#pragma endregion

#pragma region Terrain

	// Terrain -> 테스트용도
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


#pragma region Effect
#pragma endregion

#pragma endregion

	m_strLoadingText = L"Logo : Loading Model";
#pragma region Model
	//strInputFilePath = "../Bin/Resources/AnimMesh/Player/Mesh/";
	//for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	//{
	//	if (entry.is_regular_file())
	//	{

	//		if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Player"), CModel::Create(m_pDevice, m_pContext, entry.path().string()/*,false,_mat::CreateScale(10.f)*/))))
	//		{
	//			return E_FAIL;
	//		}

	//	}
	//}

#pragma region Monster
	strInputFilePath = "../Bin/Resources/AnimMesh/Monster/0_ModelTest/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Void05"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}

		}
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Rabbit"),
														CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/Rabbit/Mesh/Rabbit.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Goat"),
														CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/Goat/Mesh/Furgoat.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Nastron03"),
														CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/Nastron03/Mesh/Nastron03.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	_mat Pivot = _mat::CreateScale(0.01f) * _mat::CreateRotationX(XMConvertToRadians(-90.f));
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_NPCvsMon"),
														CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/NPCvsMon/Mesh/NPCvsMon.hyuntraanimmesh", false, Pivot))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Thief04"),
														CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/Thief04/Mesh/Thief04.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

#pragma endregion Monster

#pragma endregion



	m_strLoadingText = L"Logo : Loading Shader";
#pragma region Shader
#pragma endregion

	m_strLoadingText = L"Logo : Loading Prototype";
#pragma region Prototype
	/*

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Camera_Main"), CCamera_Main::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	*/

	//if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext))))
	//{
	//	return E_FAIL;
	//}

#pragma region Monster

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Void05"), CVoid05::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Rabbit"), CRabbit::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Goat"), CGoat::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Nastron03"), CNastron03::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_NPCvsMon"), CNPCvsMon::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Thief04"), CThief04::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

#pragma endregion Monster

	// Terrain -> 테스트용도
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

#pragma endregion

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
