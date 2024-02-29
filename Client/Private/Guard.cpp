#include "Guard.h"
#include "UI_Manager.h"
#include "Effect_Dummy.h"


CGuard::CGuard(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CGuard::CGuard(const CGuard& rhs)
	: CGameObject(rhs)
{
}

HRESULT CGuard::Init_Prototype()
{
	return S_OK;
}

HRESULT CGuard::Init(void* pArg)
{
	GuardInfo m_Info = *(GuardInfo*)pArg;
	m_iIndex = m_Info.iIndex;
	GuardMatrix = m_Info.mMatrix;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Collider()))
		return E_FAIL;

	m_Animation.iAnimIndex = ANIM_IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 1.5f;
	m_eCurState = STATE_PATROL;

	m_iHP = 1;

	m_pTransformCom->Set_Matrix(GuardMatrix);

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 1.2f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.4f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	View_Detect_Range();

	return S_OK;
}

void CGuard::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_OldMatrix();

	if (m_pGameInstance->Key_Down(DIK_DOWN))
	{
		m_Animation.iAnimIndex++;
	}

	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);

	_vec4 vToPlayer = vPlayerPos - vPos;

	_float fDist = vToPlayer.Length();
	_vec3 vNormalToPlayer = vToPlayer.Get_Normalized();

	_vec3 vLook = m_pTransformCom->Get_State(State::Look).Get_Normalized();

	_float fAngle = vLook.Dot(vNormalToPlayer);

	switch (m_eCurState)
	{
	case Client::CGuard::STATE_IDLE:

		break;
	case Client::CGuard::STATE_PATROL:


		fAngle = XMConvertToDegrees(fAngle);
		if (60.f >= fAngle && 10.f >= fDist) {
			PxRaycastBuffer Buffer{};
			if (m_pGameInstance->Raycast(m_pTransformCom->Get_CenterPos(), vNormalToPlayer, 10.f, Buffer)) {
				if (fDist < Buffer.block.distance)
					m_eCurState = STATE_CHASE;
			}
			else
				m_eCurState = STATE_CHASE;
		}
		break;
	case Client::CGuard::STATE_CHASE:
		break;
	case Client::CGuard::STATE_ATTACK:
		break;
	case Client::CGuard::STATE_HIT:
		break;
	case Client::CGuard::STATE_DIE:
		break;
	case Client::CGuard::STATE_END:
		break;
	}


	if (true == m_bChangePass) {
		m_fHitTime += fTimeDelta;

		if (0.3f <= m_fHitTime) {
			m_fHitTime = 0.f;
			m_bChangePass = false;
		}
	}

	if (0 >= m_iHP || 0.01f < m_fDeadTime) {
		m_pGameInstance->Delete_CollisionObject(this);
		m_pTransformCom->Delete_Controller();
	}

	if (1.f <= m_fDissolveRatio)
		Kill();

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	m_pTransformCom->Gravity(fTimeDelta);
	Update_Collider();

}

void CGuard::Late_Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
	m_pRendererCom->Add_DebugComponent(m_pAttackColliderCom);
#endif // _DEBUG

}

HRESULT CGuard::Render()
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

		_bool HasGlowTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_GlowTexture", i, TextureType::Specular)))
		{
			HasGlowTex = false;
		}
		else
		{
			HasGlowTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasGlowTex", &HasGlowTex, sizeof _bool)))
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

	return S_OK;
}

void CGuard::Set_Damage(_int iDamage, _uint iDamageType)
{
	m_fHittedTime = 6.f;

	m_iHP -= iDamage;
	m_bChangePass = true;

	CUI_Manager::Get_Instance()->Set_HitEffect(m_pTransformCom, iDamage, _vec2(0.f, 3.f), (ATTACK_TYPE)iDamageType);

}

