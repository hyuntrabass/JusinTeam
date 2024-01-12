#include "BodyPart.h"

CBodyPart::CBodyPart(_dev pDevice, _context pContext)
	: CPartObject(pDevice, pContext)
{
}

CBodyPart::CBodyPart(const CBodyPart& rhs)
	: CPartObject(rhs)
{
}

HRESULT CBodyPart::Init_Prototype()
{
	return S_OK;
}

HRESULT CBodyPart::Init(void* pArg)
{
	BODYPART_DESC Desc = *reinterpret_cast<BODYPART_DESC*>(pArg);

	m_eType = Desc.eType;
	m_iNumVariations = Desc.iNumVariations;
	m_Models.resize(m_iNumVariations, nullptr);
	m_Animation = Desc.Animation;

	if (FAILED(__super::Init(Desc.pParentTransform)))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CBodyPart::Tick(_float fTimeDelta)
{


	if (m_iSelectedModelIndex > m_iNumVariations)
	{
		return;
	}


	if (m_pGameInstance->Get_CurrentLevelIndex() == 0/*캐릭 생성시*/)
	{
		for (size_t i = 0; i < m_iNumVariations; i++)
		{
			m_Models[i]->Set_Animation(*m_Animation);
			m_Models[i]->Play_Animation(fTimeDelta);
		}
	}
	else
	{
		m_Models[m_iSelectedModelIndex]->Set_Animation(*m_Animation);
		m_Models[m_iSelectedModelIndex]->Play_Animation(fTimeDelta);
	}
}

void CBodyPart::Late_Tick(_float fTimeDelta)
{
	if (m_iSelectedModelIndex > m_iNumVariations)
	{
		return;
	}

	m_Worldmatrix = m_pParentTransform->Get_World_Matrix();

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);
}

HRESULT CBodyPart::Render()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}

	if (m_iSelectedModelIndex > m_iNumVariations)
	{
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_Models[m_iSelectedModelIndex]->Get_NumMeshes(); i++)
	{
		if (FAILED(m_Models[m_iSelectedModelIndex]->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
		}

		_bool HasNorTex{};
		if (FAILED(m_Models[m_iSelectedModelIndex]->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
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

		if (FAILED(m_Models[m_iSelectedModelIndex]->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
		{
			return E_FAIL;
		}

		//if (FAILED(m_pShaderCom->Begin(AnimPass_OutLine)))
		//{
		//	return E_FAIL;
		//}

		//if (FAILED(m_Models[m_iSelectedModelIndex]->Render(i)))
		//{
		//	return E_FAIL;
		//}

		if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
		{
			return E_FAIL;
		}

		if (FAILED(m_Models[m_iSelectedModelIndex]->Render(i)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CBodyPart::Render_Shadow()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_Worldmatrix)))
	{
		return E_FAIL;
	}

	LIGHT_DESC* Light = m_pGameInstance->Get_LightDesc(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Light_Main"));
	XMStoreFloat4(&Light->vPosition, m_pParentTransform->Get_State(State::Pos));

	if (FAILED(m_pGameInstance->Bind_Light_ViewProjMatrix(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Light_Main"), m_pShaderCom, "g_ViewMatrix", "g_ProjMatrix")))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_Models[m_iSelectedModelIndex]->Get_NumMeshes(); i++)
	{
		if (FAILED(m_Models[m_iSelectedModelIndex]->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(AnimPass_Shadow)))
		{
			return E_FAIL;
		}

		if (FAILED(m_Models[m_iSelectedModelIndex]->Render(i)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

_bool CBodyPart::IsAnimationFinished(_uint iAnimIndex)
{
	return m_Models[m_iSelectedModelIndex]->IsAnimationFinished(iAnimIndex);
}

_uint CBodyPart::Get_CurrentAnimationIndex()
{
	return m_Models[m_iSelectedModelIndex]->Get_CurrentAnimationIndex();
}

_float CBodyPart::Get_CurrentAnimPos()
{
	return m_Models[m_iSelectedModelIndex]->Get_CurrentAnimPos();
}

const _float44* CBodyPart::Get_BoneMatrix(const _char* pBoneName)
{
	return m_Models[m_iSelectedModelIndex]->Get_BoneMatrix(pBoneName);
}

//void CBodyPart::Set_ModelIndex(_uint iIndex)
//{
//	if (iIndex > m_iNumVariations)
//	{
//		m_iSelectedModelIndex = m_iNumVariations - 1;
//	}
//	else
//	{
//		m_iSelectedModelIndex = iIndex;
//	}
//}

void CBodyPart::Reset_Model()
{
	m_Animation->bRestartAnimation = true;
	m_Models[m_iSelectedModelIndex]->Set_Animation(*m_Animation);
	m_Animation->bRestartAnimation = false;
}

HRESULT CBodyPart::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	for (size_t i = 0; i < m_Models.size(); i++)
	{
		wstring PrototypeTag = TEXT("Prototype_Model_") + to_wstring(m_eType) + L"" + to_wstring(i);
		wstring ComTag = TEXT("Com_Model_") + to_wstring(i);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, PrototypeTag, ComTag, reinterpret_cast<CComponent**>(&m_Models[i]))))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CBodyPart::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_Worldmatrix)))
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

CBodyPart* CBodyPart::Create(_dev pDevice, _context pContext)
{
	CBodyPart* pInstance = new CBodyPart(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBodyPart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBodyPart::Clone(void* pArg)
{
	CBodyPart* pInstance = new CBodyPart(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBodyPart");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBodyPart::Free()
{
	__super::Free();

	for (auto& pModel : m_Models)
	{
		Safe_Release(pModel);
	}
	m_Models.clear();



	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
