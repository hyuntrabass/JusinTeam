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
	case Client::LEVEL_VILLAGE:
		hr = Load_Village();
		break;
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
	if (FAILED(m_pGameInstance->Init_SoundManager()))
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

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Logo_Noise"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI//Logo/Noise.dds")))))
	{
		return E_FAIL;
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

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Riding_Bird"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Riding/Mesh/Bird.hyuntraanimmesh"))))
				return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Riding_Tiger"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Riding/Mesh/Tiger.hyuntraanimmesh"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Riding_Nihilir"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Riding/Mesh/Nihilir.hyuntraanimmesh"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Arrow"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/Arrow/Mesh/arrow.hyuntrastatmesh"))))
		return E_FAIL;
	// WorldMap

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_WorldMap_Ground"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/WorldMap/Mesh/ground.hyuntrastatmesh"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_WorldMap_Object"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/WorldMap/Mesh/object.hyuntrastatmesh"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_WorldMap_Water"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/WorldMap/Mesh/water.hyuntrastatmesh"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Texture_WorldMap_Cloud"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/cloud.dds")))))
		return E_FAIL;


	// Load Instance Data
	_mat Pivot = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Environment/Grass/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), false, Pivot))))
			{
				return E_FAIL;
			}
		}
	}

	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Environment/Rock/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), true, Pivot))))
			{
				return E_FAIL;
			}
		}
	}

	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Environment/Tree/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), true, Pivot))))
			{
				return E_FAIL;
			}
		}
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
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_WorldMap"), CWorldMap::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Arrow"), CArrow::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Riding"), CRiding::Create(m_pDevice, m_pContext))))
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
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Quest"), CQuest::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_QuestBox"), CQuestBox::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_CommonTrail"), CCommonTrail::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_CommonSurfaceTrail"), CCommonSurfaceTrail::Create(m_pDevice, m_pContext))))
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
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Button_Blue"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Select/buttonBlue.png")))))
	{
		return E_FAIL;
	
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Button_Blue2"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Select/buttonBlue2.png")))))
	{
		return E_FAIL;
	
	}
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_NameWindow"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Custom/NameWindow.png")))))
	{
		return E_FAIL;
	
	}


	m_strLoadingText = L"Select : Loading Model";
#pragma region Model
	string strInputFilePath{};

#pragma region 지형 맵


#pragma endregion

	strInputFilePath = "../Bin/Resources/StaticMesh/Effect/Mesh/";
	_uint iMeshNumber{};
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Model_Effect_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}

	_mat PivotMat = _mat::CreateScale(0.006f) * _mat::CreateRotationX(XMConvertToRadians(90.f));

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, TEXT("Prototype_Model_Select_Map"),
														CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/Select_Map/Mesh/map.hyuntrastatmesh", false, PivotMat))))
	{
		return E_FAIL;
	}

	PivotMat = _mat::CreateScale(0.012f);
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Sky"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/StaticMesh/Sky/Mesh/sky.hyuntrastatmesh",false , PivotMat))))
	{
		return E_FAIL;
	}
	 strInputFilePath = "../Bin/Resources/StaticMesh/Sky/Texture/";
	 _uint iIndex{};
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Component_Texture_Sky") + to_wstring(iIndex);

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CTexture::Create(m_pDevice, m_pContext, entry.path().wstring()))))
			{
				return E_FAIL;
			}
			iIndex++;
		}
	}

	strInputFilePath = "../Bin/Resources/AnimMesh/Select_Model/Mesh/";
	iIndex = 0;
	PivotMat = _mat::CreateScale(0.01f) * _mat::CreateRotationY(XMConvertToRadians(90.f));
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_SELECT, TEXT("Prototype_Model_Select0"), CModel::Create(m_pDevice, m_pContext, strInputFilePath + "Select_Priest.hyuntraanimmesh", false, PivotMat))))
	{
		return E_FAIL;
	}

	PivotMat = _mat::CreateScale(0.01f) *_mat::CreateRotationY(XMConvertToRadians(30.f));
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

	// VTF Test
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VTF"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VTFModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_RTVTF"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_RT_VTFModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
	{
		return E_FAIL;
	}

	CRealtimeVTFModel* pModel = CRealtimeVTFModel::Create(m_pDevice, m_pContext, 
		"../Bin/Resources/AnimMesh/VTFPlayer/Main/basemodel.hyuntraanimmesh");


	strInputFilePath = "../Bin/Resources/AnimMesh/VTFPlayer/Part/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (FAILED(pModel->Seting_Parts(entry.path().string())))
				return E_FAIL;
		}
	}



	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Player"), pModel)))
		return E_FAIL;



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

	//if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Map"), CMap::Create(m_pDevice, m_pContext))))
	//{
	//	return E_FAIL;
	//}

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
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_NameWindow"), CNameWindow::Create(m_pDevice, m_pContext))))
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

	m_strLoadingText = L"GamePlay : Loading Texture";
