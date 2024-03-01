#include "BlackCat.h"

#include "Effect_Manager.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"

CBlackCat::CBlackCat(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBlackCat::CBlackCat(const CBlackCat& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlackCat::Init_Prototype()
{
	return S_OK;
}

HRESULT CBlackCat::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_State(State::Pos, _vec4(75.f, 0.f, 110.f, 1.f));
	m_Animation.fAnimSpeedRatio = 2.f;
	m_Animation.bSkipInterpolation = false;
	m_Animation.isLoop = true;

	m_pTransformCom->Set_Scale(_vec3(2.f, 2.f, 2.f));

	m_eCurState = STATE_IDLE;

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Pet_Cat_Parti");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info, true);

	/*
	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Pet_Cat_Light");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info, true);
	*/
	m_pColliderCom->Set_Normal();
	return S_OK;
}

void CBlackCat::Tick(_float fTimeDelta)
{
	//m_pTransformCom->Set_OldMatrix();
	__super::Tick(fTimeDelta);
	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);
	Update_Collider();
	m_pModelCom->Set_Animation(m_Animation);

	m_EffectMatrix = *m_pModelCom->Get_BoneMatrix("Bip001-Spine") * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix();
}

void CBlackCat::Late_Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CBlackCat::Render()
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

		_bool HasMaskTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", i, TextureType::Shininess)))
		{
			HasMaskTex = false;
		}
		else
		{
			HasMaskTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
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

void CBlackCat::Init_State(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_L))
	{
		m_eCurState = STATE_EMOTION;
	}
	//m_eCurState = STATE_IDLE;
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CBlackCat::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;
			break;
		case Client::CBlackCat::STATE_CHASE:
			m_Animation.iAnimIndex = RUN;
			m_Animation.isLoop = true;
			m_fIdleTime = 0.f;
			break;
		case Client::CBlackCat::STATE_HIT:
			m_Animation.iAnimIndex = EMOTION;
			m_Animation.isLoop = false;
			m_fIdleTime = 0.f;
			break;
		case Client::CBlackCat::STATE_EMOTION:
		{
			_uint iRandom = rand() % 3;
			switch (iRandom)
			{
			case 0:
				m_Animation.iAnimIndex = EMOTION;
				break;

			case 1:
				m_Animation.iAnimIndex = TELEPORT_END;
				break;

			case 2:
				m_Animation.iAnimIndex = TELEPORT_START;
				break;
			}
		}

			m_Animation.isLoop = false;

			m_fIdleTime = 0.f;

		break;
		}

		m_ePreState = m_eCurState;
	}
}

void CBlackCat::Tick_State(_float fTimeDelta)
{

	m_pTransformCom->Set_State(State::Pos, _vec4(-2000.f, -1.f, -1999.f, 1.f));
	_vec3 vNormal = _vec3(0.f, 0.f, -1.f);
	switch (m_eCurState)
	{
	case CBlackCat::STATE_IDLE:
	{
		m_pTransformCom->LookAt_Dir(vNormal);
		CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_VILLAGE, TEXT("Layer_BrickGame"), TEXT("BrickBall"));
		if (pCollider == nullptr)
		{
			return;
		}
		_bool isColl = m_pColliderCom->Intersect(pCollider);
		if (isColl)
		{
			CCamera_Manager::Get_Instance()->Set_ShakeCam(true, 2.0f);
			m_eCurState = STATE_HIT;
		}
	}
		break;

	case CBlackCat::STATE_CHASE:
	{
		m_pTransformCom->LookAt_Dir(vNormal * -1.f);
	}
		break;
	case CBlackCat::STATE_HIT:
	{
		if (m_pModelCom->IsAnimationFinished(EMOTION))
		{
			m_eCurState = STATE_IDLE;
		}
	}
		break;

	case CBlackCat::STATE_EMOTION:
	{
		if (m_pModelCom->IsAnimationFinished(EMOTION) || m_pModelCom->IsAnimationFinished(TELEPORT_END) || m_pModelCom->IsAnimationFinished(TELEPORT_START))
		{
			
			m_eCurState = STATE_IDLE;
			
		}
	}
	break;
	}


}

HRESULT CBlackCat::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_BlackCat"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBlackCat::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_OldWorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBlackCat::Add_Collider()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::AABB;
	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(1.2f, 1.2f, 1.2f);
	CollDesc.vCenter = _vec3(0.f, 1.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Bar"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

void CBlackCat::Update_Collider()
{
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}


CBlackCat* CBlackCat::Create(_dev pDevice, _context pContext)
{
	CBlackCat* pInstance = new CBlackCat(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBlackCat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlackCat::Clone(void* pArg)
{
	CBlackCat* pInstance = new CBlackCat(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBlackCat");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlackCat::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pShaderCom);
}
