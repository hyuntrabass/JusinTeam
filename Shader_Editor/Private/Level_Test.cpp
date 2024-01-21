#include "pch.h"
#include "Level_Test.h"

#include "Camera.h"

CLevel_Test::CLevel_Test(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Test::Init()
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_ShaderTest);

	if (FAILED(Ready_Camera()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Test::Tick(_float fTimeDelta)
{
}

HRESULT CLevel_Test::Render()
{
	return S_OK;
}

HRESULT CLevel_Test::Ready_Select()
{
	return E_NOTIMPL;
}

HRESULT CLevel_Test::Ready_Model()
{
	return E_NOTIMPL;
}

HRESULT CLevel_Test::Ready_Npc()
{
	return E_NOTIMPL;
}

HRESULT CLevel_Test::Ready_Map()
{
	return E_NOTIMPL;
}

HRESULT CLevel_Test::Ready_Camera()
{
	CCamera::Camera_Desc Desc;
	Desc.vCameraPos = _float4(0.f, 5.f, -5.f, 1.f);
	Desc.vFocusPos = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovY = XMConvertToRadians(60.f);
	Desc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.fFar = 500.f;

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, L"Layer_Camera", L"Prototype_GameObject_Camera_Main", &Desc)))
		return E_FAIL;

	m_pGameInstance->Set_CameraModeIndex(CM_MAIN);


	return S_OK;
}

HRESULT CLevel_Test::Ready_Light()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Point;
	LightDesc.vAttenuation = LIGHT_RANGE_50;
	LightDesc.vDiffuse = _float4(1.f, 0.6f, 0.1f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vPosition = _float4(0.134f, 0.5f, -3.2f, 1.f);
	LightDesc.vSpecular = _vec4(1.f);

	if (FAILED(m_pGameInstance->Add_Light(LEVEL_ShaderTest, L"Light_Select", LightDesc)))
		return E_FAIL;

	return S_OK;
}

CLevel_Test* CLevel_Test::Create(_dev pDevice, _context pContext)
{
	CLevel_Test* pInstance = new CLevel_Test(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to create : CLevel_Test");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Test::Free()
{
	__super::Free();
}
