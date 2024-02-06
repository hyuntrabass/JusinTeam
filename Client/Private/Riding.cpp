#include "Riding.h"
#include "FadeBox.h"
CRiding::CRiding(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CRiding::CRiding(const CRiding& rhs)
	: CGameObject(rhs)
{
}

HRESULT CRiding::Init_Prototype()
{
	return S_OK;
}

HRESULT CRiding::Init(void* pArg)
{
	Riding_Desc* Desc = (Riding_Desc*)pArg;
	m_CurrentIndex = Desc->Type;


	switch (m_CurrentIndex)
	{
	case Client::Bird:
	{
		m_Animation.iAnimIndex = Bird_1005_Start;
		m_eState = Riding_Sky;
		m_strPrototypeTag = TEXT("Prototype_Model_Riding_Bird");
	}
	break;
	case Client::Horse:
	{
		m_Animation.iAnimIndex = Horse_1004_Idle;
		m_eState = Riding_Idle;
		m_strPrototypeTag = TEXT("Prototype_Model_Riding_Horse");
		m_fRunSpeed = 10.f;
	}
	break;
	case Client::Tiger:
	{
		m_Animation.iAnimIndex = Tiger_1003_Idle;
		m_eState = Riding_Idle;
		m_strPrototypeTag = TEXT("Prototype_Model_Riding_Tiger");
		m_fRunSpeed = 12.f;
	}
		break;
	case Client::Nihilir:
	{
		m_Animation.iAnimIndex = Nihilir_VC_Nihilir_5002_Idle;
		m_eState = Riding_Idle;
		m_strPrototypeTag = TEXT("Prototype_Model_Riding_Nihilir");
		m_fWalkSpeed = 5.f;
		m_fRunSpeed = 14.f;
	}
		break;
	default:
		break;
	}
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	if (m_CurrentIndex == Nihilir)
	{
		PxCapsuleControllerDesc ControllerDesc{};
		ControllerDesc.height = 3.0f; // 높이(위 아래의 반구 크기 제외
		ControllerDesc.radius = 2.2f; // 위아래 반구의 반지름
		ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
		ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
		ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
		ControllerDesc.stepOffset = 0.3f; // 캐릭터가 오를 수 있는 계단의 최대 높이

		m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_PLAYER, &ControllerDesc);

	}
	else if (m_CurrentIndex == Tiger)
	{
		PxCapsuleControllerDesc ControllerDesc{};
		ControllerDesc.height = 1.176f; // 높이(위 아래의 반구 크기 제외
		ControllerDesc.radius = 0.65f; // 위아래 반구의 반지름
		ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
		ControllerDesc.slopeLimit = cosf(PxDegToRad(65.f)); // 캐릭터가 오를 수 있는 최대 각도
		ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
		ControllerDesc.stepOffset = 0.3f; // 캐릭터가 오를 수 있는 계단의 최대 높이
		m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_PLAYER, &ControllerDesc);

	}
	else if (m_CurrentIndex == Horse)
	{
		PxCapsuleControllerDesc ControllerDesc{};
		ControllerDesc.height = 1.176f; // 높이(위 아래의 반구 크기 제외
		ControllerDesc.radius = 0.6f; // 위아래 반구의 반지름
		ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
		ControllerDesc.slopeLimit = cosf(PxDegToRad(65.f)); // 캐릭터가 오를 수 있는 최대 각도
		ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
		ControllerDesc.stepOffset = 0.3f; // 캐릭터가 오를 수 있는 계단의 최대 높이
		m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_PLAYER, &ControllerDesc);

	}
	m_pTransformCom->Set_Position(_vec3(Desc->vSummonPos + _vec3(0.f,1.f,0.f)));
	m_Animation.fAnimSpeedRatio = 2.f;
	m_fDissolveRatio = 1.f;
	return S_OK;
}

