#pragma once
#include "Effect_Object.h"
#include "Effect_Manager.h"
#include "Camera_Main.h"

CEffect_Object::CEffect_Object(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CEffect_Object::CEffect_Object(const CEffect_Object& rhs)
	: CBlendObject(rhs)
{
}


HRESULT CEffect_Object::Init_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Object::Init(void* pArg)
{
	m_Info = *(EffectObjectInfo*)pArg;
	wstring strEffectName = m_Info.strEffectName;
	_mat mMatrix = m_Info.m_WorldMatrix;
	_float fSize = m_Info.m_fSize;
	_bool isFollow = m_Info.m_isFollow;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	_mat* pEffectMatrix = &mMatrix;

	CEffect_Manager::Get_Instance()->Create_Effect(strEffectName, pEffectMatrix, isFollow);

	m_pTransformCom->Set_Matrix(m_Info.m_WorldMatrix);

	return S_OK;
}

void CEffect_Object::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CEffect_Object::Late_Tick(_float fTimeDelta)
{
	CCollider* pCameraCollider = dynamic_cast<CCollider*>(m_pGameInstance->Get_Component(LEVEL_STATIC, L"Layer_Camera", L"Com_Collider"));
	if (m_pColliderCom->Intersect(pCameraCollider))
	{
		__super::Late_Tick(fTimeDelta);
	}
}


HRESULT CEffect_Object::Render()
{

	return S_OK;
}

HRESULT CEffect_Object::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Collider"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}
	return S_OK;
}

CEffect_Object* CEffect_Object::Create(_dev pDevice, _context pContext)
{
	CEffect_Object* pInstance = new CEffect_Object(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Object::Clone(void* pArg)
{
	CEffect_Object* pInstance = new CEffect_Object(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Object::Free()
{
	__super::Free();
}
