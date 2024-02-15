#include "Meteor.h"

#include "Effect_Dummy.h"
#include "Effect_Manager.h"

CMeteor::CMeteor(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CMeteor::CMeteor(const CMeteor& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMeteor::Init_Prototype()
{
    return S_OK;
}

HRESULT CMeteor::Init(void* pArg)
{
    return S_OK;
}

void CMeteor::Tick(_float fTimeDelta)
{
}

void CMeteor::Late_Tick(_float fTimeDelta)
{
}

HRESULT CMeteor::Render()
{
    return S_OK;
}

HRESULT CMeteor::Add_Collider()
{
    return S_OK;
}

void CMeteor::Update_Collider()
{
}

HRESULT CMeteor::Add_Components()
{
    return S_OK;
}

HRESULT CMeteor::Bind_ShaderResources()
{
    return S_OK;
}

CMeteor* CMeteor::Create(_dev pDevice, _context pContext)
{
	CMeteor* pInstance = new CMeteor(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CMeteor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMeteor::Clone(void* pArg)
{
	CMeteor* pInstance = new CMeteor(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CMeteor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeteor::Free()
{
	__super::Free();
}
