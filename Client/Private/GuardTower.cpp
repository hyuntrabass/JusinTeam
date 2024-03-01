#include "GuardTower.h"
#include "UI_Manager.h"
#include "Effect_Dummy.h"


CGuardTower::CGuardTower(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CGuardTower::CGuardTower(const CGuardTower& rhs)
	: CGameObject(rhs)
	, m_Info(rhs.m_Info)

{
}

HRESULT CGuardTower::Init_Prototype()
{
	m_isPrototype = true;

	return S_OK;
}

HRESULT CGuardTower::Init(void* pArg)
{
	m_Info = *(GuardTowerInfo*)pArg;
	m_iIndex = m_Info.iIndex;
	m_GuardTowerMatrix = m_Info.mMatrix;
	m_LazerMatrix = m_GuardTowerMatrix;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Collider()))
		return E_FAIL;

	m_Animation.iAnimIndex = ANIM_IDLE;
	m_Animation.isLoop = false;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 1.f;

	m_iHP = 1;

	m_pTransformCom->Set_Matrix(m_GuardTowerMatrix);

	//m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_45_Frame");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_45_Base");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Cannon_Laser_Warning");
	Info.pMatrix = &m_LazerMatrix;
	Info.isFollow = true;
	m_pThreatEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);



	m_eCurState = STATE_IDLE;

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 1.2f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.4f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	CUI_Manager::Get_Instance()->Set_RadarPos(CUI_Manager::MONSTER, m_pTransformCom);

	return S_OK;
}

void CGuardTower::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_UP))
	{
		m_eCurState = STATE_IDLE;
	}
	if (m_pGameInstance->Key_Pressing(DIK_DOWN))
	{
		m_eCurState = STATE_DIE;
	}
	if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		m_eCurState = STATE_DETECT;
	}

	if (m_pGameInstance->Key_Pressing(DIK_EQUALS))
	{
		Kill();
		m_pGameInstance->Delete_CollisionObject(this);
		m_pTransformCom->Delete_Controller();
	}

	if (m_eCurState == STATE_DETECT)
	{
		if (m_pFrameEffect)
			m_pFrameEffect->Tick(fTimeDelta);
		if (m_pBaseEffect)
			m_pBaseEffect->Tick(fTimeDelta);
	}
	if (m_eCurState == STATE_ATTACK_READY)
	{
		if (m_pThreatEffect)
			m_pThreatEffect->Tick(fTimeDelta);
	}
	if(m_eCurState == STATE_ATTACK)
	{
		if (m_pAttackEffect)
			m_pAttackEffect->Tick(fTimeDelta);
	}
	m_pTransformCom->Set_OldMatrix();

	//if (m_bChangePass == true)
	//{
	//	m_fHitTime += fTimeDelta;

	//	if (m_iPassIndex == AnimPass_Default)
	//	{
	//		m_iPassIndex = AnimPass_Rim;
	//	}
	//	else
	//	{
	//		m_iPassIndex = AnimPass_Default;
	//	}

	//	if (m_fHitTime >= 1.f)
	//	{
	//		m_fHitTime = 0.f;
	//		m_bChangePass = false;
	//		m_iPassIndex = AnimPass_Default;
	//	}
	//}

	//if (true == m_bChangePass) {
	//	m_fHitTime += fTimeDelta;

	//	if (0.3f <= m_fHitTime) {
	//		m_fHitTime = 0.f;
	//		m_bChangePass = false;
	//	}
	//}

	//if (0 >= m_iHP || 0.01f < m_fDeadTime) {
	//	m_pGameInstance->Delete_CollisionObject(this);
	//	m_pTransformCom->Delete_Controller();
	//}

	//if (1.f <= m_fDissolveRatio)
	//	Kill();

	//if (m_fDeadTime >= 2.f)
	//{
	//	m_iPassIndex = AnimPass_Dissolve;
	//}

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);
	Update_Collider();
}

void CGuardTower::Late_Tick(_float fTimeDelta)
{
	if (m_eCurState == STATE_DETECT)
	{
		if (m_pFrameEffect)
			m_pFrameEffect->Late_Tick(fTimeDelta);

		if (m_pBaseEffect)
			m_pBaseEffect->Late_Tick(fTimeDelta);
	}

	if (m_eCurState == STATE_ATTACK_READY)
	{
		if (m_pThreatEffect)
			m_pThreatEffect->Late_Tick(fTimeDelta);
	}
	if (m_eCurState == STATE_ATTACK)
	{
		if (m_pAttackEffect)
			m_pAttackEffect->Late_Tick(fTimeDelta);
	}
	if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_CenterPos(), 20.f))
	{
		m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
	}

	if(m_eCurState != STATE_IDLE)
		m_pModelCom->Play_Animation(fTimeDelta);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);

#endif // _DEBUG

}

HRESULT CGuardTower::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i) {
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
			return E_FAIL;

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
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(m_iPassIndex)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}

	if (!m_bChangePass && m_iHP > 0)
	{
		m_iPassIndex = AnimPass_Default;
	}

	return S_OK;
}

