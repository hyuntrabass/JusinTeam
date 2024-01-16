#include "Level_Custom.h"
#include "Level_Loading.h"
#include "Camera.h"

CLevel_Custom::CLevel_Custom(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Custom::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_CUSTOM);

	if (FAILED(Ready_Player()))
	{
		MSG_BOX("Failed to Ready Player");
		return E_FAIL;
	}

	if (FAILED(Ready_UI()))
	{
		MSG_BOX("Failed to Ready UI");
		return E_FAIL;
	}



	if (FAILED(Ready_Light()))
	{
		MSG_BOX("Failed to Ready Light");
		return E_FAIL;
	}



	return S_OK;
}

void CLevel_Custom::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Is_Level_ShutDown(LEVEL_CUSTOM))
	{
		if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY))))
		{
			return;
		}

		return;
	}
	m_pGameInstance->Set_HellHeight(-2000.f);

	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
	{
		DestroyWindow(g_hWnd);
	}
}

HRESULT CLevel_Custom::Render()
{
	return S_OK;
}


HRESULT CLevel_Custom::Ready_Player()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Custom::Ready_UI()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_CUSTOM, TEXT("Layer_Custom"), TEXT("Prototype_GameObject_Custom"))))
	{
		return E_FAIL;
	}

	return S_OK;
}



HRESULT CLevel_Custom::Ready_Light()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(1.f, 2.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _vec4(0.2f);

	for (size_t i = 0; i < 1; i++)
	{
		if (FAILED(m_pGameInstance->Add_Light(LEVEL_CUSTOM, TEXT("Light_Custom") + to_wstring(i), LightDesc)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

CLevel_Custom* CLevel_Custom::Create(_dev pDevice, _context pContext)
{
	CLevel_Custom* pInstance = new CLevel_Custom(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to create : CLevel_Custom");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Custom::Free()
{
	__super::Free();
}
