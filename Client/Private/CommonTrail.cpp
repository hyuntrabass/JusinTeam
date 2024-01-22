#include "CommonTrail.h"

CCommonTrail::CCommonTrail(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CCommonTrail::CCommonTrail(const CCommonTrail& rhs)
	: CBlendObject(rhs)
{
}

void CCommonTrail::On()
{
	m_bNoRender = false;
}

void CCommonTrail::Off()
{
	m_bNoRender = true;
}

HRESULT CCommonTrail::Init_Prototype()
{
	return S_OK;
}

HRESULT CCommonTrail::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (not pArg)
	{
		MSG_BOX("No Argument!");
		return E_FAIL;
	}

	m_Info = *reinterpret_cast<TRAIL_DESC*>(pArg);

	if (m_Info.iNumVertices > 50)
	{
		MSG_BOX("���ؽ� ������ 50�� �ʰ��� �� �����ϴ�.");
	}

	m_PosArray = new _float3[m_Info.iNumVertices];
	m_ColorArray = new _float4[m_Info.iNumVertices];
	m_PSizeArray = new _float2[m_Info.iNumVertices];

	return S_OK;
}

void CCommonTrail::Tick(_float3 vPos)
{
	if (m_TrailPosList.size() >= m_Info.iNumVertices)
	{
		m_TrailPosList.pop_back();
	}
	m_TrailPosList.push_front(vPos);

	m_pTransformCom->Set_State(State::Pos, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
}

void CCommonTrail::Late_Tick(_float fTimeDelta)
{
	if (m_bNoRender)
	{
		return;
	}
	for (size_t i = 0; i < m_Info.iNumVertices; i++)
	{
		XMStoreFloat3(&m_PosArray[i], m_pTransformCom->Get_State(State::Pos));
		m_ColorArray[i] = _float4(m_Info.vColor.x, m_Info.vColor.y, m_Info.vColor.z, 1.f - static_cast<_float>(i) / m_Info.iNumVertices);
	}

	_uint iIndex{};
	for (auto& vPos : m_TrailPosList)
	{
		m_PosArray[iIndex++] = vPos;
	}

	for (size_t i = 0; i < m_Info.iNumVertices; i++)
	{
		m_PSizeArray[i] = m_Info.vPSize;
	}

	m_pTrailBufferCom->Update(m_Info.iNumVertices, m_PosArray, m_ColorArray, m_PSizeArray);

	__super::Compute_CamDistance();
	m_pRendererCom->Add_RenderGroup(RG_Blend, this);
	m_pRendererCom->Add_RenderGroup(RG_BlendBlur, this);
}

HRESULT CCommonTrail::Render()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(0)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTrailBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCommonTrail::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail_50"), TEXT("Com_TrailBuffer"), reinterpret_cast<CComponent**>(&m_pTrailBufferCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Trail"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCommonTrail::Bind_ShaderResources()
{
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

	return S_OK;
}

CCommonTrail* CCommonTrail::Create(_dev pDevice, _context pContext)
{
	CCommonTrail* pInstnace = new CCommonTrail(pDevice, pContext);

	if (FAILED(pInstnace->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCommonTrail");
		Safe_Release(pInstnace);
	}

	return pInstnace;
}

CGameObject* CCommonTrail::Clone(void* pArg)
{
	CCommonTrail* pInstance = new CCommonTrail(*this);

	if (FAILED(pInstance->Init((pArg))))
	{
		MSG_BOX("Failed to Clone : CCommonTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCommonTrail::Free()
{
	__super::Free();

	Safe_Delete_Array(m_PosArray);
	Safe_Delete_Array(m_ColorArray);
	Safe_Delete_Array(m_PSizeArray);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTrailBufferCom);
	Safe_Release(m_pShaderCom);
}
