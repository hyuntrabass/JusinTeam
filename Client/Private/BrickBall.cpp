#include "BrickBall.h"
#include "Effect_Dummy.h"
#include "Effect_Manager.h"

_uint CBrickBall::m_iMissileID = 0;

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



	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec3 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
	m_pTransformCom->Set_Position(vPlayerPos);

	m_pTransformCom->Set_Scale(_vec3(1.5f, 1.5f, 1.5f));

	m_EffectMatrix = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Brick_Ball_Init");
	Info.pMatrix = &m_EffectMatrix;
	Info.isFollow = true;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

	return S_OK;
}

void CBrickBall::Tick(_float fTimeDelta)
{
	/*
	
	if (m_bParryingOk)
	{
		m_pGameInstance->Set_TimeRatio(1.0f);
		m_pTransformCom->Go_Straight(fTimeDelta);

		m_fLifeTime += fTimeDelta;
		if (m_fLifeTime >= 1.f)
		{
			Kill();
		}

		m_EffectMatrix = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

		if (m_pEffect_Ball)
		{
			m_pEffect_Ball->Tick(fTimeDelta);
		}

		return;
	}


	switch (m_eType)
	{
	case Client::CBrickBall::LEFT_THROW:
	{
		if (m_pGameInstance->CheckCollision_Parrying(m_pColliderCom))
		{
			Parry_Success();
			return;
		}
		else if (m_pGameInstance->Attack_Player(m_pColliderCom, 30 + rand() % 10))
		{
			_uint iSoundIndex = rand() % 4 + 1;
			wstring strSoundTag = TEXT("Hit_Large_Acid_SFX_0") + to_wstring(iSoundIndex);
			m_pGameInstance->Play_Sound(strSoundTag);
			Kill();
		}
		else if (m_fLifeTime >= 1.f)
		{
			Kill();
		}

		if (!m_bShoot && m_pGroarModel->Get_CurrentAnimationIndex() == CGroar_Boss::MON_GROAR_ASGARD_ATTACK01 &&
			m_pGroarModel->Get_CurrentAnimPos() >= 51.f)
		{
			m_bShoot = true;

			EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Groar_Ball_Smoke");
			Info.pMatrix = &m_EffectMatrix;
			Info.isFollow = true;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
		}


		if (m_bShoot == true)
		{
			m_pTransformCom->Go_Straight(fTimeDelta);
			m_fLifeTime += fTimeDelta;
		}
		else
		{
			_mat vLeftHandMatrix = *(m_pGroarModel->Get_BoneMatrix("Bip002-L-Finger2")) * m_pGroarTransform->Get_World_Matrix();
			m_pTransformCom->Set_State(State::Pos, vLeftHandMatrix.Position());

			CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
			_vec4 vPlayerPos = pPlayerTransform->Get_CenterPos();
			m_pTransformCom->LookAt(vPlayerPos);

		}

	}
	break;

	case Client::CBrickBall::RIGHT_THROW:
	{
		if (m_pGameInstance->CheckCollision_Parrying(m_pColliderCom))
		{
			Parry_Success();
			return;
		}
		else if (m_pGameInstance->Attack_Player(m_pColliderCom, 30 + rand() % 10))
		{
			_uint iSoundIndex = rand() % 4 + 1;
			wstring strSoundTag = TEXT("Hit_Large_Acid_SFX_0") + to_wstring(iSoundIndex);
			m_pGameInstance->Play_Sound(strSoundTag);
			Kill();
		}
		else if (m_fLifeTime >= 1.f)
		{
			Kill();
		}

		if (!m_bShoot && m_pGroarModel->Get_CurrentAnimationIndex() == CGroar_Boss::MON_GROAR_ASGARD_ATTACK00 &&
			m_pGroarModel->Get_CurrentAnimPos() >= 38.f)
		{
			m_bShoot = true;

			EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Groar_Ball_Smoke");
			Info.pMatrix = &m_EffectMatrix;
			Info.isFollow = true;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
		}


		if (m_bShoot == true)
		{
			m_pTransformCom->Go_Straight(fTimeDelta);
			m_fLifeTime += fTimeDelta;
		}
		else
		{
			_mat vRightHandMatrix = *(m_pGroarModel->Get_BoneMatrix("Bip002-R-Finger2")) * m_pGroarTransform->Get_World_Matrix();
			m_pTransformCom->Set_State(State::Pos, vRightHandMatrix.Position());

			CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
			_vec4 vPlayerPos = pPlayerTransform->Get_CenterPos();
			m_pTransformCom->LookAt(vPlayerPos);
		}

	}
	break;

	case Client::CBrickBall::SIX_MISSILE:
		if (m_pGameInstance->CheckCollision_Parrying(m_pColliderCom))
		{
			Parry_Success();
			return;
		}
		else if (m_pGameInstance->Attack_Player(m_pColliderCom, 30 + rand() % 10))
		{
			_uint iSoundIndex = rand() % 4 + 1;
			wstring strSoundTag = TEXT("Hit_Large_Acid_SFX_0") + to_wstring(iSoundIndex);
			m_pGameInstance->Play_Sound(strSoundTag);
			Kill();
		}
		else if (m_fLifeTime >= 1.f)
		{
			Kill();
		}

		if (m_fEffectTimer > 0.8f and m_fEffectTimer < 2.f)
		{
			EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Groar_Ball_Smoke");
			Info.pMatrix = &m_EffectMatrix;
			Info.isFollow = true;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			m_fEffectTimer = 3.f;
		}

		m_fEffectTimer += fTimeDelta;

		if (!m_bShoot && m_pGroarModel->Get_CurrentAnimationIndex() == CGroar_Boss::MON_GROAR_ASGARD_ATTACK02 &&
			m_pGroarModel->Get_CurrentAnimPos() >= m_fDepartTime)
		{
			m_bShoot = true;
		}

		if (m_bShoot == true)
		{
			m_pTransformCom->Go_Straight(fTimeDelta);
			m_fLifeTime += fTimeDelta;
		}
		else
		{
			CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
			_vec4 vPlayerPos = pPlayerTransform->Get_CenterPos();
			m_pTransformCom->LookAt(vPlayerPos);

			_vec4 vGroarUp = m_pGroarTransform->Get_State(State::Up).Get_Normalized();
			_vec3 vGroarLook = m_pGroarTransform->Get_State(State::Look).Get_Normalized();
			_vec4 vGroarPos = m_pGroarTransform->Get_State(State::Pos);

			_vec4 vDir = {};

			switch (m_iMissileIndex)
			{
			case 1:
				vDir = _vec4::Transform(vGroarUp, _mat::CreateFromAxisAngle(vGroarLook, XMConvertToRadians(60.f)));
				break;

			case 2:
				vDir = _vec4::Transform(vGroarUp, _mat::CreateFromAxisAngle(vGroarLook, XMConvertToRadians(-60.f)));
				break;

			case 3:
				vDir = _vec4::Transform(vGroarUp, _mat::CreateFromAxisAngle(vGroarLook, XMConvertToRadians(40.f)));
				break;

			case 4:
				vDir = _vec4::Transform(vGroarUp, _mat::CreateFromAxisAngle(vGroarLook, XMConvertToRadians(-40.f)));
				break;

			case 5:
				vDir = _vec4::Transform(vGroarUp, _mat::CreateFromAxisAngle(vGroarLook, XMConvertToRadians(20.f)));
				break;

			case 6:
				vDir = _vec4::Transform(vGroarUp, _mat::CreateFromAxisAngle(vGroarLook, XMConvertToRadians(-20.f)));
				break;
			}

			m_pTransformCom->Set_State(State::Pos, vGroarPos + 6 * vDir);
		}

		break;
	}

	RayCast();

	if (m_isDead)
	{
		EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Groar_Ball_Explosion");
		Info.pMatrix = &m_EffectMatrix;
		CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
	}

	m_EffectMatrix = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

	if (m_pEffect_Ball)
	{
		m_pEffect_Ball->Tick(fTimeDelta);
	}

	Update_Collider();*/
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
	//if (FAILED(Bind_ShaderResources()))
	//{
	//	return E_FAIL;
	//}

	//_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	//for (_uint i = 0; i < iNumMeshes; i++)
	//{
	//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
	//	{
	//		return E_FAIL;
	//	}

	//	_bool HasNorTex{};
	//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
	//	{
	//		HasNorTex = false;
	//	}
	//	else
	//	{
	//		HasNorTex = true;
	//	}

	//	if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
	//	{
	//		return E_FAIL;
	//	}

	//	if (FAILED(m_pShaderCom->Begin(StaticPass_Default)))
	//	{
	//		return E_FAIL;
	//	}

	//	if (FAILED(m_pModelCom->Render(i)))
	//	{
	//		return E_FAIL;
	//	}


	//}

	return S_OK;
}

