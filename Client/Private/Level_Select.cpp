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

	if (FAILED(Ready_Select()))
	{
		MSG_BOX("Failed to Ready Select");
		return E_FAIL;
	}
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

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_SELECT, strLayerTag, TEXT("Prototype_GameObject_Camera_Custom"), &CamDesc)))
	{
		return E_FAIL;
	}

	
	m_pGameInstance->Set_CameraModeIndex(CM_CUSTOM);

	return S_OK;
}

HRESULT CLevel_Select::Ready_Light()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(-1.f, -2.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);

	return m_pGameInstance->Add_Light(LEVEL_SELECT, TEXT("Light_Select"), LightDesc);
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