void CGuardTower::Set_Damage(_int iDamage, _uint iDamageType)
{
	//m_fHittedTime = 6.f;

	//m_iHP -= iDamage;
	//m_bChangePass = true;
	if (iDamageType == AT_Sword_Common)
	{
		m_bDamaged = true;
	}
	CUI_Manager::Get_Instance()->Set_HitEffect(m_pTransformCom, iDamage, _vec2(0.f, 2.f), (ATTACK_TYPE)iDamageType);

}

void CGuardTower::Init_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = Compute_PlayerPos();
	_float fDistance = Compute_PlayerDistance();
	_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_CenterPos()).Get_Normalized();

	if (m_ePreState != m_eCurState) {
		switch (m_eCurState)
		{
		case Client::CGuardTower::STATE_IDLE:
			m_Animation.iAnimIndex = ANIM_IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 1.f;
			break;
		case Client::CGuardTower::STATE_DETECT:
			m_Animation.iAnimIndex = ANIM_IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 1.f;
			break;
		case Client::CGuardTower::STATE_ATTACK_READY:
			m_Animation.iAnimIndex = ANIM_IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 1.f;
			break;
		case Client::CGuardTower::STATE_ATTACK:
			m_Animation.iAnimIndex = ANIM_IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 1.f;
			break;
		case Client::CGuardTower::STATE_DIE:
			m_Animation.iAnimIndex = ANIM_DIE;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;
			break;
		}

		m_ePreState = m_eCurState;
	}

}

void CGuardTower::Tick_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = Compute_PlayerPos();
	_float fDistance = Compute_PlayerDistance();
	_vec4 vLazerLook{};
	_float fDist = 1.0f;
	PxRaycastBuffer Buffer{};
	View_Detect_Range(m_fDetectTime);

	switch (m_eCurState)
	{
	case Client::CGuardTower::STATE_IDLE:
		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 1.f)
		{
			m_eCurState = STATE_DETECT;
			m_fIdleTime = 0.f;
		}

		break;

	case Client::CGuardTower::STATE_DETECT:


		m_fDetectTime += fTimeDelta * 20.f;
		if (m_bInit == true)
		{
			m_fDetectTime = 0.f;
			m_bInit = false;
		}

		if (m_isDetected == true)
		{
			m_eCurState = STATE_ATTACK_READY;
		}

		break;
	case Client::CGuardTower::STATE_ATTACK_READY:
		//Delete_Range();

		m_fAttackTime += fTimeDelta;

		if (m_isDetected == false)
		{	
			m_eCurState = STATE_IDLE;
			m_fAttackTime = 0.f;

		}
		else
		{
			if (m_fAttackTime > 1.f)
			{
				m_eCurState = STATE_ATTACK;
				m_vCurPlayerPos = Compute_PlayerPos();
				Create_Attack_Lazer();
				m_fAttackTime = 0.f;
			}
		}

		break;
	case Client::CGuardTower::STATE_ATTACK:

		m_fAttackTime += fTimeDelta;
		if (m_fAttackTime > 1.f)
		{
			if (m_isDetected == false)
				m_eCurState = STATE_IDLE;
			else
				m_eCurState = STATE_ATTACK_READY;
			m_fAttackTime = 0.f;
			Delete_Attack_Lazer();
		}
		break;

	case Client::CGuardTower::STATE_DIE:
		if (m_pModelCom->IsAnimationFinished(ANIM_DIE))
		{
			m_fDeadTime += fTimeDelta;
		}


		break;
	}
}

void CGuardTower::View_Detect_Range(_float fTimeDelta)
{
	_float fRadian = fTimeDelta;
	_float fDistance = 11.5f;
	m_EffectMatrix = _mat::CreateScale(30.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * _mat::CreateRotationY(XMConvertToRadians(fRadian)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(0.f, 0.1f, 0.f);
	m_LazerMatrix = _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(0.f, 3.f, 0.f);
	
	_vec4 vTowerLook = m_EffectMatrix.Up().Get_Normalized();
	//_vec4 vLazerLook = Compute_Player_To_Dir(m_LazerMatrix.Position()).Get_Normalized();

	//m_LazerMatrix.Up(Compute_PlayerPos());
	Compute_Lazer_Dir();

	_float fDetectRadian = XMConvertToDegrees(acosf(vTowerLook.Dot(Compute_Player_To_Dir(m_pTransformCom->Get_State(State::Pos)))));

	PxRaycastBuffer pBuffer{};

		if (fDetectRadian < 22.5f && Compute_PlayerDistance() <= fDistance)
		{
			if (!m_pGameInstance->Raycast(m_pTransformCom->Get_CenterPos(), vTowerLook, fDistance, pBuffer))
			{
				if (pBuffer.block.distance > Compute_PlayerDistance())
				{
					m_isDetected = true;
				}
				else
					m_isDetected = false;
			}
			else
				m_isDetected = false;
		}
		else
		{
			m_isDetected = false;
		}
	


	if (fRadian >= 360.f)
	{
		m_bInit = true;
	}
}

_vec4 CGuardTower::Compute_PlayerPos()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	//return pPlayerTransform->Get_State(State::Pos);
	return pPlayerTransform->Get_CenterPos();
}

_vec4 CGuardTower::Compute_PlayerLook()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	return pPlayerTransform->Get_State(State::Look).Get_Normalized();
}

