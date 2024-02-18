#include "Human_Boss.h"
#include "Effect_Manager.h"
#include "Effect_Dummy.h"
CHuman_Boss::CHuman_Boss(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CHuman_Boss::CHuman_Boss(const CHuman_Boss& rhs)
	: CGameObject(rhs)
{
}

HRESULT CHuman_Boss::Init_Prototype()
{
	return S_OK;
}

HRESULT CHuman_Boss::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}
	
	m_pTransformCom->Set_State(State::Pos, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	Safe_AddRef(m_pPlayerTransform);
	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 1.8f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.7f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(1.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.01f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);
	m_pTransformCom->Set_Position(_vec3(100.325f, 8.5294f, 110.833f));
	m_Animation.iAnimIndex = BossAnim_attack01;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 1.5f;
	m_iPassIndex = AnimPass_DefaultNoCull;
	m_iWeaponPassIndex = AnimPass_Dissolve;
	m_iHP = 100;

	return S_OK;
}

void CHuman_Boss::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_NUMPAD8,InputChannel::UI))
	{
		m_eState = CommonAtt0;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD7, InputChannel::UI))
	{
		m_eState = CommonAtt1;
	}
	if (m_pGameInstance->Key_Down(DIK_NUMPAD6, InputChannel::UI))
	{
		m_eState = Hide_Start;
	}
	if (m_pFrameEffect)
	{
		m_pFrameEffect->Tick(fTimeDelta);
	}
	if (m_pBaseEffect)
	{
		m_pBaseEffect->Tick(fTimeDelta);
	}
	if (m_pDimEffect)
	{
		m_pDimEffect->Tick(fTimeDelta);
	}
	if (m_pAttackEffect)
	{
		m_pAttackEffect->Tick(fTimeDelta);
	}
	if (m_pShieldEffect)
	{
		m_pShieldEffect->Tick(fTimeDelta);

	}
	if (m_pRingEffect)
	{
		m_pRingEffect->Tick(fTimeDelta);
	}
	if (!m_bViewWeapon && m_fDissolveRatio < 1.f)
	{
		m_fDissolveRatio += fTimeDelta *2.f;
	}
	else if (m_bViewWeapon && m_fDissolveRatio > 0.f)
	{
		m_fDissolveRatio -= fTimeDelta *2.f;
	}
	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);
	After_Attack(fTimeDelta);
	Update_Collider();
	m_pModelCom->Set_Animation(m_Animation);
	m_pTransformCom->Gravity(fTimeDelta);
}

void CHuman_Boss::Late_Tick(_float fTimeDelta)
{
	if (m_pFrameEffect)
	{
		m_pFrameEffect->Late_Tick(fTimeDelta);
	}

	if (m_pBaseEffect)
	{
		m_pBaseEffect->Late_Tick(fTimeDelta);
	}
	if (m_pDimEffect)
	{
		m_pDimEffect->Late_Tick(fTimeDelta);
	}
	if (m_pShieldEffect)
	{
		m_pShieldEffect->Late_Tick(fTimeDelta);

	}
	if (m_pRingEffect)
	{
		m_pRingEffect->Late_Tick(fTimeDelta);
	}
	if (m_pAttackEffect)
	{
		m_pAttackEffect->Late_Tick(fTimeDelta);
	}
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyCollider);
	m_pRendererCom->Add_DebugComponent(m_pCommonAttCollider);
#endif

}

HRESULT CHuman_Boss::Render()
{
	
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

		for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
		{
			if (i == 3)
			{
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissolveRatio, sizeof _float)))
				{
					return E_FAIL;
				}
			}
			else
			{
				_float Ratio = 0.f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &Ratio, sizeof _float)))
				{
					return E_FAIL;
				}
			}
			if(!m_bSecondPattern)
			{
				if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
				{
					int a = 0;
				}
			}
			else
			{
				if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Opacity)))
				{
					int a = 0;
				}
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

			

			if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
			{
				return E_FAIL;
			}
			if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			{
				return E_FAIL;
			}

			if (i == 3)
			{
				if (FAILED(m_pShaderCom->Begin(m_iWeaponPassIndex)))
				{
					return E_FAIL;
				}

			}
			else
			{
				if (FAILED(m_pShaderCom->Begin(m_iPassIndex)))
				{
					return E_FAIL;
				}

			}
			
			if (FAILED(m_pModelCom->Render(i)))
			{
				return E_FAIL;
			}
	
	}

	return S_OK;
}