void CRiding::Tick(_float fTimeDelta)
{
	if (m_CurrentIndex == Horse)
	{
		if (!m_isDead)
		{
			m_fDissolveRatio = 0.f;
		}
		else
		{
			_vec3 vPos = m_pTransformCom->Get_State(State::Pos) + _vec4(0.f, 1.f, 0.f, 0.f);
			m_pTransformCom->Set_Position(vPos);
			m_bDelete = true;
		}
	}
	else
	{
		if (m_fDissolveRatio >= 0.f && !m_isDead)
		{
			m_fDissolveRatio -= fTimeDelta / 2.f;
		}
		else if (m_isDead)
		{
			m_fDissolveRatio += fTimeDelta / 1.4f;

			if (m_fDissolveRatio >= 1.f)
			{
				_vec3 vPos = m_pTransformCom->Get_State(State::Pos) + _vec4(0.f, 1.f, 0.f, 0.f);
				m_pTransformCom->Set_Position(vPos);
				m_bDelete = true;
			}
		}
	}
	
	if (m_pGameInstance->Key_Down(DIK_L))

	{
		if (m_CurrentIndex == Bird)
		{
			m_eState = Riding_Landing;
			m_hasJumped = false;
		}
	}
	Move(fTimeDelta);
	Init_State();
	Tick_State(fTimeDelta);
	m_pModelCom->Set_Animation(m_Animation);
	if (m_CurrentIndex == Bird)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (m_eState == Riding_Sky)
		{
			if (Index >= 88.f && !m_hasJumped)
			{


				m_hasJumped = true;
			}
			else if (Index >= 95.f)
			{
				CFadeBox::FADE_DESC Desc = {};
				Desc.eState = CFadeBox::FADEOUT;
				Desc.fDuration = 1.5f;
				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FadeBox"), &Desc)))
				{
					return;
				}

				m_bDelete = true;

			}
		}
		else if (m_eState == Riding_Landing)
		{
			if (Index >= 1.f && !m_hasJumped)
			{
				CFadeBox::FADE_DESC Desc = {};
				Desc.eState = CFadeBox::FADEOUT;
				Desc.fDuration = 0.7f;
				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FadeBox"), &Desc)))
				{
					return;
				}
				m_hasJumped = true;
			}
		}
	}
	if (m_CurrentIndex != Bird)
	{
		m_pTransformCom->Gravity(fTimeDelta);
	}

	//Update_Collider();
}

void CRiding::Late_Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

#ifdef _DEBUG
	/*m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);*/
#endif
}

HRESULT CRiding::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}


	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			return E_FAIL;
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

		if (FAILED(m_pShaderCom->Begin(AnimPass_Dissolve)))
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

