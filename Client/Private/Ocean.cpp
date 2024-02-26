#include "Ocean.h"

COcean::COcean(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

COcean::COcean(const COcean& rhs)
	: CGameObject(rhs)
{
}

HRESULT COcean::Init_Prototype()
{
	return S_OK;
}

HRESULT COcean::Init(void* pArg)
{
	return S_OK;
}

void COcean::Tick(_float fTimeDelta)
{
}

void COcean::Late_Tick(_float fTimeDelta)
{
}

HRESULT COcean::Render()
{
	return S_OK;
}

HRESULT COcean::Add_Component()
{
	return S_OK;
}

HRESULT COcean::Bind_ShaderResources()
{
	return S_OK;
}

COcean* COcean::Create(_dev pDevice, _context pContext)
{
	// 원형 객체 생성 -> Initialize_Prototype 을 호출한다
	COcean* pInstance = new COcean(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : COcean");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* COcean::Clone(void* pArg)
{
	// 실사용 객체를 생성한다. -> 복사 생성자 호출

	COcean* pInstance = new COcean(*this);
	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : COcean");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COcean::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
