#include "Lever.h"

CLever::CLever(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CLever::CLever(const CLever& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLever::Init_Prototype()
{
	return S_OK;
}

HRESULT CLever::Init(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CLever::Tick(_float fTimeDelta)
{
}

void CLever::Late_Tick(_float fTimeDelta)
{
}

HRESULT CLever::Render()
{
	return S_OK;
}

void CLever::Set_Damage(_int iDamage, _uint iDamageType)
{
}

HRESULT CLever::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	//if(FAILED(__super::Add_Component()))
	// 모델 추가해라

	return S_OK;
}

HRESULT CLever::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CLever::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::Sphere;
	BodyCollDesc.fRadius = 1.f;
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.fRadius, 0.f);



	return S_OK;
}

HRESULT CLever::Add_Attack_Collider()
{
	return S_OK;
}

void CLever::Update_Collider()
{
}

CLever* CLever::Create(_dev pDevice, _context pContext)
{
	CLever* pInstance = new CLever(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CLever");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLever::Clone(void* pArg)
{
	CLever* pInstance = new CLever(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CLever");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLever::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}