#pragma region Texture

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, L"Prototype_Component_Texture_Water_Normal",
		CTexture::Create(m_pDevice, m_pContext, L"../../Client/Bin/Resources/Textures/waterNormal.dds"))))
		return E_FAIL;
#pragma endregion

#pragma region UI
	string strInputFilePath = "../Bin/Resources/Textures/UI/Gameplay";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = TEXT("Prototype_Component_Texture_UI_Gameplay_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CTexture::Create(m_pDevice, m_pContext, entry.path().wstring()))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../Bin/Resources/Textures/UI/Item";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring strPrototypeTag = entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CTexture::Create(m_pDevice, m_pContext, entry.path().wstring()))))
			{
				return E_FAIL;
			}
		}
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Tuto"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Tutorial/Tuto%d.png"), 3))))
	{
		return E_FAIL;
	}
#pragma endregion

#pragma region Terrain

	strInputFilePath = "../Bin/Resources/Textures/Terrain/";
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


	m_strLoadingText = L"GamePlay : Loading Model";
#pragma region Model


#pragma region Object
	m_strLoadingText = L"GamePlay : Loading Object Model";
	_mat Pivot = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	//_matrix Pivot = XMMatrixRotationAxis(XMVectorSet(-1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));

	// Prologue Object Model
	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Map/Tutorial/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), true, Pivot))))
			{
				return E_FAIL;
			}
		}
	}

	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Object/Tutorial/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), true, Pivot))))
			{
				return E_FAIL;
			}
		}
	}


#pragma endregion

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_VTFTest"),
		CVTFModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/VTFRabbit/Mesh/Rabbit.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

#pragma region Monster
	strInputFilePath = "../Bin/Resources/AnimMesh/Monster/0_ModelTest/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_ModelTest"), CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
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

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Furgoat"),
														CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/Furgoat/Mesh/Furgoat.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Nastron03"),
														CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/Nastron03/Mesh/Nastron03.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	Pivot = _mat::CreateScale(0.01f) * _mat::CreateRotationX(XMConvertToRadians(-90.f));
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

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_TrilobiteA"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/TrilobiteA/Mesh/TrilobiteA.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Void01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/Void01/Mesh/Void01.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Void05"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/Void05/Mesh/Void05.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Imp"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/Imp/Mesh/Imp.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	_mat Pivot1 = _mat::CreateScale(0.5f);
	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Void09"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/Void09/Mesh/Void09.hyuntraanimmesh", false, Pivot1))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Void20"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/Void20/Mesh/Void20.hyuntraanimmesh", false, Pivot1))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Void23"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/Void23/Mesh/Void23.hyuntraanimmesh", false, Pivot1))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Nastron07"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/Nastron07/Mesh/Nastron07.hyuntraanimmesh", false, Pivot1))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Spider"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Monster/Spider/Mesh/Spider.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

#pragma endregion Monster

#pragma region NPC

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Cat"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/NPC/Cat/Mesh/Cat.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Dog"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/NPC/Dog/Mesh/Dog.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_ItemMerchant"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/NPC/ItemMerchant/Mesh/ItemMerchant.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_BlackSmith"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/NPC/BlackSmith/Mesh/BlackSmith.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Roskva"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/NPC/Roskva/Mesh/Roskva.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Aur"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/NPC/Aur/Mesh/Aur.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	strInputFilePath = "../../Client/Bin/Resources/AnimMesh/NPC/NPC_Dummy/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;

			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string()))))
			{
				return E_FAIL;
			}
		}
	}

#pragma endregion NPC

#pragma  region Boss

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Groar"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/NPC/Groar/Mesh/Groar.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_GroarScene01"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Boss/GroarScene01/Mesh/GroarScene01.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_GroarScene02"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Boss/GroarScene02/Mesh/GroarScene02.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Groar_Boss"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Boss/Groar_Boss/Mesh/Groar_Boss.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

#pragma  endregion Boss

#pragma region Pet

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Pet_Cat"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Pet/Pet_Cat/Mesh/Pet_Cat.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_STATIC, TEXT("Prototype_Model_Pet_Dragon"),
		CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/AnimMesh/Pet/Pet_Dragon/Mesh/Pet_Dragon.hyuntraanimmesh"))))
	{
		return E_FAIL;
	}