void CGuard::Init_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = Compute_PlayerPos();
	_float fDistance = Compute_PlayerDistance();
	_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();

	if (m_ePreState != m_eCurState) {
		switch (m_eCurState)
		{
		case Client::CGuard::STATE_IDLE:
			m_Animation.iAnimIndex = ANIM_IDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio;

			break;
		case Client::CGuard::STATE_PATROL:
			m_Animation.iAnimIndex = ANIM_WALK;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 1.f;
			m_pTransformCom->Set_Speed(1.f);

			break;
		case Client::CGuard::STATE_CHASE:
			m_Animation.iAnimIndex = ANIM_RUN;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 4.f;
			break;
		case Client::CGuard::STATE_ATTACK:
			m_Animation.iAnimIndex = ANIM_ATTACK_1;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 4.f;
			break;
		case Client::CGuard::STATE_DIE:
			m_Animation.iAnimIndex = ANIM_DIE;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;
			break;
		}

		m_ePreState = m_eCurState;
	}

}

void CGuard::Tick_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = Compute_PlayerPos();
	_float fDistance = Compute_PlayerDistance();

	_float fDist = 1.0f;
	PxRaycastBuffer Buffer{};
	
	switch (m_eCurState)
	{
	case STATE_IDLE:
		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 1.f)
		{
			m_eCurState = STATE_PATROL;

		}
			m_fIdleTime = 0.f;
		
		break;

	case STATE_PATROL:

		if (m_pGameInstance->Raycast(m_pTransformCom->Get_CenterPos(), m_pTransformCom->Get_State(State::Look).Get_Normalized(), fDist, Buffer))
		{
			m_pTransformCom->LookAt_Dir(PxVec3ToVector(Buffer.block.normal));
		}
		else
		{
			m_pTransformCom->Go_Straight(fTimeDelta);
		}

		if (m_pModelCom->IsAnimationFinished(ANIM_WALK))
		{
			m_eCurState = STATE_IDLE;
		}

		break;

	case STATE_DIE:
		if (m_pModelCom->IsAnimationFinished(ANIM_DIE))
		{
			m_fDeadTime += fTimeDelta;
		}

		break;
	}
}

void CGuard::View_Detect_Range()
{
	Safe_Release(m_pBaseEffect);
	Safe_Release(m_pFrameEffect);

	_mat EffectMatrix{};
	EffectInfo Info{};

	EffectMatrix = _mat::CreateScale(30.f) * _mat::CreateRotationX(XMConvertToRadians(90.f)) * m_pTransformCom->Get_World_Matrix() * _mat::CreateTranslation(_vec3(0.f, 0.25f, 0.f));

	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_45_Frame");
	Info.pMatrix = &EffectMatrix;
	m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_45_Base");
	Info.pMatrix = &EffectMatrix;
	m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

}

_vec4 CGuard::Compute_PlayerPos()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	return pPlayerTransform->Get_State(State::Pos);
}

_vec4 CGuard::Compute_PlayerLook()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	return pPlayerTransform->Get_State(State::Look).Get_Normalized();
}

_float CGuard::Compute_PlayerDistance()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);

	_float fDistance = (vPlayerPos - vPos).Length();

	return fDistance;
}


HRESULT CGuard::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Guard"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CGuard::Bind_ShaderResources()
{
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

HRESULT CGuard::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(0.3f, 1.f, 0.2f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider", L"Com_Collider_OBB", (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
		return E_FAIL;

	// Frustum
	Collider_Desc AttackColDesc{};
	AttackColDesc.eType = ColliderType::Frustum;
	_matrix matView = XMMatrixLookAtLH(m_pTransformCom->Get_State(State::Pos), m_pTransformCom->Get_State(State::Look), _vec3(0.f,1.f,0.f));
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 0.5f, 0.01f, 1.5f);

	AttackColDesc.matFrustum = matView * matProj;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &AttackColDesc)))
	{
		return E_FAIL;
	}


	return S_OK;
}

void CGuard::Update_Collider()
{
	_mat Offset = _mat::CreateTranslation(0.f, 2.f, 0.f);
	m_pAttackColliderCom->Update(Offset * m_pTransformCom->Get_World_Matrix());
	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());
	
}

CGuard* CGuard::Create(_dev pDevice, _context pContext)
{
	CGuard* pInstance = new CGuard(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CGuard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGuard::Clone(void* pArg)
{
	CGuard* pInstance = new CGuard(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CGuard");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGuard::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pBaseEffect);
	Safe_Release(m_pFrameEffect);
	Safe_Release(m_pBodyColliderCom);
	Safe_Release(m_pAttackColliderCom);
	Safe_Release(m_pDissolveTextureCom);
}