void CRiding::Move(_float fTimeDelta)
{
	_bool hasMoved{};
	_vec4 vForwardDir = m_pGameInstance->Get_CameraLook();
	vForwardDir.y = 0.f;
	_vec4 vRightDir = XMVector3Cross(m_pTransformCom->Get_State(State::Up), vForwardDir);
	vRightDir.Normalize();

	_vec4 vDirection{};
	if (m_CurrentIndex == Bird)
	{
		return;
	}

	if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		vDirection += vForwardDir;
		hasMoved = true;
	}
	else if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		vDirection -= vForwardDir;
		hasMoved = true;
	}

	if (m_pGameInstance->Key_Pressing(DIK_D))
	{
		vDirection += vRightDir;
		hasMoved = true;
	}
	else if (m_pGameInstance->Key_Pressing(DIK_A))
	{
		vDirection -= vRightDir;
		hasMoved = true;
	}
	
	if (m_eState == Riding_Jump_Start or
		m_eState == Riding_Jump)
	{
		if (!m_pTransformCom->Is_Jumping())
		{
			m_eState = Riding_Jump_End;
			m_hasJumped = false;
		}
	}

	

	if (hasMoved)
	{

		if (m_pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			if (m_eState == Riding_Jump_End)
			{
				m_eState = Riding_Jump_Run;
			}

			if (m_eState == Riding_Walk or m_eState == Riding_Idle)
			{
				m_eState = Riding_Run;
			}
			m_pTransformCom->Set_Speed(m_fRunSpeed);
		}
		else if (m_eState == Riding_Run)
		{
			m_eState = Riding_Run;
			m_pTransformCom->Set_Speed(m_fRunSpeed);
		}
		else
		{
			if (m_CurrentIndex == Tiger or m_CurrentIndex == Horse)
			{
				if (m_eState == Riding_Jump_End)
				{
					m_eState = Riding_Jump_Run;
				}
				else if (m_eState == Riding_Run or
					m_eState == Riding_Walk or
					m_eState == Riding_Idle)
				{
					m_eState = Riding_Run;
				}



				m_pTransformCom->Set_Speed(m_fRunSpeed);
			}
			else
			{
				if (m_eState == Riding_Idle or
					m_eState == Riding_Jump_End or
					m_eState == Riding_Walk)
				{
					m_eState = Riding_Walk;
				}
				m_pTransformCom->Set_Speed(m_fWalkSpeed);
			}
		}
		m_pTransformCom->Go_To_Dir(vDirection, fTimeDelta);
		_vec4 vLook = m_pTransformCom->Get_State(State::Look).Get_Normalized();

		if (vLook.Dot(vDirection) < 0)
		{
			vLook = vDirection;
		}
		else
		{
			vLook = XMVectorLerp(vLook, vDirection, 0.15f);
		}

		m_pTransformCom->LookAt_Dir(vLook);
	}
	else if (m_eState == Riding_Walk or m_eState == Riding_Run)
	{
		m_eState = Riding_Idle;
	}
	if (m_pGameInstance->Key_Down(DIK_SPACE, InputChannel::GamePlay))
	{
		if (!m_hasJumped)
		{
			m_eState = Riding_Jump_Start;
			m_pTransformCom->Jump(10.f);
		}
	}
}
void CRiding::Init_State()
{
	if (m_eState != m_ePrevState)
	{
		m_Animation.isLoop = false;
		m_Animation.fAnimSpeedRatio = 2.f;
		m_Animation.bSkipInterpolation = false;

		switch (m_eState)
		{
		case Client::Riding_Landing:
		{
			m_Animation.iAnimIndex = Bird_2005_Landing;
			m_Animation.isLoop = false;
			m_hasJumped = false;
		}
			break;
		case Client::Riding_Idle:
			switch (m_CurrentIndex)
			{
			case Client::Bird:
				break;
			case Client::Tiger:
			{
				m_Animation.iAnimIndex = Tiger_1003_Idle;
				m_Animation.isLoop = true;
				m_hasJumped = false;
			}
				break;
			case Client::Nihilir:
			{
				m_Animation.iAnimIndex = Nihilir_VC_Nihilir_5002_Idle;
				m_Animation.isLoop = true;
				m_hasJumped = false;
			}
				break;
			case Client::Horse:
			{
				m_Animation.iAnimIndex = Horse_1004_Idle;
				m_Animation.isLoop = true;
				m_hasJumped = false;
			}
				break;

			default:
				break;
			}
			break;
		case Client::Riding_Run:
			switch (m_CurrentIndex)
			{
			case Client::Bird:
				break;
			case Client::Tiger:
			{
				m_Animation.iAnimIndex = Tiger_1003_Run;
				m_Animation.isLoop = true;
				m_hasJumped = false;
			}
				break;
			case Client::Nihilir:
			{
				m_Animation.iAnimIndex = Nihilir_VC_Nihilir_5002_Run;
				m_Animation.isLoop = true;
				m_hasJumped = false;
			}
			break;
			case Client::Horse:
			{
				m_Animation.iAnimIndex = Horse_1004_Run_F;
				m_Animation.isLoop = true;
				m_hasJumped = false;
			}
				break;
			default:
				break;
			}
			break;
		case Client::Riding_Walk:
			switch (m_CurrentIndex)
			{
			case Client::Bird:
				break;
			case Client::Tiger:
			{
				m_Animation.iAnimIndex = Tiger_1003_Walk;
				m_Animation.bSkipInterpolation = true;
				m_Animation.isLoop = true;
				m_hasJumped = false;
			}
				break;
			case Client::Nihilir:
			{
				m_Animation.iAnimIndex = Nihilir_VC_Nihilir_5002_Walk;
				m_Animation.isLoop = true;
				m_hasJumped = false;
			}
				break;
			case Client::Horse:
			{
				m_Animation.iAnimIndex = Horse_1004_Run_F;
				m_Animation.isLoop = true;
				m_hasJumped = false;
			}
			break;

			default:
				break;
			}
			break;
		case Client::Riding_Attack:
		{
			m_hasJumped = false;
		}
			break;
		case Client::Riding_Jump_Start:
			switch (m_CurrentIndex)
			{
			case Client::Tiger:
			{
				m_Animation.iAnimIndex = Tiger_1003_Jump_Start;
				m_hasJumped = true;
			}
				break;
			case Client::Nihilir:
			{
				m_Animation.iAnimIndex = Nihilir_VC_Nihilir_5002_Jump_Start;
				m_hasJumped = true;
			}
				break;
			case Client::Horse:
			{
				m_Animation.iAnimIndex = Horse_1004_jump_start;
				m_hasJumped = true;
			}
			break;
			default:
				break;
			}
			break;
		case Client::Riding_Jump:
			switch (m_CurrentIndex)
			{
			case Client::Tiger:
			{
				m_Animation.iAnimIndex = Tiger_1003_Jump_loop;
				m_hasJumped = true;
				m_Animation.isLoop = true;
			}
				break;
			case Client::Nihilir:
			{
				m_Animation.iAnimIndex = Nihilir_VC_Nihilir_5002_Jump_Loop;
				m_hasJumped = true;
				m_Animation.isLoop = true;
			}
				break;
			case Client::Horse:
			{
				m_Animation.iAnimIndex = Horse_1004_jump_loop;
				m_hasJumped = true;
				m_Animation.isLoop = true;
			}
			break;
			default:
				break;
			}
			break;
		case Client::Riding_Jump_Run:
			switch (m_CurrentIndex)
			{
			case Client::Tiger:
			{
				m_Animation.iAnimIndex = Tiger_1003_Jump_End_Run;
				m_hasJumped = false;
			}
				break;
			case Client::Nihilir:
			{
				m_Animation.iAnimIndex = Nihilir_VC_Nihilir_5002_Jump_End_Run;
				m_hasJumped = false;
			}
				break;
			case Client::Horse:
			{
				m_Animation.iAnimIndex = Horse_1004_jump_End_Run;
				m_hasJumped = false;
			}
			break;
			default:
				break;
			}
			break;
		case Client::Riding_Jump_End:
			switch (m_CurrentIndex)
			{
			case Client::Tiger:
			{
				m_Animation.iAnimIndex = Tiger_1003_Jump_End;
				m_hasJumped = false;
			}
				break;
			case Client::Nihilir:
			{
				m_Animation.iAnimIndex = Nihilir_VC_Nihilir_5002_Jump_End;
				m_hasJumped = false;
			}
				break;
			case Client::Horse:
			{
				m_Animation.iAnimIndex = Horse_1004_jump_End;
				m_hasJumped = false;
				m_Animation.isLoop = false;
			}
			break;
			default:
				break;
			}
			break;
		case Client::Riding_Sky:
		{
			m_Animation.iAnimIndex = Bird_1005_Takeoff;
		}
			break;
		case Client::Riding_End:
			break;
		default:
			break;
		}
		m_ePrevState = m_eState;
	}
}

