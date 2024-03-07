#include "Wasp.h"
#include "CommonSurfaceTrail.h"
CWasp::CWasp(_dev pDevice, _context pContext)
	:CVTFMonster(pDevice, pContext)
{
}

CWasp::CWasp(const CWasp& rhs)
	:CVTFMonster(rhs)
{
}

HRESULT CWasp::Init_Prototype()
{
	return S_OK;
}

HRESULT CWasp::Init(void* pArg)
{
	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.5f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.1f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(1.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이
	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	SURFACETRAIL_DESC Desc{};
	Desc.vColor = _color(1.f, 0.5f, 0.1f, 1.f);

	Desc.iNumVertices = 8;
	m_pAttack_Trail = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);

	Desc.iPassIndex = 2;
	Desc.strMaskTextureTag = L"FX_J_Noise_Normal004_Tex";
	m_pAttack_Distortion_Trail = (CCommonSurfaceTrail*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CommonSurfaceTrail"), &Desc);
	
	m_pTransformCom->Set_Scale(_vec3(0.5f, 0.5f, 0.5f));

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	m_pTransformCom->Set_Speed(3.f);

	m_iHP = 200;

	m_eState = State_Idle;
	m_ePreState = m_eState;

	m_Animation.iAnimIndex = Anim_Idle;
	m_Animation.bSkipInterpolation = true;
	_randFloat RandomAnimPos(0.f, 1000.f);
	m_Animation.fStartAnimPos = RandomAnimPos(m_RandomNumber);

	m_Animation.isLoop = true;

	m_HasAttacked = true;

	return S_OK;
}

void CWasp::Tick(_float fTimeDelta)
{
	if (m_HasAttacked)
	{
		m_fAttackDelay += fTimeDelta;
		if (m_fAttackDelay >= 5.f)
		{
			m_HasAttacked = false;
			m_fAttackDelay = 0.f;
		}
	}

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);
	__super::Tick(fTimeDelta);

	Update_Trail();
	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());
	m_pAttackColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

void CWasp::Late_Tick(_float fTimeDelta)
{
	m_pAttack_Trail->Late_Tick(fTimeDelta);
	m_pAttack_Distortion_Trail->Late_Tick(fTimeDelta);
	__super::Late_Tick(fTimeDelta);
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif // DEBUG
}

HRESULT CWasp::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CWasp::Render_Instance()
{
	__super::Render_Instance();

	return S_OK;
}

void CWasp::Update_Trail()
{
	_vec3 vTopPos{};
	_vec3 vBotPos{};
	if (m_pAttack_Trail != nullptr)
	{
		vBotPos = m_pTransformCom->Get_World_Matrix().Position_vec3();
		vBotPos = vBotPos + _vec3(0.f, 0.5f, 0.f);
		vTopPos = vBotPos + _vec3(0.f, 1.6f, 0.f);
		m_pAttack_Trail->Tick(vTopPos, vBotPos);
		m_pAttack_Distortion_Trail->Tick(vTopPos, vBotPos);
	}

}

void CWasp::Init_State(_float fTimeDelta)
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
		case Client::CWasp::State_Idle:
			m_Animation.iAnimIndex = Anim_Idle;
			m_Animation.isLoop = true;

			m_pTransformCom->Set_Speed(3.f);
			m_fMoveDirRatio = 0.f;
			break;
		case Client::CWasp::State_Attack:
		{
			m_Animation.iAnimIndex = Anim_Run;
			m_Animation.isLoop = true;

			m_pTransformCom->Set_Speed(50.f);
			_vec3 vPlayerPos = m_pPlayerTransform->Get_State(State::Pos);
			vPlayerPos.y = m_pTransformCom->Get_State(State::Pos).y;
			_vec3 vLookAtMeDir = (m_pTransformCom->Get_State(State::Pos) - vPlayerPos).Get_Normalized();
			_vec3 vAttackPos = m_pPlayerTransform->Get_State(State::Pos) + vLookAtMeDir;
			m_vAttackDir = vAttackPos - m_pTransformCom->Get_State(State::Pos);

			m_pTransformCom->LookAt_Dir(m_vAttackDir);
			break;
		}
		case Client::CWasp::State_Attack_End:
		{
			m_Animation.iAnimIndex = Anim_Walk;
			m_Animation.isLoop = true;

			m_vAttackDir.y = 0.f;
			break;
		}
		case Client::CWasp::State_Die:
			m_Animation.iAnimIndex = Anim_Knockdown;

			m_pTransformCom->Delete_Controller();
			m_pGameInstance->Delete_CollisionObject(this);
			//사운드
			m_IsPlaySound = true;
			m_strSoundTag = TEXT("Crab_Die_SFX_02");
			break;
		}

		m_ePreState = m_eState;
	}
}

