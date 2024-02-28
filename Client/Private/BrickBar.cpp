#include "BrickBar.h"
#include "Effect_Dummy.h"
#include "Effect_Manager.h"
#include "GameInstance.h"
#include "Collider.h"
CBrickBar::CBrickBar(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBrickBar::CBrickBar(const CBrickBar& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBrickBar::Init_Prototype()
{
	return S_OK;
}

HRESULT CBrickBar::Init(void* pArg)
{

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}
	m_fSpeed = 5.f;
	m_pTransformCom->Set_Scale(_vec3(1.f, 1.f, 0.5f));
	m_pTransformCom->Set_Speed(m_fSpeed);

	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec3 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
	vPlayerPos.y += 1.f; 
	_vec4 vCenterPos = _vec4(-2000.70496f, 1.4677677f, -1992.06152f, 1.f);
	_vec4 vPlayerLook = pPlayerTransform->Get_State(State::Look);
	vPlayerLook.y = 0.f;

	//m_pTransformCom->LookAt_Dir(vPlayerLook);
	m_pTransformCom->Set_State(State::Pos, vCenterPos);

	m_EffectMatrix = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"BrickArrow");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

	m_pColliderCom->Set_Normal();
	m_vColor = _vec4(1.f, 1.f, 1.f, 1.f);
	/*
	PxBoxControllerDesc ControllerDesc{};

	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이
	ControllerDesc.halfHeight = 0.8f;
	ControllerDesc.halfSideExtent = 0.8f;
	ControllerDesc.halfForwardExtent = 0.2f;
	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);
	*/
	//
	return S_OK;
}

void CBrickBar::Tick(_float fTimeDelta)
{
	m_shouldRenderBlur = true;
	m_pColliderCom->Change_Extents(_vec3(1.f, 1.f, 1.f));
	if (m_pGameInstance->Key_Pressing(DIK_LEFT))
	{
		_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
		vPos.x += fTimeDelta * 5.f; 
		m_pTransformCom->Set_State(State::Pos, vPos);
	}
	if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
		vPos.x -= fTimeDelta * 5.f; 
		m_pTransformCom->Set_State(State::Pos, vPos);
	}

	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
	_vec4 vPosCol = m_pColliderCom->Get_ColliderPos();
	
	m_EffectMatrix = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

	Update_Collider();

	if (m_pEffect_Ball)
	{
		m_pEffect_Ball->Tick(fTimeDelta);
	}

	

	return;


}

void CBrickBar::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_Blend, this);
	/*
	if (m_pEffect_Ball)
	{
		m_pEffect_Ball->Late_Tick(fTimeDelta);
	}
	*/

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CBrickBar::Render()
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

		_bool isBlur = true;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_isBlur", &isBlur, sizeof _bool)))
		{
			return E_FAIL;
		}

	
		if (FAILED(m_pShaderCom->Begin(StaticPass_MaskEffect)))
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

HRESULT CBrickBar::Add_Collider()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::AABB;
	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(0.1f, 0.2f, 0.1f);
	CollDesc.vCenter = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Bar"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

void CBrickBar::Update_Collider()
{
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}



HRESULT CBrickBar::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Effect_CommonCube"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_T_EFF_Noise_04_BC"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBrickBar::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CBrickBar* CBrickBar::Create(_dev pDevice, _context pContext)
{
	CBrickBar* pInstance = new CBrickBar(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBrickBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBrickBar::Clone(void* pArg)
{
	CBrickBar* pInstance = new CBrickBar(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBrickBar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBrickBar::Free()
{
	__super::Free();

	Safe_Release(m_pEffect_Ball);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