HRESULT CBrickBall::Add_Collider()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.vCenter = _vec3(0.f);
	CollDesc.fRadius = 0.2f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Sphere"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

void CBrickBall::Update_Collider()
{
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

void CBrickBall::Parry_Success()
{
	_uint random = rand() % 2;
	_float RandFloat{ 1.f };
	m_fLifeTime = 0.f;
	if (random == 0)
	{
		RandFloat = -1.f;
	}

	_uint randomsound = rand() % 3;
	if (randomsound == 0) 
	{
	m_pGameInstance->Play_Sound(TEXT("Parry1"), 0.6f);
	}
	else if (randomsound == 1)
	{
		m_pGameInstance->Play_Sound(TEXT("Parry2"), 0.6f);
	}
	else
	{
		m_pGameInstance->Play_Sound(TEXT("Parry3"), 0.6f);
	}

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"hitSpark_Right");
	_mat EffectMat = m_pTransformCom->Get_World_Matrix();
	Info.pMatrix = &EffectMat;
	CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);
	m_pTransformCom->Rotation(_vec4(0.f, 1.f, 0.f, 0.f), 45.f * RandFloat);
	m_pGameInstance->Set_TimeRatio(0.05f);
	m_bParryingOk = true;
}

void CBrickBall::RayCast()
{
	_float fDist = 0.5f;
	PxRaycastBuffer Buffer{};

	if (m_pGameInstance->Raycast(m_pTransformCom->Get_State(State::Pos), 
		m_pTransformCom->Get_State(State::Look).Get_Normalized(),
		fDist, Buffer))
	{
		Kill();
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

	EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Groar_Ball");
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