void CRiding::Tick_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::Riding_Landing:
		if (m_pModelCom->IsAnimationFinished(Bird_2005_Landing))
		{
			m_pGameInstance->Set_FlyCam(false);


			Delete_Riding();
			CFadeBox::FADE_DESC Desc = {};
			Desc.eState = CFadeBox::FADEOUT;
			Desc.fDuration = 1.f;
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FadeBox"), &Desc)))
			{
				return;
			}
		}
		break;
	case Client::Riding_Idle:
		break;
	case Client::Riding_Jump_Start:
	{
		switch (m_CurrentIndex)
		{
		case Client::Tiger:
		{
			if (m_pModelCom->IsAnimationFinished(Tiger_1003_Jump_Start))
			{
				m_eState = Riding_Jump;
			}
		}
		break;
		case Client::Nihilir:
		{
			if (m_pModelCom->IsAnimationFinished(Nihilir_VC_Nihilir_5002_Jump_Start))
			{
				m_eState = Riding_Jump;
			}
		}
		case Client::Horse:
		{
			if (m_pModelCom->IsAnimationFinished(Horse_1004_jump_start))
			{
				m_eState = Riding_Jump;
			}
		}
		break;
		default:
			break;
		}
	}
		break;
	case Client::Riding_Jump_End:
	{
		switch (m_CurrentIndex)
		{
		case Client::Tiger:
		{
			if (m_pModelCom->IsAnimationFinished(Tiger_1003_Jump_End))
			{
				m_eState = Riding_Idle;
			}
		}
		break;
		case Client::Nihilir:
			if (m_pModelCom->IsAnimationFinished(Nihilir_VC_Nihilir_5002_Jump_End))
			{
				m_eState = Riding_Idle;
			}
			break;
		case Client::Horse:
		{
			if (m_pModelCom->IsAnimationFinished(Horse_1004_jump_End))
			{
				m_eState = Riding_Idle;
			}
		}
		break;
		default:
			break;
		}
	}
		break;
	case Client::Riding_Jump_Run:
	{
		switch (m_CurrentIndex)
		{
		case Client::Tiger:
			if (m_pModelCom->IsAnimationFinished(Tiger_1003_Jump_End_Run))
			{
				m_eState = Riding_Run;
			}
			break;
		case Client::Nihilir:
			if (m_pModelCom->IsAnimationFinished(Nihilir_VC_Nihilir_5002_Jump_End_Run))
			{
				m_eState = Riding_Run;
			}
			break;
		case Client::Horse:
		{
			if (m_pModelCom->IsAnimationFinished(Horse_1004_jump_End_Run))
			{
				m_eState = Riding_Run;
			}
		}
		break;
		default:
			break;
		}
	}
		break;
	case Client::Riding_Run:
		break;
	case Client::Riding_Walk:
		break;
	case Client::Riding_Attack:
		break;
	case Client::Riding_Jump:
		break;
	case Client::Riding_Sky:
		// 애니메이션 종료후 지역 이동하는 효과
		break;
	case Client::Riding_End:
		break;
	default:
		break;
	}
}

