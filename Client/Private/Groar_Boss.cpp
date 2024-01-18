#include "Groar_Boss.h"

CGroar_Boss::CGroar_Boss(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CGroar_Boss::CGroar_Boss(const CGroar_Boss& rhs)
	: CGameObject(rhs)
{
}

HRESULT CGroar_Boss::Init_Prototype()
{
    return S_OK;
}

HRESULT CGroar_Boss::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_State(State::Pos, _vec4(static_cast<_float>(rand() % 20), 0.f, static_cast<_float>(rand() % 20), 1.f));
	m_pTransformCom->Set_State(State::Pos, _vec4(10.f, 0.f, 0.f, 1.f));

	m_Animation.iAnimIndex = NPC_IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 1.5f;

	m_eCurState = STATE_NPC_IDLE;

	m_iHP = 100;

    return S_OK;
}

void CGroar_Boss::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_G))
	{
		m_eCurState = STATE_SCENE01;
	}

	if (m_eCurState == STATE_SCENE01)
	{
		m_Animation.iAnimIndex = 0;
		m_Animation.isLoop = false;

		if (m_pScene01ModelCom->IsAnimationFinished(0))
		{
			m_eCurState = STATE_SCENE02;
		}
	}

	if (m_eCurState == STATE_SCENE02)
	{
		m_Animation.iAnimIndex = 0;
		m_Animation.isLoop = false;

		//m_pTransformCom->Go_Up(fTimeDelta);

		if (m_pScene02ModelCom->IsAnimationFinished(0))
		{
			m_bBossState = true;
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
		}

	}

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	if (m_eCurState == STATE_NPC_IDLE || m_eCurState == STATE_NPC_TALK)
	{
		m_pNPCModelCom->Set_Animation(m_Animation);
	}

	if (m_eCurState == STATE_SCENE01)
	{
		m_pScene01ModelCom->Set_Animation(m_Animation);
	}

	if (m_eCurState == STATE_SCENE02 && !m_bBossState)
	{
		m_pScene02ModelCom->Set_Animation(m_Animation);
	}

	if (m_bBossState == true)
	{
		m_pBossModelCom->Set_Animation(m_Animation);
	}

}

void CGroar_Boss::Late_Tick(_float fTimeDelta)
{
	if (m_eCurState == STATE_NPC_IDLE || m_eCurState == STATE_NPC_TALK)
	{
		m_pNPCModelCom->Play_Animation(fTimeDelta);
	}

	if (m_eCurState == STATE_SCENE01)
	{
		m_pScene01ModelCom->Play_Animation(fTimeDelta);
	}

	if (m_eCurState == STATE_SCENE02)
	{
		m_pScene02ModelCom->Play_Animation(fTimeDelta);
	}

	if (m_bBossState == true)
	{
		m_pBossModelCom->Play_Animation(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CGroar_Boss::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (m_eCurState == STATE_NPC_IDLE || m_eCurState == STATE_NPC_TALK)
	{
		for (_uint i = 0; i < m_pNPCModelCom->Get_NumMeshes(); i++)
		{
			if (FAILED(m_pNPCModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
			{
			}

			_bool HasNorTex{};
			if (FAILED(m_pNPCModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
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

			if (FAILED(m_pNPCModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pNPCModelCom->Render(i)))
			{
				return E_FAIL;
			}
		}

	}

	if (m_eCurState == STATE_SCENE01)
	{
		for (_uint i = 0; i < m_pScene01ModelCom->Get_NumMeshes(); i++)
		{
			if (FAILED(m_pScene01ModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
			{
			}

			_bool HasNorTex{};
			if (FAILED(m_pScene01ModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
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

			if (FAILED(m_pScene01ModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pScene01ModelCom->Render(i)))
			{
				return E_FAIL;
			}
		}

	}

	if (m_eCurState == STATE_SCENE02)
	{
		for (_uint i = 0; i < m_pScene02ModelCom->Get_NumMeshes(); i++)
		{
			if (FAILED(m_pScene02ModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
			{
			}

			_bool HasNorTex{};
			if (FAILED(m_pScene02ModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
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

			if (FAILED(m_pScene02ModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pScene02ModelCom->Render(i)))
			{
				return E_FAIL;
			}
		}

	}

	if (m_bBossState == true)
	{
		for (_uint i = 0; i < m_pBossModelCom->Get_NumMeshes(); i++)
		{
			if (FAILED(m_pBossModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
			{
			}

			_bool HasNorTex{};
			if (FAILED(m_pBossModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
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

			if (FAILED(m_pBossModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pBossModelCom->Render(i)))
			{
				return E_FAIL;
			}
		}

	}

	return S_OK;
}

void CGroar_Boss::Init_State(_float fTimeDelta)
{
}

void CGroar_Boss::Tick_State(_float fTimeDelta)
{
}

HRESULT CGroar_Boss::Add_Collider()
{
    return S_OK;
}

void CGroar_Boss::Update_Collider()
{
}

HRESULT CGroar_Boss::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Groar"), TEXT("Com_NPC_Model"), reinterpret_cast<CComponent**>(&m_pNPCModelCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_GroarScene01"), TEXT("Com_Scene01_Model"), reinterpret_cast<CComponent**>(&m_pScene01ModelCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_GroarScene02"), TEXT("Com_Scene02_Model"), reinterpret_cast<CComponent**>(&m_pScene02ModelCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_Groar_Boss"), TEXT("Com_Boss_Model"), reinterpret_cast<CComponent**>(&m_pBossModelCom))))
	{
		return E_FAIL;
	}

    return S_OK;
}

HRESULT CGroar_Boss::Bind_ShaderResources()
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

CGroar_Boss* CGroar_Boss::Create(_dev pDevice, _context pContext)
{
	CGroar_Boss* pInstance = new CGroar_Boss(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CGroar_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGroar_Boss::Clone(void* pArg)
{
	CGroar_Boss* pInstance = new CGroar_Boss(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CGroar_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGroar_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pNPCModelCom);
	Safe_Release(m_pScene01ModelCom);
	Safe_Release(m_pScene02ModelCom);
	Safe_Release(m_pBossModelCom);

}