void CWasp::Tick_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CWasp::State_Idle:
	{
		_vec4 vPlayerPos = m_pPlayerTransform->Get_State(State::Pos);
		m_pTransformCom->LookAt(vPlayerPos);

		if (m_HasAttacked)
		{
			m_fAttackDelay += fTimeDelta;

			if (m_fAttackDelay >= 3.f)
			{
				m_eState = State_Attack;
				m_HasAttacked = false;
				m_fAttackDelay = 0.f;
			}
		}

		break;
	}
	case Client::CWasp::State_Attack:
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
		m_pAttack_Trail->On();
		m_pAttack_Distortion_Trail->On();
		PxRaycastBuffer Buffer{};
		_vec3 vDir = m_pTransformCom->Get_State(State::Look).Get_Normalized();
		if (m_pGameInstance->Raycast(m_pTransformCom->Get_State(State::Pos), vDir, 1.5f, Buffer))
		{
			m_eState = State_Attack_End;
			m_HasAttacked = true;
		}

		if (not m_HasAttacked)
		{
			if (m_pGameInstance->Attack_Player(m_pAttackColliderCom, rand() % 11 + 20))
			{
				m_HasAttacked = true;
				m_eState = State_Attack_End;
			}
		}

		if (m_HasAttacked)
		{
			m_pGameInstance->Play_Sound(TEXT("Whoosh_Quick_B_SFX_03"));
		}
	}
		break;
	case Client::CWasp::State_Attack_End:
	{
		m_fMoveDirRatio += fTimeDelta;
		if (m_fMoveDirRatio < 1.f)
		{
			_vec3 vUp = _vec3(0.f, 1.f, 0.f);
			_vec3 vLookAtPlayerDir = (m_pPlayerTransform->Get_State(State::Pos) - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
			_vec3 vLook = XMVectorLerp(m_pTransformCom->Get_State(State::Look), vLookAtPlayerDir, 0.1f);
			m_pTransformCom->Set_State(State::Look, vLook);
			_vec3 vRight = vUp.Cross(vLook);
			m_pTransformCom->Set_State(State::Right, vRight);
			vUp = vLook.Cross(vRight);
			m_pTransformCom->Set_State(State::Up, vUp);
			m_pTransformCom->Set_Scale(_vec3(0.5f, 0.5f, 0.5f));

			_vec4 vMoveDir = XMVectorLerp(m_vAttackDir, _vec3(0.f, 1.f, 0.f), m_fMoveDirRatio);
			vMoveDir.w = 0.f;
			m_pTransformCom->Go_To_Dir(vMoveDir, fTimeDelta);

			_float MoveSpeed = m_pTransformCom->Get_Speed();
			if (MoveSpeed > 5.f)
			{
				MoveSpeed -= fTimeDelta * 80.f;
				if (MoveSpeed <= 5.f)
				{
					MoveSpeed = 5.f;
				}
				m_pTransformCom->Set_Speed(MoveSpeed);
			}
		}
		else
		{
			m_pTransformCom->LookAt(m_pPlayerTransform->Get_State(State::Pos));
			m_pTransformCom->Go_To_Dir(_vec4(0.f, 1.f, 0.f, 0.f), fTimeDelta);
		}

		if (m_pTransformCom->Get_CenterPos().y >= 12.f)
		{
			m_eState = State_Idle;
		}

		break;
	}
	case Client::CWasp::State_Die:
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

HRESULT CWasp::Add_Components()
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

CWasp* CWasp::Create(_dev pDevice, _context pContext)
{
	CWasp* pInstance = new CWasp(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CWasp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWasp::Clone(void* pArg)
{
	CWasp* pInstance = new CWasp(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CWasp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWasp::Free()
{
	__super::Free();

	Safe_Release(m_pAttack_Trail);
	Safe_Release(m_pAttack_Distortion_Trail);
	Safe_Release(m_pAttackColliderCom);
}