_float CGuardTower::Compute_PlayerDistance()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);

	_float fDistance = (vPlayerPos - vPos).Length();

	return fDistance;
}

_vec4 CGuardTower::Compute_Player_To_Dir(_vec4 vPos)
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	_vec4 vDir = (vPlayerPos - vPos).Get_Normalized();

	return vDir;
}

void CGuardTower::Compute_Lazer_Dir()
{
	_vec3 m_vLazerDir{};
	if(m_eCurState == STATE_ATTACK_READY)
		m_vLazerDir = (Compute_PlayerPos() - m_LazerMatrix.Position()).Get_Normalized();
	else if(m_eCurState == STATE_ATTACK)
		m_vLazerDir = (m_vCurPlayerPos - m_LazerMatrix.Position()).Get_Normalized();


	_vec3 vUp = m_vLazerDir;
	_vec3 vRight = vUp.Cross(_vec3(0.f, 0.f, 1.f)).Get_Normalized();
	_vec3 vLook = vRight.Cross(vUp).Get_Normalized();

	m_LazerMatrix.Right(vRight);
	m_LazerMatrix.Up(vUp);
	m_LazerMatrix.Look(vLook);

	return;
}

void CGuardTower::Create_Range()
{
	if (!m_pFrameEffect)
	{
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_45_Frame");
		Info.pMatrix = &m_EffectMatrix;
		Info.isFollow = true;
		m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
	}

	if (!m_pBaseEffect)
	{
		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_45_Base");
		Info.pMatrix = &m_EffectMatrix;
		Info.isFollow = true;
		m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
	}
}


void CGuardTower::Create_Lazer()
{
	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Cannon_Laser_Warning");
	Info.pMatrix = &m_LazerMatrix;
	Info.isFollow = true;
	m_pThreatEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
}

void CGuardTower::Create_Attack_Lazer()
{
	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Cannon_Laser");
	Info.pMatrix = &m_LazerMatrix;
	Info.isFollow = true;
	Info.fLifeTime = 0.1f;
	m_pAttackEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
}

void CGuardTower::Delete_Range()
{
	if (m_pFrameEffect)
	{
		m_pFrameEffect->Kill();
		m_pFrameEffect = nullptr;
	}

	if (m_pBaseEffect)
	{
		m_pBaseEffect->Kill();
		m_pBaseEffect = nullptr;
	}
}
void CGuardTower::Delete_Lazer()
{
	if (m_pThreatEffect)
	{
		m_pThreatEffect->Kill();
		m_pThreatEffect = nullptr;
	}
}

void CGuardTower::Delete_Attack_Lazer()
{
	if (m_pAttackEffect)
	{
		m_pAttackEffect->Kill();
		m_pAttackEffect = nullptr;
	
	}

}

HRESULT CGuardTower::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_GuardTower"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CGuardTower::Bind_ShaderResources()
{
	if (m_iPassIndex == AnimPass_OutLine)
	{
		_uint iColor = OutlineColor_Red;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_OutlineColor", &iColor, sizeof iColor)))
		{
			return E_FAIL;
		}

	}

	if (m_iPassIndex == AnimPass_Rim && m_bChangePass == true)
	{
		_vec4 vColor = Colors::Red;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &vColor, sizeof vColor)))
		{
			return E_FAIL;
		}
	}

	if (m_iPassIndex == AnimPass_Dissolve)
	{
		if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
		{
			return E_FAIL;
		}

		m_fDissolveRatio += 0.02f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissolveRatio, sizeof _float)))
		{
			return E_FAIL;
		}

		_bool bHasNorTex = true;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &bHasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

	}

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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CGuardTower::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(1.f, 2.5f, 1.f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider", L"Com_Collider_OBB", (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
		return E_FAIL;

	return S_OK;
}

void CGuardTower::Update_Collider()
{
	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());

}

CGuardTower* CGuardTower::Create(_dev pDevice, _context pContext)
{
	CGuardTower* pInstance = new CGuardTower(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CGuardTower");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGuardTower::Clone(void* pArg)
{
	CGuardTower* pInstance = new CGuardTower(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CGuardTower");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGuardTower::Free()
{
	__super::Free();
	//if (!m_isPrototype)
	//{
	CUI_Manager::Get_Instance()->Delete_RadarPos(CUI_Manager::MONSTER, m_pTransformCom);
	//}
	//m_pGameInstance->Delete_CollisionObject(this);

	//Delete_Range();
	//Delete_Lazer();
	//Delete_Attack_Lazer();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pBaseEffect);
	Safe_Release(m_pFrameEffect);
	Safe_Release(m_pThreatEffect);
	Safe_Release(m_pAttackEffect);
	Safe_Release(m_pBodyColliderCom);
	Safe_Release(m_pDissolveTextureCom);
}
