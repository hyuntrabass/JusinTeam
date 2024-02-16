#pragma once
#include "Torch_Object.h"
#include "Camera_Manager.h"
#include "Effect_Manager.h"
#include "Effect_Dummy.h"

CTorch_Object::CTorch_Object(_dev pDevice, _context pContext)
	: CObjects(pDevice, pContext)
{
}

CTorch_Object::CTorch_Object(const CTorch_Object& rhs)
	: CObjects(rhs)
{
	m_isInstancing = true;
}


HRESULT CTorch_Object::Init_Prototype()
{
	return S_OK;
}

HRESULT CTorch_Object::Init(void* pArg)
{
	m_Info = *(ObjectInfo*)pArg;
	strPrototype = m_Info.strPrototypeTag;

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

	EffectInfo Effect = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"TorchFire_Dun");
	Effect.pMatrix =  & m_FireMat;
	Effect.isFollow = true;
	m_pFire = CEffect_Manager::Get_Instance()->Clone_Effect(Effect);

	if (strPrototype == L"Prototype_Model_Wall_Torch")
	{
		vFire_Hight = _vec3(0.f, 1.5f, 0.f);
	}
	else if (strPrototype == L"Prototype_Model_Stand_Torch1")
	{
		vFire_Hight = _vec3(0.f, 2.f, 0.f);
	}
	else if (strPrototype == L"Prototype_Model_Stand_Torch2")
	{
		vFire_Hight = _vec3(0.f, 1.f, 0.f);
	}
	else if (strPrototype == L"Prototype_Model_Torch")
	{
		vFire_Hight = _vec3(0.f, 2.f, 0.f);
	}
	else if (strPrototype == L"Prototype_Model_Brazier")
	{
		vFire_Hight = _vec3(0.f, 0.5f, 0.f);
	}

	return S_OK;
}

void CTorch_Object::Tick(_float fTimeDelta)
{
	m_FireMat = _mat::CreateTranslation(m_Info.m_WorldMatrix.Position_vec3());
	if (m_pFire)
	{
		m_pFire->Tick(fTimeDelta);
	}
}

void CTorch_Object::Late_Tick(_float fTimeDelta)
{
	CAMERA_STATE CamState = CCamera_Manager::Get_Instance()->Get_CameraState();
	if (CamState == CS_SKILLBOOK or CamState == CS_INVEN or CamState == CS_WORLDMAP)
	{
		return;
	}
	__super::Late_Tick(fTimeDelta);

	if (m_pFire)
	{
		m_pFire->Late_Tick(fTimeDelta);
	}
}

HRESULT CTorch_Object::Render()
{

	__super::Render();

	return S_OK;
}


CTorch_Object* CTorch_Object::Create(_dev pDevice, _context pContext)
{
	CTorch_Object* pInstance = new CTorch_Object(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTorch_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTorch_Object::Clone(void* pArg)
{
	CTorch_Object* pInstance = new CTorch_Object(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTorch_Object");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTorch_Object::Free()
{
	__super::Free();

	Safe_Release(m_pFire);
}
