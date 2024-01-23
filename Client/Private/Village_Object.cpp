#pragma once
#include "Village_Object.h"

CVillage_Object::CVillage_Object(_dev pDevice, _context pContext)
	: CObjects(pDevice, pContext)
{
}

CVillage_Object::CVillage_Object(const CVillage_Object& rhs)
	: CObjects(rhs)
	,m_Info(rhs.m_Info)
{
}


HRESULT CVillage_Object::Init_Prototype()
{
	return S_OK;
}

HRESULT CVillage_Object::Init(void* pArg)
{
	m_Info = *(ObjectInfo*)pArg;
	wstring strPrototype = m_Info.strPrototypeTag;

	if (FAILED(__super::Add_Components(strPrototype, m_Info.eObjectType)))
	{
		return E_FAIL;
	}

	m_iShaderPass = 0;

	_vec4 vRight = _vec4(m_Info.m_WorldMatrix._11, m_Info.m_WorldMatrix._12, m_Info.m_WorldMatrix._13, m_Info.m_WorldMatrix._14);
	_vec4 vUp = _vec4(m_Info.m_WorldMatrix._21, m_Info.m_WorldMatrix._22, m_Info.m_WorldMatrix._23, m_Info.m_WorldMatrix._24);
	_vec4 vLook = _vec4(m_Info.m_WorldMatrix._31, m_Info.m_WorldMatrix._32, m_Info.m_WorldMatrix._33, m_Info.m_WorldMatrix._34);
	_vec4 vPos = _vec4(m_Info.m_WorldMatrix._41, m_Info.m_WorldMatrix._42, m_Info.m_WorldMatrix._43, m_Info.m_WorldMatrix._44);

	m_pTransformCom->Set_State(State::Right, vRight);
	m_pTransformCom->Set_State(State::Up, vUp);
	m_pTransformCom->Set_State(State::Look, vLook);
	m_pTransformCom->Set_State(State::Pos, vPos);
	return S_OK;
}

void CVillage_Object::Tick(_float fTimeDelta)
{

}

void CVillage_Object::Late_Tick(_float fTimeDelta)
{

	__super::Late_Tick(fTimeDelta);
}

HRESULT CVillage_Object::Render()
{

	__super::Render();

	return S_OK;
}


CVillage_Object* CVillage_Object::Create(_dev pDevice, _context pContext)
{
	CVillage_Object* pInstance = new CVillage_Object(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CVillage_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVillage_Object::Clone(void* pArg)
{
	CVillage_Object* pInstance = new CVillage_Object(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVillage_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVillage_Object::Free()
{
	__super::Free();
}
