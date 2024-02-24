#include "Void19.h"

CVoid19::CVoid19(_dev pDevice, _context pContext)
	:CVTFMonster(pDevice, pContext)
{
}

CVoid19::CVoid19(const CVoid19& rhs)
	:CVTFMonster(rhs)
{
}

HRESULT CVoid19::Init_Prototype()
{
	return S_OK;
}

HRESULT CVoid19::Init(void* pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVoid19::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CVoid19::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CVoid19::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CVoid19::Render_Instance()
{
	__super::Render_Instance();

	return S_OK;
}

CVoid19* CVoid19::Create(_dev pDevice, _context pContext)
{
	CVoid19* pInstance = new CVoid19(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CVoid19");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVoid19::Clone(void* pArg)
{
	CVoid19* pInstance = new CVoid19(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVoid19");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVoid19::Free()
{
	__super::Free();
}
