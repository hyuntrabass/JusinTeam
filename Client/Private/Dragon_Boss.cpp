#include "Dragon_Boss.h"

CDragon_Boss::CDragon_Boss(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CDragon_Boss::CDragon_Boss(const CDragon_Boss& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDragon_Boss::Init_Prototype()
{
	return S_OK;
}

HRESULT CDragon_Boss::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, _vec4(10.f, 0.f, 0.f, 1.f));

	

	m_Animation.iAnimIndex = Dragon_Idle;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 1.5f;

	m_iHP = 100;

	return S_OK;
}

void CDragon_Boss::Tick(_float fTimeDelta)
{
	
	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	Update_Collider();
}

void CDragon_Boss::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

#ifdef _DEBUGTEST
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif

}

HRESULT CDragon_Boss::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

		for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
			{
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

			if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
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

void CDragon_Boss::Init_State(_float fTimeDelta)
{
	//if (m_ePreState != m_eState)
	//{
	//	switch (m_eState)
	//	{
	//	case Client::CDragon_Boss::STATE_NPC:
	//		break;
	//	case Client::CDragon_Boss::STATE_SCENE01:
	//		m_Animation.iAnimIndex = 0;
	//		m_Animation.isLoop = false;
	//		m_Animation.fAnimSpeedRatio = 1.5f;

	//		break;
	//	case Client::CDragon_Boss::STATE_SCENE02:
	//		m_Animation.iAnimIndex = 0;
	//		m_Animation.isLoop = false;
	//		m_Animation.fAnimSpeedRatio = 2.f;

	//		break;
	//	case Client::CDragon_Boss::STATE_BOSS:
	//		break;
	//	}

	//	m_ePreState = m_eState;
	//}

}

void CDragon_Boss::Tick_State(_float fTimeDelta)
{
	
}

HRESULT CDragon_Boss::Add_Collider()
{
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
}

void CDragon_Boss::Update_Collider()
{
	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());

	_mat Offset = _mat::CreateTranslation(0.f, 2.f, 1.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
}

HRESULT CDragon_Boss::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}



	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Dragon"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDragon_Boss::Bind_ShaderResources()
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

CDragon_Boss* CDragon_Boss::Create(_dev pDevice, _context pContext)
{
	CDragon_Boss* pInstance = new CDragon_Boss(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CDragon_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDragon_Boss::Clone(void* pArg)
{
	CDragon_Boss* pInstance = new CDragon_Boss(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CDragon_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDragon_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pModelCom);
	Safe_Release(m_pBodyColliderCom);
	Safe_Release(m_pAttackColliderCom);

}
