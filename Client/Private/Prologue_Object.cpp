#pragma once
#include "Prologue_Object.h"

CPrologue_Object::CPrologue_Object(_dev pDevice, _context pContext)
	: CObjects(pDevice, pContext)
{
}

CPrologue_Object::CPrologue_Object(const CPrologue_Object& rhs)
	: CObjects(rhs)
	,m_Info(rhs.m_Info)
{
}


HRESULT CPrologue_Object::Init_Prototype()
{
	return S_OK;
}

HRESULT CPrologue_Object::Init(void* pArg)
{
	m_Info = *(ObjectInfo*)pArg;
	wstring strPrototype = m_Info.strPrototypeTag;

	if (FAILED(__super::Add_Components(strPrototype, m_Info.eObjectType)))
	{
		return E_FAIL;
	}

	m_iShaderPass = 0;

	m_pTransformCom->Set_Matrix(m_Info.m_WorldMatrix);
	m_pModelCom->Apply_TransformToActor(m_Info.m_WorldMatrix);
	
	return S_OK;
}

void CPrologue_Object::Tick(_float fTimeDelta)
{

}

void CPrologue_Object::Late_Tick(_float fTimeDelta)
{

	__super::Late_Tick(fTimeDelta);
}

HRESULT CPrologue_Object::Render()
{

	__super::Render();

	return S_OK;
}


CPrologue_Object* CPrologue_Object::Create(_dev pDevice, _context pContext)
{
	CPrologue_Object* pInstance = new CPrologue_Object(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CPrologue_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPrologue_Object::Clone(void* pArg)
{
	CPrologue_Object* pInstance = new CPrologue_Object(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPrologue_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPrologue_Object::Free()
{
	__super::Free();
}