void CHuman_Boss::Init_State(_float fTimeDelta)
{

	if (m_bChangePass == true)
	{
		m_fHitTime += fTimeDelta;

		if (m_iPassIndex == AnimPass_DefaultNoCull)
		{
			m_iPassIndex = AnimPass_Rim;
			m_iWeaponPassIndex = AnimPass_Rim;
		}
		else
		{
			m_iPassIndex = AnimPass_DefaultNoCull;
			m_iWeaponPassIndex = AnimPass_Dissolve;
		}
		if (m_fHitTime >= 0.3f)
		{
			m_fHitTime = 0.f;
			m_bChangePass = false;
			m_iPassIndex = AnimPass_DefaultNoCull;
			m_iWeaponPassIndex = AnimPass_Dissolve;
		}
	}

	if (m_ePreState != m_eState)
	{
		m_Animation = {};
		m_Animation.fAnimSpeedRatio = 1.5f;
		switch (m_eState)
		{
		case Client::CHuman_Boss::CommonAtt0:
			m_Animation.iAnimIndex = BossAnim_attack01;
			m_Animation.isLoop = false;
			m_bAttacked = false;
			View_Attack_Range();
			break;
		case Client::CHuman_Boss::CommonAtt1:
		{
			m_Animation.iAnimIndex = BossAnim_attack02;
			m_Animation.isLoop = false;
			
			_vec4 vLook = m_pTransformCom->Get_State(State::Look);
			vLook.x *= -1;
			vLook.y = 0;
			vLook.z *= -1;
			m_pTransformCom->LookAt_Dir(vLook);
			m_bAttacked = false;
			View_Attack_Range();
		}
			break;
		case Client::CHuman_Boss::CommonAtt2:
			m_Animation.iAnimIndex = BossAnim_attack03;
			m_Animation.isLoop = false;
			View_Attack_Range();
			break;

		case Client::CHuman_Boss::Reflect:
			m_Animation.iAnimIndex = BossAnim_attack13;
			m_Animation.isLoop = false;
			break;
		case Client::CHuman_Boss::CounterAtt:
			break;
		case Client::CHuman_Boss::Hide_Start:
			m_Animation.iAnimIndex = BossAnim_attack06_Start;
			m_Animation.isLoop = false;
			break;
		case Client::CHuman_Boss::Hide:
			m_Animation.iAnimIndex = BossAnim_attack06_Loop;
			m_Animation.isLoop = true;
			break;
		case Client::CHuman_Boss::Hide_Att:
		{
			m_Animation.iAnimIndex = BossAnim_attack06_End;
			m_Animation.isLoop = false;

		}
			break;
		case Client::CHuman_Boss::Razer:
			break;
		case Client::CHuman_Boss::Hit:
			break;
		case Client::CHuman_Boss::Idle:
			break;
		case Client::CHuman_Boss::Walk:
			break;
		case Client::CHuman_Boss::Roar:
			break;
		case Client::CHuman_Boss::Run:
			break;
		case Client::CHuman_Boss::Die:
			break;
		case Client::CHuman_Boss::Spwan:
			break;
		default:
			break;
		}
		m_ePreState = m_eState;
	}

}

void CHuman_Boss::Tick_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case CommonAtt0:
		if (m_pModelCom->IsAnimationFinished(BossAnim_attack01))
		{
			m_eState = CommonAtt1;
		}
		break;
	case CommonAtt1:
		if (m_pModelCom->IsAnimationFinished(BossAnim_attack02))
		{
			m_eState = CommonAtt2;
		}
		break;
	case CommonAtt2:
		if (m_pModelCom->IsAnimationFinished(BossAnim_attack03))
		{
			m_eState = Idle;
		}
		break;
	case CounterAtt:
		if (m_pModelCom->IsAnimationFinished(BossAnim_attack13))
		{
			m_eState = Idle;
		}
		break;
	case Hide_Start:
		if (m_pModelCom->IsAnimationFinished(BossAnim_attack06_Start))
		{
			m_eState = Hide;
		}
		break;
	case Hide:
		break;
	case Hide_Att:
		if (m_pModelCom->IsAnimationFinished(BossAnim_attack06_End))
		{
			m_eState = Idle;
		}
		break;
	case Razer:
		break;
	case Hit:
		break;
	case Idle:
		break;
	case Walk:
		break;
	case Roar:
		break;
	case Run:
		break;
	case Die:
		break;
	case Spwan:
		break;
	default:
		break;
	}

}

