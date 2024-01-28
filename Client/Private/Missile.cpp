#include "Missile.h"

CMissile::CMissile(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CMissile::CMissile(const CMissile& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMissile::Init_Prototype()
{
    return S_OK;
}

HRESULT CMissile::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

    return S_OK;
}

void CMissile::Tick(_float fTimeDelta)
{
}

void CMissile::Late_Tick(_float fTimeDelta)
{
}

HRESULT CMissile::Render()
{
    return S_OK;
}

HRESULT CMissile::Add_Collider()
{
    return S_OK;
}

void CMissile::Update_Collider()
{
}

HRESULT CMissile::Add_Components()
{
    return S_OK;
}

HRESULT CMissile::Bind_ShaderResources()
{
    return S_OK;
}

CMissile* CMissile::Create(_dev pDevice, _context pContext)
{
	CMissile* pInstance = new CMissile(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMissile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMissile::Clone(void* pArg)
{
	CMissile* pInstance = new CMissile(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CMissile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMissile::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
