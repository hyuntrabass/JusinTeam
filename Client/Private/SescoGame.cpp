#include "SescoGame.h"

CSescoGame::CSescoGame(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CSescoGame::Init()
{
	return S_OK;
}

void CSescoGame::Tick(_float fTimeDelta)
{
}

void CSescoGame::Late_Tick(_float fTimeDelta)
{
}

CSescoGame* CSescoGame::Create(_dev pDevice, _context pContext)
{
	CSescoGame* pInstance = new CSescoGame(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CSescoGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSescoGame::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
