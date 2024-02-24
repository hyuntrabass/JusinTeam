#include "Launcher.h"

CLauncher::CLauncher(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CLauncher::CLauncher(const CLauncher& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLauncher::Init_Prototype()
{
	return S_OK;
}

HRESULT CLauncher::Init(void* pArg)
{
	return S_OK;
}

void CLauncher::Tick(_float fTimeDelta)
{
}

void CLauncher::Late_Tick(_float fTimeDelta)
{
}

HRESULT CLauncher::Render()
{
	return S_OK;
}

CLauncher* CLauncher::Create(_dev pDevice, _context pContext)
{
	CLauncher* pInstance = new CLauncher(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CLauncher");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLauncher::Clone(void* pArg)
{
	CLauncher* pInstance = new CLauncher(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CLauncher");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLauncher::Free()
{
	__super::Free();
}
