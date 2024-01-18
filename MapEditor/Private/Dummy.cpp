#include "Dummy.h"


static _int iID = 1;

CDummy::CDummy(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)

{
}

CDummy::CDummy(const CDummy& rhs)
	: CBlendObject(rhs)
	//, m_pImGui_Manager(CImGui_Manager::Get_Instance())
{
	//Safe_AddRef(m_pImGui_Manager);
	m_iID = iID++;
}

void CDummy::Select(const _bool& isSelected)
{
	m_isSelected = isSelected;
}

void CDummy::Modify(_fvector vPos, _fvector vLook)
{
	m_pTransformCom->Set_State(State::Pos, vPos);
	m_pTransformCom->LookAt_Dir(vLook);
	XMStoreFloat4(&m_Info.vPos, vPos);
	XMStoreFloat4(&m_Info.vLook, vLook);
}

void CDummy::Get_State(_float4& vPos, _float4& vLook)
{
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(State::Pos));
	XMStoreFloat4(&vLook, m_pTransformCom->Get_State(State::Look));
}


HRESULT CDummy::Init_Prototype()
{
	return S_OK;
}

HRESULT CDummy::Init(void* pArg)
{

	m_Info = *(DummyInfo*)pArg;

	/*if (m_Info.Prototype == L"Prototype_Model_Barlog" ||
		m_Info.Prototype == L"Prototype_Model_Furgoat" ||
		m_Info.Prototype == L"Prototype_Model_GiantBoss" ||
		m_Info.Prototype == L"Prototype_Model_Nastron02" ||
		m_Info.Prototype == L"Prototype_Model_Nastron03" ||
		m_Info.Prototype == L"Prototype_Model_Orc02" ||
		m_Info.Prototype == L"Prototype_Model_Penguin" ||
		m_Info.Prototype == L"Prototype_Model_Rabbit" ||
		m_Info.Prototype == L"Prototype_Model_Thief04" ||
		m_Info.Prototype == L"Prototype_Model_Trilobite" ||
		m_Info.Prototype == L"Prototype_Model_TrilobiteA" ||
		m_Info.Prototype == L"Prototype_Model_Void13" ||
		m_Info.Prototype == L"Prototype_Model_VoidDragon" ||
		m_Info.Prototype == L"Prototype_Model_Void05")*/
	if(m_Info.eType == ItemType::Monster || m_Info.eType == ItemType::NPC)
	{
		m_isAnim = true;
		m_Animation.isLoop = true;
	}
	else
	{
		m_iShaderPass = StaticPass_AlphaTestMeshes;
	}
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (m_Info.ppDummy)
	{
		*m_Info.ppDummy = this;
		m_Info.ppDummy = nullptr;
	}

	m_pTransformCom->Set_State(State::Pos, XMLoadFloat4(&m_Info.vPos));
	m_pTransformCom->LookAt_Dir(XMLoadFloat4(&m_Info.vLook));

	return S_OK;
}

void CDummy::Tick(_float fTimeDelta)
{
	
	if (m_isAnim)
	{

		m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CDummy::Late_Tick(_float fTimeDelta)
{

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);
	
}

HRESULT CDummy::Render()
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

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bSelected", &m_isSelected, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (m_isAnim)
		{
			if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			{
				return E_FAIL;
			}
		}


		if (FAILED(m_pShaderCom->Begin(m_iOutLineShaderPass)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
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

HRESULT CDummy::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}
	if (m_isAnim)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		{
			return E_FAIL;
		}
		m_iShaderPass = AnimPass_Default;
		m_iOutLineShaderPass = AnimPass_OutLine;
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		{
			return E_FAIL;
		}
		m_iShaderPass = StaticPass_Default;
		m_iOutLineShaderPass = StaticPass_OutLine;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_Info.Prototype, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDummy::Bind_ShaderResources()
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


	if (FAILED(m_pShaderCom->Bind_RawValue("g_iID", &m_iID, sizeof(_int))))
	{
		return E_FAIL;
	}
	

	if (m_Info.eType == ItemType::Trigger)
	{
		_float4 vColor{ 0.3f, 0.8f, 0.3f, 0.5f };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof _float4)))
		{
			return E_FAIL;
		}

		const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, TEXT("Light_Main"));
		if (!pLightDesc)
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof _float4)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof _float4)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof _float4)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof _float4)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

CDummy* CDummy::Create(_dev pDevice, _context pContext)
{
	CDummy* pInstance = new CDummy(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CDummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy::Clone(void* pArg)
{
	CDummy* pInstance = new CDummy(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CDummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDummy::Free()
{
	__super::Free();

	//if(m_pImGui_Manager)
	//Safe_Release(m_pImGui_Manager);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
