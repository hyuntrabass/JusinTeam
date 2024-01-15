#include "Level_GamePlay.h"
#include "Camera.h"

CLevel_GamePlay::CLevel_GamePlay(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_GamePlay::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_GAMEPLAY);

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

	// Monster
	if (FAILED(Ready_Void05()))
	{
		MSG_BOX("Failed to Ready Void05");
		return E_FAIL;
	}

	if (FAILED(Ready_Rabbit()))
	{
		MSG_BOX("Failed to Ready Rabbit");
		return E_FAIL;
	}

	if (FAILED(Ready_Goat()))
	{
		MSG_BOX("Failed to Ready Goat");
		return E_FAIL;
	}

	if (FAILED(Ready_Nastron03()))
	{
		MSG_BOX("Failed to Ready Nastron03");
		return E_FAIL;
	}

	if (FAILED(Ready_NPCvsMon()))
	{
		MSG_BOX("Failed to Ready NPCvsMon");
		return E_FAIL;
	}

	if (FAILED(Ready_Thief04()))
	{
		MSG_BOX("Failed to Ready Thief04");
		return E_FAIL;
	}

	if (FAILED(Ready_Map()))
	{
		MSG_BOX("Failed to Ready Map");
		return E_FAIL;
	}

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
	{
		DestroyWindow(g_hWnd);
	}
}

HRESULT CLevel_GamePlay::Render()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Camera()
{
	if (not m_pGameInstance)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Light()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(-1.f, -2.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);

	return m_pGameInstance->Add_Light(LEVEL_GAMEPLAY, TEXT("Light_Main"), LightDesc);
}

HRESULT CLevel_GamePlay::Ready_Player()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Prototype_GameObject_Player"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Map()
{

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Void05()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Void05"), TEXT("Prototype_GameObject_Void05"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Rabbit()
{
	for (size_t i = 0; i < 5; i++)
	{
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Rabbit"), TEXT("Prototype_GameObject_Rabbit"))))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Goat()
{
	for (size_t i = 0; i < 5; i++)
	{
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Goat"), TEXT("Prototype_GameObject_Goat"))))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Nastron03()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Nastron03"), TEXT("Prototype_GameObject_Nastron03"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_NPCvsMon()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_NPCvsMon"), TEXT("Prototype_GameObject_NPCvsMon"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Thief04()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Thief04"), TEXT("Prototype_GameObject_Thief04"))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(_dev pDevice, _context pContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to create : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}
