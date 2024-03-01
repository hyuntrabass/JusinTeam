#include "Scorpion.h"

CScorpion::CScorpion(_dev pDevice, _context pContext)
	:CVTFMonster(pDevice, pContext)
{
}

CScorpion::CScorpion(const CScorpion& rhs)
	:CVTFMonster(rhs)
{
}

HRESULT CScorpion::Init_Prototype()
{
	return S_OK;
}

HRESULT CScorpion::Init(void* pArg)
{
	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.5f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.5f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(1.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이
	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	m_pTransformCom->Set_Speed(3.f);

	m_iHP = 200;

	m_eState = State_Run;
	m_ePreState = m_eState;

	m_Animation.iAnimIndex = Anim_run;
	m_Animation.bSkipInterpolation = true;
	_randFloat RandomAnimPos(0.f, 1000.f);
	m_Animation.fStartAnimPos = RandomAnimPos(m_RandomNumber);

	m_Animation.isLoop = true;

	return S_OK;
}

void CScorpion::Tick(_float fTimeDelta)
{
	if (m_HasAttacked)
	{
		m_fAttackDelay += fTimeDelta;
		if (m_fAttackDelay >= 1.f)
		{
			m_HasAttacked = false;
		}
	}

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pTransformCom->Gravity(fTimeDelta);
	__super::Tick(fTimeDelta);

	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());
	m_pAttackColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

void CScorpion::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif // DEBUG
}

HRESULT CScorpion::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CScorpion::Render_Instance()
{
	__super::Render_Instance();

	return S_OK;
}

void CScorpion::Init_State(_float fTimeDelta)
{
	if (m_HasHitted == true)
	{
		if (m_iHP <= 0)
		{
			m_eState = State_Die;
			m_HasHitted = false;
		}
		else
		{
			m_fHitTime += fTimeDelta;

			m_pShaderCom->Set_PassIndex(VTF_InstPass_Main_Rim);

			if (m_fHitTime >= 0.3f)
			{
				m_fHitTime = 0.f;
				m_HasHitted = false;

				m_pShaderCom->Set_PassIndex(VTF_InstPass_Default);
			}
		}
	}

	if (m_eState != m_ePreState)
	{
		m_Animation = {};
		m_Animation.fAnimSpeedRatio = 2.f;

		switch (m_eState)
		{
		case Client::CScorpion::State_Idle:
			m_Animation.iAnimIndex = Anim_idle;
			m_Animation.isLoop = true;
			break;
		case Client::CScorpion::State_Run:
			m_Animation.iAnimIndex = Anim_run;
			m_Animation.isLoop = true;
			break;
		case Client::CScorpion::State_Attack:
		{
			m_Animation.iAnimIndex = Anim_attack03;

			_vec4 vPlayerPos = m_pPlayerTransform->Get_CenterPos();
			vPlayerPos.y = m_pTransformCom->Get_State(State::Pos).y;
			m_pTransformCom->LookAt(vPlayerPos);
			break;
		}
		case Client::CScorpion::State_Die:
			m_Animation.iAnimIndex = Anim_knockdown;

			m_pTransformCom->Delete_Controller();
			m_pGameInstance->Delete_CollisionObject(this);
			break;
		}

		m_ePreState = m_eState;
	}
}

void CScorpion::Tick_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CScorpion::State_Idle:
	{
		_vec4 vPlayerPos = m_pPlayerTransform->Get_CenterPos();
		vPlayerPos.y = m_pTransformCom->Get_State(State::Pos).y;
		_float fDis = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Length();
		if (fDis >= 2.f)
		{
			m_eState = State_Run;
		}
		else
		{
			if (not m_HasAttacked)
			{
				m_eState = State_Attack;
			}
		}
		break;
	}
	case Client::CScorpion::State_Run:
	{
		_vec4 vPlayerPos = m_pPlayerTransform->Get_CenterPos();
		vPlayerPos.y = m_pTransformCom->Get_State(State::Pos).y;
		m_pTransformCom->LookAt(vPlayerPos);

		m_pTransformCom->Go_Straight(fTimeDelta);

		_float fDis = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Length();
		if (fDis <= 2.f)
		{
			if (not m_HasAttacked)
			{
				m_eState = State_Attack;
			}
			else
			{
				m_eState = State_Idle;
			}
		}
		break;
	}
	case Client::CScorpion::State_Attack:
	{
		_float fCurrentAnimPos = m_pModelCom->Get_CurrentAnimPos();
		if (fCurrentAnimPos >= 20.f && fCurrentAnimPos <= 23.f && not m_HasAttacked)
		{
			_uint iDamage = rand() % 6 + 10;
			m_pGameInstance->Attack_Player(m_pAttackColliderCom, iDamage);
			m_HasAttacked = true;
		}

		if (m_pModelCom->IsAnimationFinished(Anim_attack03))
		{
			_vec4 vPlayerPos = m_pPlayerTransform->Get_CenterPos();
			vPlayerPos.y = m_pTransformCom->Get_State(State::Pos).y;
			_float fDis = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Length();

			if (fDis <= 2.f)
			{
				m_eState = State_Idle;
			}
			else
			{
				m_eState = State_Run;
			}
		}
		break;
	}
	case Client::CScorpion::State_Die:
		if (m_fDissolveRatio < 1.f)
		{
			m_fDissolveRatio += fTimeDelta;
			m_pShaderCom->Set_PassIndex(VTF_InstPass_Dissolve);
		}
		else
		{
			Kill();
		}
		break;
	}
}

HRESULT CScorpion::Add_Components()
{
	Collider_Desc ColliderDesc{};
	ColliderDesc.eType = ColliderType::AABB;
	ColliderDesc.vExtents = _vec3(0.5f, 1.2f, 0.5f);
	ColliderDesc.vCenter = _vec3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pBodyColliderCom), &ColliderDesc)))
	{
		return E_FAIL;
	}

	ColliderDesc = {};
	ColliderDesc.eType = ColliderType::Frustum;
	_mat matView = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	_mat matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(40.f), 0.5f, 0.01f, 3.f);
	ColliderDesc.matFrustum = matView * matProj;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColliderDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CScorpion* CScorpion::Create(_dev pDevice, _context pContext)
{
	CScorpion* pInstance = new CScorpion(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CScorpion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CScorpion::Clone(void* pArg)
{
	CScorpion* pInstance = new CScorpion(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CScorpion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CScorpion::Free()
{
	__super::Free();

	Safe_Release(m_pAttackColliderCom);
}
