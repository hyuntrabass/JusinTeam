#include "Larva.h"
#include "Larva_Ball.h"

CLarva::CLarva(_dev pDevice, _context pContext)
	:CVTFMonster(pDevice, pContext)
{
}

CLarva::CLarva(const CLarva& rhs)
	:CVTFMonster(rhs)
{
}

HRESULT CLarva::Init_Prototype()
{
	return S_OK;
}

HRESULT CLarva::Init(void* pArg)
{
	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 1.2f; // 높이(위 아래의 반구 크기 제외
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

	m_iHP = 300;

	m_eState = State_Idle;
	m_ePreState = m_eState;

	m_Animation.iAnimIndex = Anim_roar;
	m_Animation.bSkipInterpolation = true;
	_randFloat RandomAnimPos(0.f, 1000.f);
	m_Animation.fStartAnimPos = RandomAnimPos(m_RandomNumber);

	return S_OK;
}

void CLarva::Tick(_float fTimeDelta)
{
	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());

	__super::Tick(fTimeDelta);
}

void CLarva::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
#endif // DEBUG
}

HRESULT CLarva::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CLarva::Render_Instance()
{
	__super::Render_Instance();

	return S_OK;
}

void CLarva::Init_State(_float fTimeDelta)
{
	if (m_HasHitted == true)
	{
		if (m_iHP <= 0)
		{
			m_eState = State_Die;
			m_HasHitted = false;

			m_pShaderCom->Set_PassIndex(VTF_InstPass_Dissolve);
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
		case Client::CLarva::State_Idle:
			m_Animation.iAnimIndex = Anim_roar;
			m_Animation.fAnimSpeedRatio = 1.5f;
			break;
		case Client::CLarva::State_Attack:
		{
			m_Animation.iAnimIndex = Anim_attack01;

			_vec4 vPlayerPos = m_pPlayerTransform->Get_CenterPos();
			vPlayerPos.y = m_pTransformCom->Get_State(State::Pos).y;
			m_pTransformCom->LookAt(vPlayerPos);
			break;
		}
		case Client::CLarva::State_Die:
			m_Animation.iAnimIndex = Anim_stun;
			m_Animation.fDurationRatio = 0.1f;
			m_Animation.fInterpolationTime = 1.f;

			m_pTransformCom->Delete_Controller();
			m_pGameInstance->Delete_CollisionObject(this);
			//사운드
			m_IsPlaySound = true;
			m_strSoundTag = TEXT("Void_19_Die_Voice_02");
			m_fSoundVolume = 0.5f;
			m_fSoundStartPosRatio = 0.f;
			break;
		}

		m_ePreState = m_eState;
	}
}

void CLarva::Tick_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CLarva::State_Idle:
		if (m_pModelCom->IsAnimationFinished(Anim_roar))
		{
			m_eState = State_Attack;
		}
		break;
	case Client::CLarva::State_Attack:
	{
		_float fCurrentAnimPos = m_pModelCom->Get_CurrentAnimPos();
		if (fCurrentAnimPos >= 58.f && fCurrentAnimPos <= 61.f && not m_HasAttacked)
		{
			CLarva_Ball::LARVABALL_DESC LarvaBallDesc{};
			_vec3 vPos = m_pTransformCom->Get_CenterPos();
			vPos.y = 0.5f;
			LarvaBallDesc.vPosition = vPos;

			_vec4 vPlayerPos = m_pPlayerTransform->Get_CenterPos();
			vPlayerPos.y = 0.5f;
			LarvaBallDesc.vDirection = vPlayerPos - vPos;
			LarvaBallDesc.vDirection.w = 0.f;
			
			if (FAILED(m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_larva_Ball"), TEXT("Prototype_GameObject_Larva_Ball"), &LarvaBallDesc)))
				return;

			m_HasAttacked = true;
			//사운드
			m_IsPlaySound = true;
			m_strSoundTag = TEXT("Void_19_Attack01_Voice_02");
			m_fSoundVolume = 0.1f;
			m_fSoundStartPosRatio = 0.3f;
		}

		if (m_pModelCom->IsAnimationFinished(Anim_attack01))
		{
			m_eState = State_Idle;
			m_HasAttacked = false;
		}
		break;
	}
	case Client::CLarva::State_Die:
		if (m_fDissolveRatio < 1.f)
		{
			m_fDissolveRatio += fTimeDelta;
		}
		else
		{
			Kill();
		}
		break;
	}
}

HRESULT CLarva::Add_Components()
{
	Collider_Desc ColliderDesc{};
	ColliderDesc.eType = ColliderType::AABB;
	ColliderDesc.vExtents = _vec3(0.5f, 1.2f, 0.5f);
	ColliderDesc.vCenter = _vec3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pBodyColliderCom), &ColliderDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLarva* CLarva::Create(_dev pDevice, _context pContext)
{
	CLarva* pInstance = new CLarva(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CLarva");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLarva::Clone(void* pArg)
{
	CLarva* pInstance = new CLarva(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CLarva");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLarva::Free()
{
	__super::Free();
}