HRESULT CHuman_Boss::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(2.f, 2.f, 2.f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pBodyCollider, &BodyCollDesc)))
		return E_FAIL;

	// Frustum
	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Sphere;
	ColDesc.vCenter = {};
	ColDesc.fRadius = 12.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pCommonAttCollider), &ColDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CHuman_Boss::Update_Collider()
{
	m_pBodyCollider->Update(m_pTransformCom->Get_World_Matrix());
	m_pCommonAttCollider->Update(m_pTransformCom->Get_World_Matrix());
}

void CHuman_Boss::Set_Damage(_int iDamage, _uint MonAttType)
{
	if (m_bShieldOn)
	{
		m_pGameInstance->Attack_Player(nullptr, iDamage, MonAtt_Hit);
		return;
	}
	m_bChangePass = true;

}

void CHuman_Boss::View_Attack_Range()
{
	if (m_pBaseEffect)
	{
		Safe_Release(m_pBaseEffect);
	}
	if (m_pDimEffect)
	{
		Safe_Release(m_pDimEffect);
	}
	if (m_pFrameEffect)
	{
		Safe_Release(m_pFrameEffect);
	}

	_mat EffectMatrix{};
	EffectInfo Info{};
	switch (m_eState)
	{
	case CommonAtt0:
	{
		EffectMatrix = _mat::CreateScale(30.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f,0.2f,0.f));
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_135_Frame");
		Info.pMatrix = &EffectMatrix;
		m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_135_Dim");
		Info.pMatrix = &EffectMatrix;
		m_pDimEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		m_fBaseEffectScale = 1.f;
		m_BaseEffectOriMat = _mat::CreateScale(m_fBaseEffectScale) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.24f, 0.f));
		m_BaseEffectMat = m_BaseEffectOriMat;
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_135_Base");
		Info.pMatrix = &m_BaseEffectMat;
		Info.isFollow = true;
		m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
		break;
	}
	case CommonAtt1:
	{
		EffectMatrix = _mat::CreateScale(30.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.2f, 0.f));
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_135_Frame");
		Info.pMatrix = &EffectMatrix;
		m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_135_Dim");
		Info.pMatrix = &EffectMatrix;
		m_pDimEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		m_fBaseEffectScale = 1.f;
		m_BaseEffectOriMat = _mat::CreateScale(m_fBaseEffectScale) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.24f, 0.f));
		m_BaseEffectMat = m_BaseEffectOriMat;
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_135_Base");
		Info.pMatrix = &m_BaseEffectMat;
		Info.isFollow = true;
		m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
		break;
	}
	case CommonAtt2:
	{
		EffectMatrix = _mat::CreateScale(30.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.2f, 0.f));
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Frame");
		Info.pMatrix = &EffectMatrix;
		m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Dim");
		Info.pMatrix = &EffectMatrix;
		m_pDimEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		m_fBaseEffectScale = 1.f;
		m_BaseEffectOriMat = _mat::CreateScale(m_fBaseEffectScale) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.24f, 0.f));
		m_BaseEffectMat = m_BaseEffectOriMat;
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Base");
		Info.pMatrix = &m_BaseEffectMat;
		Info.isFollow = true;
		m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
		break;
	}
	default:
		break;
	}



	
	


}

