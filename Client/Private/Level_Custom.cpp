#include "Level_Custom.h"
#include "Level_Loading.h"

CLevel_Custom::CLevel_Custom(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Custom::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_LOGO);

	if (FAILED(Ready_Logo()))
	{
		MSG_BOX("Failed to Ready Logo");
		return E_FAIL;
	}
	return S_OK;
}

void CLevel_Custom::Tick(_float fTimeDelta)
{
	if (not m_hasBGMStarted and m_pGameInstance->Is_SoundManager_Ready())
	{
		m_pGameInstance->PlayBGM(TEXT("Buzz"), 0.2f);
		m_hasBGMStarted = true;
	}

	if (m_pGameInstance->Key_Down(DIK_RETURN))
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

HRESULT CLevel_Custom::Ready_Logo()
{
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_LOGO, TEXT("Layer_Logo"), TEXT("Prototype_GameObject_Background"))))
	{
		return E_FAIL;
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
