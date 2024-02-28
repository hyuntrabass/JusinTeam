#include "Balloon.h"
#include "UI_Manager.h"
#include "Event_Manager.h"
#include "Effect_Manager.h"
#include "Camera_Manager.h"
#include "GlowCube.h"

CBalloon::CBalloon(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBalloon::CBalloon(const CBalloon& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBalloon::Init_Prototype()
{
	return S_OK;
}

HRESULT CBalloon::Init(void* pArg)
{
	m_vColor = ((BALLOON_DESC*)pArg)->vColor;
	_vec3 vPos = ((BALLOON_DESC*)pArg)->vPosition;
	m_pTransformCom->Set_Position(vPos);

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}
	Set_RandomColor();
	//Prototype_GameObject_GlowCube

	CGlowCube::GLOWCUBE_DESC CubeDesc{};
	CubeDesc.vColor = m_vColor;
	CubeDesc.pParentTransform = m_pTransformCom;
	CubeDesc.vPos = _vec3(0.f, 0.5f, 0.f);
	m_pCube = (CGlowCube*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_GlowCube"), &CubeDesc);
	if (m_pCube == nullptr)
	{
		return E_FAIL;
	}

	m_eCurState = STATE_IDLE;
	m_pTransformCom->Set_Scale(_vec3(1.4f, 1.4f, 1.4f));
	
	return S_OK;
}

void CBalloon::Tick(_float fTimeDelta)
{
	m_fX += fTimeDelta;

	m_pBodyColliderCom->Change_Extents(_vec3(0.82f, 0.82f, 0.82f));
	m_pBodyColliderCom->Set_Normal();
	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);
	m_pCube->Tick(fTimeDelta);

	Update_BodyCollider();

	m_pTransformCom->Gravity(fTimeDelta);


}

void CBalloon::Late_Tick(_float fTimeDelta)
{
	m_pCube->Late_Tick(fTimeDelta);
	//m_shouldRenderBlur = true;
	m_pRendererCom->Add_RenderGroup(RG_Blend, this);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
#endif
}

HRESULT CBalloon::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}


	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			_bool bFailed = true;
		}

		if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof _vec4)))
		{
			return E_FAIL;
		}

		_float fAlpha = 1.f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &fAlpha, sizeof _float)))
		{
			return E_FAIL;
		}

		_bool isBlur = false;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_isBlur", &isBlur, sizeof _bool)))
		{
			return E_FAIL;
		}

		_vec2 vUV = _vec2(0.f, 0.f);
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &vUV, sizeof _vec2)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(StaticPass_DiffEffect)))
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

void CBalloon::Init_State(_float fTimeDelta)
{

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CBalloon::STATE_IDLE:

			m_bDamaged = false;
			break;

		case Client::CBalloon::STATE_HIT:

			break;

		case Client::CBalloon::STATE_DIE:

			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CBalloon::Tick_State(_float fTimeDelta)
{
	m_isColl = false;
	switch (m_eCurState)
	{
	case Client::CBalloon::STATE_IDLE:
	{

		CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_VILLAGE, TEXT("Layer_BrickBall"), TEXT("Com_Collider_Sphere"));
		if (pCollider == nullptr)
		{
			return;
		}

		m_isColl = m_pBodyColliderCom->Intersect(pCollider);
		if (m_isColl)
		{
			if (CUI_Manager::Get_Instance()->Get_BrickBallColor() == m_eCurColor)
			{
				CCamera_Manager::Get_Instance()->Set_ShakeCam(true, 2.0f);
				m_eCurState = STATE_HIT;
			}
		}
	}
	break;

	case Client::CBalloon::STATE_HIT:
	{
		_uint iColor = (_uint)m_eCurColor + 1;
		m_eCurColor = (BrickColor)iColor;
		break;
	}


	case Client::CBalloon::STATE_DIE:
			m_isDead = true;
		break;
	}

	Set_Color();

}

void CBalloon::Set_Color()
{
	switch (m_eCurColor)
	{
	case PINK:
		m_vColor = _vec4(1.f, 0.56f, 0.93f, 1.f);
		break;
	case YELLOW:
		m_vColor = _vec4(0.94f, 0.77f, 0.2f, 1.f);
		break;
	case PURPLE:
		m_vColor = _vec4(0.63f, 0.4f, 0.9f, 1.f);
		break;
	case BLUE:
		m_vColor = _vec4(0.f, 0.6f, 1.f, 1.f);
		break;
	case COLOR_END:
		if (m_eCurState != STATE_DIE)
		{
			m_eCurState = STATE_DIE;
			m_isDead = true;
		}
		break;
	default:
		break;
	}
	if(m_pCube != nullptr)
	{
		m_pCube->Set_Color(m_vColor);
	}

}

void CBalloon::Set_RandomColor()
{
	_uint iRandom = rand() % 100;

	if (iRandom < 50)
	{
		m_eCurColor = BLUE;
	}
	else if (iRandom < 70)
	{
		m_eCurColor = PURPLE;
	}
	else if (iRandom < 80)
	{
		m_eCurColor = PINK;
	}
	else
	{
		m_eCurColor = YELLOW;
	}
	Set_Color();
}

HRESULT CBalloon::Add_Collider()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::AABB;
	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(0.8f, 0.8f, 0.8f);
	CollDesc.vCenter = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Sphere"), (CComponent**)&m_pBodyColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}


void CBalloon::Update_BodyCollider()
{
	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

HRESULT CBalloon::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh_Effect"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Balloon"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_T_EFF_Noise_04_BC"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_cubealpha"), TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CBalloon::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CBalloon* CBalloon::Create(_dev pDevice, _context pContext)
{
	CBalloon* pInstance = new CBalloon(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBalloon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBalloon::Clone(void* pArg)
{
	CBalloon* pInstance = new CBalloon(*this);
                                                                                                                                                                                                                  
	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBalloon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBalloon::Free()
{
	__super::Free();

	Safe_Release(m_pCube);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pBodyColliderCom);

	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pDissolveTextureCom);
}
