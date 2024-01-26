#include "Trigger_Manager.h"
#include "Trigger.h"

IMPLEMENT_SINGLETON(CTrigger_Manager)

// TODO : 필요한 기능 작성하기

CTrigger_Manager::CTrigger_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}
HRESULT CTrigger_Manager::Initialize_Prototype(const GRAPHIC_DESC& GraphicDesc)
{
	m_hWnd = GraphicDesc.hWnd;
	m_iWinSizeX = GraphicDesc.iWinSizeX;
	m_iWinSizeY = GraphicDesc.iWinSizeY;

	return S_OK;
}

void CTrigger_Manager::Late_Tick(_float fTimeDelta)
{


}

CTrigger_Manager* CTrigger_Manager::Create(const GRAPHIC_DESC& GraphicDesc)
{
	CTrigger_Manager* pInstance = new CTrigger_Manager();

	if (FAILED(pInstance->Initialize_Prototype(GraphicDesc)))
	{
		MSG_BOX("Failed to Created : CImGui_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrigger_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}