#pragma endregion Pet

#pragma endregion


	m_strLoadingText = L"GamePlay : Loading Shader";
#pragma region Shader


	if(FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_GAMEPLAY, L"Prototype_Component_Shader_Water", 
		CShader::Create(m_pDevice, m_pContext, L"../../Client/Bin/ShaderFiles/Shader_Water.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

#pragma endregion

	m_strLoadingText = L"GamePlay : Loading Prototype";
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
#pragma region UI
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Player_HP"), CHPBar::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Player_MP"), CMPBar::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Player_Job"), CJobMark::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_SkillSlot"), CSkillSlot::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_SkillBlock"), CSkillBlock::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_ItemSlot"), CItemSlot::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_ItemBlock"), CItemBlock::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_ExpBar"), CExpBar::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_NameTag"), CNameTag::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Pop_QuestIn"), CPop_QuestIn::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Pop_QuestEnd"), CPop_QuestEnd::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_BlurTexture"), CBlurTexture::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Inven"), CInven::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Wearable_Slot"), CWearable_Slot::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Item"), CItem::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Coin"), CCoin::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Diamond"), CDiamond::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Menu"), CMenu::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_InvenFrame"), CInvenFrame::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Radar"), CRadar::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_ItemInfo"), CItemInfo::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_HPMonster"), CHPMonster::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_3DUITex"), C3DUITex::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Dialog"), CDialog::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_DialogText"), CDialogText::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Shop"), CShop::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_ShopDesc"), CShopDesc::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_ShopWindow"), CShopWindow::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_InvenWindow"), CInvenWindow::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Pop_Alert"), CPop_Alert::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Tutorial"), CTutorial::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	

#pragma endregion

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_VTFTest"), CVTFTest::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

#pragma region Monster

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_ModelTest"), CModelTest::Create(m_pDevice, m_pContext))))
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

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_TrilobiteA"), CTrilobiteA::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Void01"), CVoid01::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Void05"), CVoid05::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Imp"), CImp::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Void09"), CVoid09::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Void20"), CVoid20::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Void23"), CVoid23::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Nastron07"), CNastron07::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Spider"), CSpider::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

#pragma endregion Monster

#pragma region NPC

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Cat"), CCat::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Dog"), CDog::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_ItemMerchant"), CItemMerchant::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_BlackSmith"), CBlackSmith::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Roskva"), CRoskva::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Aur"), CAur::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_NPC_Dummy"), CNPC_Dummy::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

#pragma endregion NPC

#pragma region Boss

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Groar_Boss"), CGroar_Boss::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Missile"), CMissile::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

#pragma endregion Boss

#pragma region Pet

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Pet_Cat"), CPet_Cat::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Pet_Dragon"), CPet_Dragon::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}


#pragma endregion Pet

#pragma region Terrain

	// Terrain
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Terrain"), CTerrain::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}



#pragma endregion

#pragma region Prologue
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Prologue_Object"), CEtc_Object::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Prologue_Map"), CMap::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Prologue_Envir"), CEnvironment_Object::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(L"Prototype_GameObject_Water", CLake::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion

	m_strLoadingText = L"GamePlay : Loading Complete!";
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_Village()
{
	// 맵

	_mat Pivot = _mat::CreateScale(0.003f);
	string strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Map/Midgard/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_VILLAGE, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), true, Pivot))))
			{
				return E_FAIL;
			}
		}
	}

	_mat DungeonPivot = _mat::CreateScale(0.001f);
	 strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Map/Dungeon/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_VILLAGE, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), true, DungeonPivot))))
			{
				return E_FAIL;
			}
		}
	}
	strInputFilePath = "../../Client/Bin/Resources/StaticMesh/Object/Midgard/Mesh/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			if (!entry.exists())
				return S_OK;
			wstring strPrototypeTag = TEXT("Prototype_Model_") + entry.path().stem().wstring();

			if (FAILED(m_pGameInstance->Add_Prototype_Component(LEVEL_VILLAGE, strPrototypeTag, CModel::Create(m_pDevice, m_pContext, entry.path().string(), true, Pivot))))
			{
				return E_FAIL;
			}
		}
	}
	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Village_Map"), CMap::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Dungeon"), CMap::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Village_Etc_Object"), CEtc_Object::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype_GameObejct(TEXT("Prototype_GameObject_Village_Envir_Object"), CEnvironment_Object::Create(m_pDevice, m_pContext))))
	{
		return E_FAIL;
	}

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
