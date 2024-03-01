#include "TowerLazer.h"
#include "GuardTower.h"
#include "UI_Manager.h"
#include "Effect_Dummy.h"


CTowerLazer::CTowerLazer(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CTowerLazer::CTowerLazer(const CTowerLazer& rhs)
	: CGameObject(rhs)
	, EffectMatrix(rhs.EffectMatrix)
{
}

HRESULT CTowerLazer::Init_Prototype()
{
	return S_OK;
}

HRESULT CTowerLazer::Init(void* pArg)
{
	_mat m_LazerMatrix = *(_mat*)pArg;
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Collider()))
		return E_FAIL;

	m_pTransformCom->Set_Matrix(m_LazerMatrix);

	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Cannon_Laser");
	Info.pMatrix = &m_LazerMatrix;
	Info.isFollow = true;
	Info.fLifeTime = 0.1f;
	m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	//PxCapsuleControllerDesc ControllerDesc{};
	//ControllerDesc.height = 1.2f; // 높이(위 아래의 반구 크기 제외
	//ControllerDesc.radius = 0.4f; // 위아래 반구의 반지름
	//ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	//ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	//ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	//ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	//m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	return S_OK;
}

void CTowerLazer::Tick(_float fTimeDelta)
{
	
	
	if (m_pEffect)
		m_pEffect->Tick(fTimeDelta);


	m_pTransformCom->Set_OldMatrix();


	Update_Collider();


}

void CTowerLazer::Late_Tick(_float fTimeDelta)
{
	if (m_pEffect)
		m_pEffect->Late_Tick(fTimeDelta);


	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
	

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);

#endif // _DEBUG

}

HRESULT CTowerLazer::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (!m_bChangePass && m_iHP > 0)
	{
		m_iPassIndex = AnimPass_Default;
	}

	return S_OK;
}

_vec4 CTowerLazer::Compute_PlayerPos()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	return pPlayerTransform->Get_State(State::Pos);
}

_vec4 CTowerLazer::Compute_PlayerLook()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	return pPlayerTransform->Get_State(State::Look).Get_Normalized();
}

_float CTowerLazer::Compute_PlayerDistance()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);

	_float fDistance = (vPlayerPos - vPos).Length();

	return fDistance;
}

_vec4 CTowerLazer::Compute_Player_To_Dir()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
	//_vec4 vPos = m_pModelCom->Get_CenterPos();

	_vec4 vDir = (vPlayerPos - vPos).Get_Normalized();

	return vDir;
}

HRESULT CTowerLazer::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_T_EFF_Noise_04_BC"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}


HRESULT CTowerLazer::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(1.f, 2.5f, 1.f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider", L"Com_Collider_OBB", (CComponent**)&m_pColliderCom, &BodyCollDesc)))
		return E_FAIL;

	return S_OK;
}

void CTowerLazer::Update_Collider()
{
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

CTowerLazer* CTowerLazer::Create(_dev pDevice, _context pContext)
{
	CTowerLazer* pInstance = new CTowerLazer(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTowerLazer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTowerLazer::Clone(void* pArg)
{
	CTowerLazer* pInstance = new CTowerLazer(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTowerLazer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTowerLazer::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pEffect);
	Safe_Release(m_pColliderCom);
}