void CHuman_Boss::After_Attack(_float fTimedelta)
{
	m_ShieldEffectMat = _mat::CreateRotationY(1.f) * m_ShieldEffectMat;
	if (m_eState == CommonAtt0)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >= 0.f && Index <= 49.f)
		{
	
			if (m_fBaseEffectScale<30.f)
			{
				m_fBaseEffectScale += fTimedelta * 30.f;
			}

			m_BaseEffectMat = _mat::CreateScale(m_fBaseEffectScale) * m_BaseEffectOriMat;
		}

		if (Index >= 20.f && Index < 22.f && !m_bViewWeapon)
		{
			m_bViewWeapon = true;
		}
		else if (Index >= 49.f && Index < 51.f)
		{
			if (m_pBaseEffect)
			{
				Safe_Release(m_pBaseEffect);
			}
			if (m_pDimEffect)
			{
				Safe_Release(m_pDimEffect);
			}
			if (m_pFrameEffect)
			{
				Safe_Release(m_pFrameEffect);
			}
		}
		else if (Index >= 51.f && Index <= 61.f)
		{	
			if (!m_bAttacked)
			{
				if (Compute_Angle(135.f))
				{
					m_pGameInstance->Attack_Player(m_pCommonAttCollider, 200, MonAtt_KnockDown);
				}

				Safe_Release(m_pAttackEffect);
				Safe_Release(m_pShieldEffect);
				EffectInfo Info{};
				m_AttEffectMat = _mat::CreateScale(10.f)*_mat::CreateRotationX(135.f) * _mat::CreateTranslation(_vec3(0.f, 0.7f, 0.f)) * m_pTransformCom->Get_World_Matrix();
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Boss_Sword");
				Info.pMatrix = &m_AttEffectMat;
				Info.isFollow = true;
				//m_pAttackEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
				
				m_bAttacked = true;
			}
			if (m_bAttacked)
			{
				_vec3 vLook = m_pTransformCom->Get_State(State::Look).Get_Normalized();
				m_AttEffectMat *= (_mat::CreateTranslation(vLook)*0.5f);
			}
		}
		else if (Index >= 61.f && Index <= 63.f)
		{
			Safe_Release(m_pAttackEffect);
		}
		else if (Index >= 127.f && m_bViewWeapon)
		{
				m_bViewWeapon = false;

		}
	}
	else if (m_eState == CommonAtt1)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >= 0.f && Index <= 57.f)
		{

			if (m_fBaseEffectScale < 30.f)
			{
				m_fBaseEffectScale += fTimedelta * 30.f;
			}

			m_BaseEffectMat = _mat::CreateScale(m_fBaseEffectScale) * m_BaseEffectOriMat;
		}
		if (Index >= 8.f && Index < 10.f && !m_bViewWeapon)
		{
			m_bViewWeapon = true;
		}
		else if (Index >= 55.f && Index < 57.f)
		{
			if (m_pBaseEffect)
			{
				Safe_Release(m_pBaseEffect);
			}
			if (m_pDimEffect)
			{
				Safe_Release(m_pDimEffect);
			}
			if (m_pFrameEffect)
			{
				Safe_Release(m_pFrameEffect);
			}
		}
		else if (Index >= 57.f && Index <= 67.f)
		{
			if (!m_bAttacked)
			{
				if (Compute_Angle(135.f))
				{
					m_pGameInstance->Attack_Player(m_pCommonAttCollider, 200, MonAtt_KnockDown);
				}
			}
		}
		else if (Index >= 116.f && m_bViewWeapon)
		{
				m_bViewWeapon = false;
		}
	}
	else if (m_eState == CommonAtt2)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >= 0.f && Index < 115.f)
		{

			if (m_fBaseEffectScale < 30.f)
			{
				m_fBaseEffectScale += fTimedelta * 12.f;
			}

			m_BaseEffectMat = _mat::CreateScale(m_fBaseEffectScale) * m_BaseEffectOriMat;
		}
		if (Index >= 8.f && Index <= 11.f && !m_bViewWeapon)
		{
			m_bViewWeapon = true;
		}
		else if (Index >= 113.f && Index < 115.f)
		{
			if (m_pBaseEffect)
			{
				Safe_Release(m_pBaseEffect);
			}
			if (m_pDimEffect)
			{
				Safe_Release(m_pDimEffect);
			}

			if (m_pFrameEffect)
			{
				Safe_Release(m_pFrameEffect);
			}
		}
		else if (Index >= 115.f && Index <= 120.f)
		{
			if (!m_bAttacked)
			{
				m_pGameInstance->Attack_Player(m_pCommonAttCollider, 200, MonAtt_KnockDown);
			}
		}
		else if (Index >= 186.f && m_bViewWeapon)
		{
				m_bViewWeapon = false;
		}
	}
	else if (m_eState == Reflect)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >= 0.f && Index <= 2.f)
		{
			m_bViewWeapon = true;
		}
		if (Index >= 173.f && Index <= 175.f && !m_bShieldOn)
		{

			EffectInfo Info{};
			m_ShieldEffectMat = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(0.f, 1.8f, 0.f)) * m_pTransformCom->Get_World_Matrix();
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Boss_Shield");
			Info.pMatrix = &m_ShieldEffectMat;
			Info.isFollow = true;
			m_pShieldEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
			m_RingEffectMat = _mat::CreateScale(_vec3(8.f,5.f,8.f)) * _mat::CreateTranslation(_vec3(0.f, 1.8f, 0.f)) * m_pTransformCom->Get_World_Matrix();
			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Turn_Ring");
			Info.pMatrix = &m_RingEffectMat;
			Info.isFollow = true;
			//m_pRingEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
			m_bShieldOn = true;
		}
		else if (Index >= 202.f && m_bViewWeapon)
		{
			m_bViewWeapon = false;
		}
	}
	else if (m_eState == Hide_Start)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if(Index>=48.f && !m_bHide)
		{
			m_bHide = true;
		}
	}
	else if (m_eState == Hide_Att)
	{
		_float Index = m_pModelCom->Get_CurrentAnimPos();
		if (Index >= 0.f && Index <= 1.f)
		{

		_vec3 vPos = m_pPlayerTransform->Get_State(State::Pos) - (m_pPlayerTransform->Get_State(State::Look).Get_Normalized() * 1.9f);
		m_pTransformCom->Set_FootPosition(vPos);
		_vec4 vPlayerPos = m_pPlayerTransform->Get_State(State::Pos);
		m_pTransformCom->LookAt(vPlayerPos);
		}

	}
	if (m_bShieldOn && m_fShiledTimmer <10.f)
	{
		m_fShiledTimmer += fTimedelta;
		m_ShieldEffectMat =  _mat::CreateScale(1.f) * _mat::CreateRotationY(5.f) * _mat::CreateTranslation(_vec3(0.f, 1.8f, 0.f)) * m_pTransformCom->Get_World_Matrix();
		m_RingEffectMat =  _mat::CreateScale(_vec3(8.f, 5.f, 8.f)) *  _mat::CreateTranslation(_vec3(0.f, 1.8f, 0.f)) * m_pTransformCom->Get_World_Matrix();

	}
	else if (m_bShieldOn && m_fShiledTimmer >= 10.f)
	{
		m_bShieldOn = false;
		m_fShiledTimmer = 0.f;
		Safe_Release(m_pShieldEffect);
		Safe_Release(m_pRingEffect);
	}
	if (m_bHide && m_fHideTimmer < 7.f)
	{
		m_fHideTimmer += fTimedelta;
	}
	else if (m_bHide && m_fHideTimmer >= 7.f)
	{
		m_bHide = false;
		m_fHideTimmer = 0.f;
		m_eState = Hide_Att;
	}
}

