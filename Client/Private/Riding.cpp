#include "Riding.h"

CRiding::CRiding(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CRiding::CRiding(const CRiding& rhs)
	: CGameObject(rhs)
{
}

HRESULT CRiding::Init_Prototype()
{
	return S_OK;
}

HRESULT CRiding::Init(void* pArg)
{
	Riding_Desc* Desc = (Riding_Desc*)pArg;
	m_CurrentIndex = Desc->Type;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_State(State::Pos, _vec4(static_cast<_float>(rand() % 20), 0.f, static_cast<_float>(rand() % 20), 1.f));
	m_pTransformCom->Set_State(State::Pos, _vec4(10.f, 0.f, 0.f, 1.f));


	m_Animation.iAnimIndex = 10;
	m_iNumVariations = 3;
	m_vecModel.resize(m_iNumVariations, nullptr);
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 2.f;

	m_iHP = 100;

	return S_OK;
}

void CRiding::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_G))
	{

	}

	Update_Collider();
}

void CRiding::Late_Tick(_float fTimeDelta)
{
	

	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

#ifdef _DEBUGTEST
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif

}

HRESULT CRiding::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}


		for (_uint i = 0; i < m_vecModel[m_CurrentIndex]->Get_NumMeshes(); i++)
		{
			if (FAILED(m_vecModel[m_CurrentIndex]->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
			{
			}

			_bool HasNorTex{};
			if (FAILED(m_vecModel[m_CurrentIndex]->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
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

			if (FAILED(m_vecModel[m_CurrentIndex]->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
			{
				return E_FAIL;
			}

			if (FAILED(m_vecModel[m_CurrentIndex]->Render(i)))
			{
				return E_FAIL;
			}
		}
	
	return S_OK;
}


void CRiding::Update_Collider()
{
	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());

	_mat Offset = _mat::CreateTranslation(0.f, 2.f, 1.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
}

void CRiding::Delete_Riding()
{
	//죽는 처리 후 m_bReady_Dead = true;
	
}

HRESULT CRiding::Add_Components()
{
	for (size_t i = 0; i < m_vecModel.size(); i++)
	{
		wstring PrototypeTag = TEXT("Prototype_Model_Riding")+ to_wstring(i);
		wstring ComTag = TEXT("Com_Model_") + to_wstring(i);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, PrototypeTag, ComTag, reinterpret_cast<CComponent**>(&m_vecModel[i]))))
		{
			return E_FAIL;
		}
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(2.f, 2.f, 2.f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
		return E_FAIL;

	// Frustum
	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Frustum;
	_matrix matView = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	// 1인자 : 절두체 각도(범위), 2인자 : Aspect, 3인자 : Near, 4인자 : Far(절두체 깊이)
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), 1.f, 0.01f, 3.f);
	XMStoreFloat4x4(&ColDesc.matFrustum, matView * matProj);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColDesc)))
	{
		return E_FAIL;
	}


	return S_OK;

	return S_OK;
}

HRESULT CRiding::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraNF().y, sizeof _float)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
	{
		return E_FAIL;
	}

	return S_OK;
}

CRiding* CRiding::Create(_dev pDevice, _context pContext)
{
	CRiding* pInstance = new CRiding(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CRiding");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRiding::Clone(void* pArg)
{
	CRiding* pInstance = new CRiding(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CRiding");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRiding::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	for (auto& iter : m_vecModel)
	{
		Safe_Release(iter);
	}

	m_vecModel.clear();
	Safe_Release(m_pBodyColliderCom);
	Safe_Release(m_pAttackColliderCom);

}
