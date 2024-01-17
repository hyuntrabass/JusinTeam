#pragma once
#include "Prologue_Object.h"

CPrologue_Object::CPrologue_Object(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CPrologue_Object::CPrologue_Object(const CPrologue_Object& rhs)
	: CBlendObject(rhs)
{
}


HRESULT CPrologue_Object::Init_Prototype()
{
	return S_OK;
}

HRESULT CPrologue_Object::Init(void* pArg)
{

	m_Info = *(ObjectInfo*)pArg;

	_vec4 vRight = _vec4(m_Info.m_WorldMatrix._11, m_Info.m_WorldMatrix._12, m_Info.m_WorldMatrix._13, m_Info.m_WorldMatrix._14);
	_vec4 vUp = _vec4(m_Info.m_WorldMatrix._21, m_Info.m_WorldMatrix._22, m_Info.m_WorldMatrix._23, m_Info.m_WorldMatrix._24);
	_vec4 vLook = _vec4(m_Info.m_WorldMatrix._31, m_Info.m_WorldMatrix._32, m_Info.m_WorldMatrix._33, m_Info.m_WorldMatrix._34);
	_vec4 vPos = _vec4(m_Info.m_WorldMatrix._41, m_Info.m_WorldMatrix._42, m_Info.m_WorldMatrix._43, m_Info.m_WorldMatrix._44);

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}


	m_iShaderPass = 0;

	m_pTransformCom->Set_State(State::Right, vRight);
	m_pTransformCom->Set_State(State::Up, vUp);
	m_pTransformCom->Set_State(State::Look, vLook);
	m_pTransformCom->Set_State(State::Pos, vPos);
	return S_OK;
}

void CPrologue_Object::Tick(_float fTimeDelta)
{

}

void CPrologue_Object::Late_Tick(_float fTimeDelta)
{

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);

}

HRESULT CPrologue_Object::Render()
{

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			return E_FAIL;
		}

		_bool HasNorTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
		{
			HasNorTex = false;
		}
		else
		{
			HasNorTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}


	}


	return S_OK;
}

HRESULT CPrologue_Object::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}
	m_iShaderPass = StaticPass_Default;
	m_iOutLineShaderPass = StaticPass_OutLine;


	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_Info.strPrototypeTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPrologue_Object::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraNF().y, sizeof _float)))
	{
		return E_FAIL;
	}

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

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
