#pragma once
#include "Environment_Object.h"
#include "Camera_Manager.h"
#include "Camera_Main.h"

CEnvironment_Object::CEnvironment_Object(_dev pDevice, _context pContext)
	: CObjects(pDevice, pContext)
{
}

CEnvironment_Object::CEnvironment_Object(const CEnvironment_Object& rhs)
	: CObjects(rhs)
	, m_pTrigger_Manager(CTrigger_Manager::Get_Instance())

{
	Safe_AddRef(m_pTrigger_Manager);
	m_isInstancing = true;
}


HRESULT CEnvironment_Object::Init_Prototype()
{
	return S_OK;
}

HRESULT CEnvironment_Object::Init(void* pArg)
{
	m_Info = *(ObjectInfo*)pArg;
	wstring strPrototype = m_Info.strPrototypeTag;
	m_ePlaceType = (PlaceType)m_Info.m_iIndex;

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

	m_pModelCom->Apply_TransformToActor(m_Info.m_WorldMatrix);

	return S_OK;
}

void CEnvironment_Object::Tick(_float fTimeDelta)
{
	//if (m_isRendered)
	//	return;

	__super::Tick(fTimeDelta);
}

void CEnvironment_Object::Late_Tick(_float fTimeDelta)
{
	//if (m_isRendered)
	//	return;

	CAMERA_STATE CamState = CCamera_Manager::Get_Instance()->Get_CameraState();
	if (CamState == CS_SKILLBOOK or CamState == CS_INVEN )//or CamState == CS_WORLDMAP)
	{
		return;
	}

	__super::Late_Tick(fTimeDelta);
	
	//m_isRendered = true;

}

HRESULT CEnvironment_Object::Render()
{
	__super::Render();
	//m_isRendered = false;
	return S_OK;
}

CEnvironment_Object* CEnvironment_Object::Create(_dev pDevice, _context pContext)
{
	CEnvironment_Object* pInstance = new CEnvironment_Object(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CEnvironment_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEnvironment_Object::Clone(void* pArg)
{
	CEnvironment_Object* pInstance = new CEnvironment_Object(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CEnvironment_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnvironment_Object::Free()
{
	__super::Free();
	Safe_Release(m_pTrigger_Manager);
}
