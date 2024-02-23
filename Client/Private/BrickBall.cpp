#include "BrickBall.h"
#include "Effect_Dummy.h"
#include "Effect_Manager.h"

CBrickBall::CBrickBall(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBrickBall::CBrickBall(const CBrickBall& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBrickBall::Init_Prototype()
{
	return S_OK;
}

HRESULT CBrickBall::Init(void* pArg)
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
	m_pTransformCom->Set_Scale(_vec3(8.f, 8.f, 8.f));
	m_pTransformCom->Set_Speed(m_fSpeed);

	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec3 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
	vPlayerPos.y += 1.f;
	_vec4 vPlayerLook = pPlayerTransform->Get_State(State::Look);
	vPlayerLook.y = 0.f;

	m_pTransformCom->LookAt_Dir(vPlayerLook);
	m_pTransformCom->Set_Position(vPlayerPos);

	m_EffectMatrix = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_Ball_Init");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

	return S_OK;
}

void CBrickBall::Tick(_float fTimeDelta)
{
	_vec2 vCenterPos = _vec2(-1999.90186f, -1999.60742f);
	_vec2 vSize = _vec2(14.f, 20.f);
	RECT rcRect = {
		  (LONG)(vCenterPos.x - vSize.x * 0.5f),
		  (LONG)(vCenterPos.y - vSize.y * 0.5f),
		  (LONG)(vCenterPos.x + vSize.x * 0.5f),
		  (LONG)(vCenterPos.y + vSize.y * 0.5f)
	};
	POINT vCurPos = { m_pTransformCom->Get_State(State::Pos).x, m_pTransformCom->Get_State(State::Pos).z };
	if (!PtInRect(&rcRect, vCurPos))
	{
		m_isDead = true;
	}

	RayCast();

	m_pTransformCom->Go_Straight(fTimeDelta);
	m_pTransformCom->Set_Speed(m_fSpeed);

	m_EffectMatrix = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

	Update_Collider();

	if (m_pEffect_Ball)
	{
		m_pEffect_Ball->Tick(fTimeDelta);
	}

	if (m_pGameInstance->CheckCollision_Player(m_pColliderCom))
	{
		if (!m_isColl)
		{
			CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);

			_vec4 vPlayerLook = pPlayerTransform->Get_State(State::Look);
			vPlayerLook.y = 0.f;

			m_pTransformCom->LookAt_Dir(vPlayerLook);
			m_fSpeed = 10.f;
		}
		m_isColl = true;
	}
	else
	{
		m_fSpeed = 8.f;
		m_isColl = false;
	}


	if (m_pGameInstance->CheckCollision_Monster(m_pColliderCom))
	{
		if (!m_isColl)
		{
			_vec3 vLook = m_pTransformCom->Get_State(State::Pos);
			_vec3 vNormal = vLook * -1.f;
			m_vDir = _vec3::Reflect(vLook, vNormal);
			m_vDir.y = 0.f;
			m_pTransformCom->LookAt_Dir(m_vDir);

			EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_Ball_Smoke");
			Info.pMatrix = &m_EffectMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
			CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);

		}
		m_isColl = true;
	}
	else
	{
		m_fSpeed = 8.f;
		m_isColl = false;
	}
}

void CBrickBall::Late_Tick(_float fTimeDelta)
{
	//m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);

	if (m_pEffect_Ball)
	{
		m_pEffect_Ball->Late_Tick(fTimeDelta);
	}

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CBrickBall::Render()
{
	return S_OK;
}

HRESULT CBrickBall::Add_Collider()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.vCenter = _vec3(0.f);
	CollDesc.fRadius = 0.15f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Sphere"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

void CBrickBall::Update_Collider()
{
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}


void CBrickBall::RayCast()
{
	_float fDist = 0.51f;
	PxRaycastBuffer Buffer{};

	if (m_pGameInstance->Raycast(m_pTransformCom->Get_State(State::Pos), 
		m_pTransformCom->Get_State(State::Look).Get_Normalized(),
		fDist, Buffer))
	{
		_vec3 vNormal = _vec3(PxVec3ToVector(Buffer.block.normal));
		_vec3 vLook = m_pTransformCom->Get_State(State::Look).Get_Normalized();
		vNormal.Normalize();
		m_vDir = _vec3::Reflect(vLook, vNormal);
		m_vDir.y = 0.f;
		m_pTransformCom->LookAt_Dir(m_vDir);


		EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_Ball_Smoke");
		Info.pMatrix = &m_EffectMatrix;
		CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

	}

}

HRESULT CBrickBall::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_Ball");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	m_pEffect_Ball = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
	
	return S_OK;
}

HRESULT CBrickBall::Bind_ShaderResources()
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

CBrickBall* CBrickBall::Create(_dev pDevice, _context pContext)
{
	CBrickBall* pInstance = new CBrickBall(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBrickBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBrickBall::Clone(void* pArg)
{
	CBrickBall* pInstance = new CBrickBall(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBrickBall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBrickBall::Free()
{
	__super::Free();

	Safe_Release(m_pEffect_Ball);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);
}