_mat CRiding::Get_Mat()
{
	_mat OffsetMat{};
	if (m_CurrentIndex == Tiger)
	{
		OffsetMat = _mat::CreateRotationY(XMConvertToRadians(-90.f)) * *m_pModelCom->Get_BoneMatrix("saddle");
	}
	else if (m_CurrentIndex == Nihilir)
	{
		OffsetMat = _mat::CreateRotationY(XMConvertToRadians(-180.f)) * _mat::CreateRotationX(XMConvertToRadians(-90.f)) * *m_pModelCom->Get_BoneMatrix("saddle");
	}
	else if (m_CurrentIndex == Bird)
	{
		OffsetMat = _mat::CreateTranslation(0.f, 0.8f, 0.f) * _mat::CreateRotationZ(XMConvertToRadians(-180.f)) * _mat::CreateRotationY(XMConvertToRadians(90.f)) * *m_pModelCom->Get_BoneMatrix("Saddle");
	}
	else if (m_CurrentIndex == Horse)
	{
		OffsetMat = _mat::CreateRotationY(XMConvertToRadians(-180.f)) * _mat::CreateRotationX(XMConvertToRadians(-90.f)) * *m_pModelCom->Get_BoneMatrix("saddle");
	}
	OffsetMat *= m_pTransformCom->Get_World_Matrix();

	return (OffsetMat);
}

_vec4 CRiding::Get_Pos()
{
	return m_pTransformCom->Get_State(State::Pos);
}



void CRiding::Update_Collider()
{
	//m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());

	_mat Offset = _mat::CreateTranslation(0.f, 2.f, 1.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
}

void CRiding::Delete_Riding()
{
	//죽는 처리 후 m_bReady_Dead = true;
	m_isDead = true;
}

HRESULT CRiding::Add_Components()
{


	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strPrototypeTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Logo_Noise"), TEXT("Com_Dissolve_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
	{
		return E_FAIL;
	}

	//Collider_Desc BodyCollDesc = {};
	//BodyCollDesc.eType = ColliderType::OBB;
	//BodyCollDesc.vExtents = _vec3(2.f, 2.f, 2.f);
	//BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	//BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
	//	TEXT("Com_Collider_OBB"), (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
	//	return E_FAIL;

	// Frustum
	//Collider_Desc ColDesc{};
	//ColDesc.eType = ColliderType::Frustum;
	//_matrix matView = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	//// 1인자 : 절두체 각도(범위), 2인자 : Aspect, 3인자 : Near, 4인자 : Far(절두체 깊이)
	//_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), 1.f, 0.01f, 3.f);
	//XMStoreFloat4x4(&ColDesc.matFrustum, matView * matProj);

	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColDesc)))
	//{
	//	return E_FAIL;
	//}


	return S_OK;


}

HRESULT CRiding::Bind_ShaderResources()
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



	if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissolveRatio, sizeof _float)))
	{
		return E_FAIL;
	}


	return S_OK;
}

CRiding* CRiding::Create(_dev pDevice, _context pContext)
{
	CRiding* pInstance = new CRiding(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CRiding");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRiding::Clone(void* pArg)
{
	CRiding* pInstance = new CRiding(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CRiding");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRiding::Free()
{
	__super::Free();

	Safe_Release(m_pDissolveTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);

	//Safe_Release(m_pBodyColliderCom);
	//Safe_Release(m_pAttackColliderCom);

}
