#include "Level_Select.h"
#include "Level_Loading.h"
#include "Camera.h"

CLevel_Select::CLevel_Select(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Select::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_SELECT);
	UiInfo info{};
	info.strTexture = TEXT("Prototype_Component_Texture_BackGround_Mask"); 
	info.vPos = _vec2(640,360);
	info.vSize = _vec2(1280, 720);

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Mask"), TEXT("Prototype_GameObject_BackGround_Mask"),&info)))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_Select()))
	{
		MSG_BOX("Failed to Ready Select");
		return E_FAIL;
	}

	if (FAILED(Ready_Map()))
	{
		MSG_BOX("Failed to Ready SelectMap");
		return E_FAIL;
	}
	/*
	if (FAILED(Ready_Model()))
	{
		MSG_BOX("Failed to Ready SelectModel");
		return E_FAIL;
	}
	*/
	/*
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Void05"), TEXT("Prototype_GameObject_Void05"))))
	{
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
	if (FAILED(Ready_Npc()))
	{
		MSG_BOX("Failed to Ready Npc");
		return E_FAIL;
	}
	/*
	셀렉트 커스텀용 카메라 만들어야할듯
	

	if (FAILED(Ready_Light()))
	{
		MSG_BOX("Failed to Ready Light");
		return E_FAIL;
	}
	*/
	return S_OK;
}

void CLevel_Select::Tick(_float fTimeDelta)
{

	if (m_pGameInstance->Is_Level_ShutDown(LEVEL_SELECT))
	{
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Static"), TEXT("Prototype_GameObject_FadeBox"))))
		{
			return;
		}
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_CUSTOM))))
		{
			return;
		}

		return;
	}
	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
	{
		DestroyWindow(g_hWnd);
	}
}

HRESULT CLevel_Select::Render()
{
	return S_OK;
}

HRESULT CLevel_Select::Ready_Select()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_SELECT, TEXT("Layer_Select"), TEXT("Prototype_GameObject_Select"))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CLevel_Select::Ready_Model()
{
	ObjectInfo Info{};
	Info.strPrototypeTag = TEXT("Prototype_Model_Select0");

	_vec4 vPos;
	vPos = _vec4(1.8f, 0.07f, -2.4f, 1.f);
	Info.vPos = vPos;
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_SELECT, TEXT("Layer_Select_Model"), TEXT("Prototype_GameObject_Select_Model"), &Info)))
	{
		return E_FAIL;
	}
	vPos = _vec4(0.7f, 0.07f, -1.2f, 1.f);
	
	Info.vPos = vPos;
	Info.strPrototypeTag = TEXT("Prototype_Model_Select1");
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_SELECT, TEXT("Layer_Select_Model"), TEXT("Prototype_GameObject_Select_Model"), &Info)))
	{
		return E_FAIL;
	}

	vPos = _vec4(-2.2f, 0.07f, -2.7f, 1.f);
	Info.vPos = vPos;
	Info.strPrototypeTag = TEXT("Prototype_Model_Select2");
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_SELECT, TEXT("Layer_Select_Model"), TEXT("Prototype_GameObject_Select_Model"), &Info)))
	{
		return E_FAIL;
	}

	vPos = _vec4(-0.9f, 0.07f,-1.2f, 1.f);
	Info.vPos = vPos;
	Info.strPrototypeTag = TEXT("Prototype_Model_Select3");
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_SELECT, TEXT("Layer_Select_Model"), TEXT("Prototype_GameObject_Select_Model"), &Info)))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_Select::Ready_Npc()
{
	ObjectInfo Info{};
	Info.strPrototypeTag = TEXT("Prototype_Model_Select_Npc0");

	_vec4 vPos;
	vPos = _vec4(-0.3f, 0.07f, 2.3f, 1.f);
	Info.vPos = vPos;
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_SELECT, TEXT("Layer_Select_Npc"), TEXT("Prototype_GameObject_Select_Npc"), &Info)))
	{
		return E_FAIL;
	}

	vPos = _vec4(0.6f, 0.07f, 2.3f, 1.f);

	Info.vPos = vPos;
	Info.strPrototypeTag = TEXT("Prototype_Model_Select_Npc2");
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_SELECT, TEXT("Layer_Select_Npc"), TEXT("Prototype_GameObject_Select_Npc"), &Info)))
	{
		return E_FAIL;
	}

	vPos = _vec4(-2.2f, 0.07f, 1.2f, 1.f);
	Info.vPos = vPos;
	Info.strPrototypeTag = TEXT("Prototype_Model_Select_Npc1");
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_SELECT, TEXT("Layer_Select_Npc"), TEXT("Prototype_GameObject_Select_Npc"), &Info)))
	{
		return E_FAIL;
	}

	vPos = _vec4(1.6f, -0.2f, 1.2f, 1.f);
	Info.vPos = vPos;
	Info.strPrototypeTag = TEXT("Prototype_Model_Select_Npc3");
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_SELECT, TEXT("Layer_Select_Npc"), TEXT("Prototype_GameObject_Select_Npc"), &Info)))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_Select::Ready_Map()
{


	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_SELECT, TEXT("Layer_Select_Map"), TEXT("Prototype_GameObject_Select_Map"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Select::Ready_Camera()
{
	if (not m_pGameInstance)
	{
		return E_FAIL;
	}

	wstring strLayerTag = TEXT("Layer_Camera");

	CCamera::Camera_Desc CamDesc;
	CamDesc.vCameraPos = _float4(0.f, 5.f, -5.f, 1.f);
	CamDesc.vFocusPos = _float4(0.f, 0.f, 0.f, 1.f);
	CamDesc.fFovY = XMConvertToRadians(60.f);
	CamDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CamDesc.fNear = 0.1f;
	CamDesc.fFar = 1100.f;

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, strLayerTag, TEXT("Prototype_GameObject_Camera_Main"), &CamDesc)))
	{
		return E_FAIL;
	}

	m_pGameInstance->Set_CameraModeIndex(CM_MAIN);


	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, L"Layer_Camera_Debug", TEXT("Prototype_GameObject_Camera_Debug"), &CamDesc)))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CLevel_Select::Ready_Light()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Point;
	LightDesc.vAttenuation = LIGHT_RANGE_50;
	LightDesc.vDiffuse = _float4(1.f, 0.6f, 0.1f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vPosition = _float4(0.134f, 0.5f,-3.2f, 1.f);
	LightDesc.vSpecular = _vec4(1.f);

	for (size_t i = 0; i < 1; i++)
	{
		if (FAILED(m_pGameInstance->Add_Light(LEVEL_SELECT, TEXT("Light_Select") + to_wstring(i), LightDesc)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

CLevel_Select* CLevel_Select::Create(_dev pDevice, _context pContext)
{
	CLevel_Select* pInstance = new CLevel_Select(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to create : CLevel_Select");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Select::Free()
{
	__super::Free();
}