_bool CHuman_Boss::Compute_Angle(_float fAngle)
{
	fAngle -= 85.f;
	_vec4 vLook = m_pTransformCom->Get_State(State::Look).Get_Normalized();
	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);

	_vec4 vPlayerPos = m_pPlayerTransform->Get_State(State::Pos);
	vPlayerPos -= vPos;
	vPlayerPos.Normalize();
	_float fResult = vLook.Dot(vPlayerPos);
	_float angleInRadians = acos(fResult);
	_float angleInDegrees = angleInRadians * (180.0f / XM_PI);

	
	if (angleInDegrees <= fAngle)
	{
		return true;
	}
	else
	{
		return false;
	}

}

HRESULT CHuman_Boss::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Human_Boss"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Logo_Noise"), TEXT("Com_Dissolve_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CHuman_Boss::Bind_ShaderResources()
{
	if (m_iPassIndex == AnimPass_Rim)
	{
		_vec4 vColor = Colors::Red;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &vColor, sizeof vColor)))
		{
			return E_FAIL;
		}
	}

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

	return S_OK;
}

CHuman_Boss* CHuman_Boss::Create(_dev pDevice, _context pContext)
{
	CHuman_Boss* pInstance = new CHuman_Boss(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CHuman_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHuman_Boss::Clone(void* pArg)
{
	CHuman_Boss* pInstance = new CHuman_Boss(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CHuman_Boss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHuman_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pDimEffect);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRingEffect);
	Safe_Release(m_pShieldEffect);
	Safe_Release(m_pBaseEffect);
	Safe_Release(m_pAttackEffect);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pFrameEffect);
	Safe_Release(m_pBodyCollider);
	Safe_Release(m_pPlayerTransform);
	Safe_Release(m_pCommonAttCollider);
	Safe_Release(m_pDissolveTextureCom);

}
