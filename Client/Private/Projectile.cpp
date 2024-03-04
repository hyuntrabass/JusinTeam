#include "Projectile.h"

#include "Effect_Dummy.h"
#include "Effect_Manager.h"

_uint CProjectile::m_iProjectileID = 0;

CProjectile::CProjectile(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CProjectile::CProjectile(const CProjectile& rhs)
	: CGameObject(rhs)
{
}

HRESULT CProjectile::Init_Prototype()
{
	return S_OK;
}

HRESULT CProjectile::Init(void* pArg)
{
	m_ProjectileDesc = *(PROJECTILE_DESC*)pArg;

	switch (m_ProjectileDesc.eType)
	{
	case Client::CProjectile::TYPE_MISSILE:

	{
		m_UpdateMatrix = _mat::CreateScale(0.5f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

		EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_Yellow");
		Info.pMatrix = &m_UpdateMatrix;
		Info.isFollow = true;
		m_pBall = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_Out_Yellow");
		Info.pMatrix = &m_UpdateMatrix;
		Info.isFollow = true;
		m_pBallOut = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_Parti_Yellow");
		Info.pMatrix = &m_UpdateMatrix;
		Info.isFollow = true;
		m_pBallParticle = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		_vec4 vLauncherLook = m_ProjectileDesc.pLauncherTransform->Get_State(State::Look).Get_Normalized();
		_vec4 vDir = _vec4::Transform(vLauncherLook, _mat::CreateRotationY(XMConvertToRadians(45.f * m_iProjectileID)));

		m_pTransformCom->Set_Position(m_ProjectileDesc.vStartPos + _vec3(0.f, 1.f, 0.f));
		m_pTransformCom->LookAt_Dir(vDir);

		++m_iProjectileID;

		if (m_iProjectileID == 8)
		{
			m_iProjectileID = 0;
		}
	}
	break;

	case Client::CProjectile::TYPE_FLOOR:

	{
		m_pTransformCom->Set_Position(m_ProjectileDesc.vStartPos);

		_mat Matrix = _mat::CreateScale(2.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)) + _vec3(0.f, 0.f, 0.f));

		EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_Blue2");
		Info.pMatrix = &Matrix;
		CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

		Matrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)) + _vec3(0.f, 0.5f, 0.f));

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_Parti_Blue");
		Info.pMatrix = &Matrix;
		CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);


		_mat FrameMatrix = _mat::CreateScale(5.f) * _mat::CreateRotationX(XMConvertToRadians(90.f))
			* _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Frame");
		Info.pMatrix = &FrameMatrix;
		m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		m_UpdateMatrix = _mat::CreateScale(m_fCircleRange) * _mat::CreateRotationX(XMConvertToRadians(90.f))
			* _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Base");
		Info.pMatrix = &m_UpdateMatrix;
		Info.isFollow = true;
		m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	}

	break;

	case Client::CProjectile::TYPE_GUIDED_MISSILE:

	{
		m_pTransformCom->Set_Position(m_ProjectileDesc.vStartPos);

		m_UpdateMatrix = _mat::CreateScale(0.75f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)) + _vec3(0.f, 1.f, 0.f));

		EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_White");
		Info.pMatrix = &m_UpdateMatrix;
		Info.isFollow = true;
		m_pBall = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
		_vec3 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
		_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
		vDir.y = 0.f;

		m_pTransformCom->LookAt_Dir(vDir);
		m_pTransformCom->Set_Speed(rand() % 10 + 5);
	}

	break;

	case Client::CProjectile::TYPE_TANGHURU:

	{
		m_pTransformCom->Set_Position(m_ProjectileDesc.vStartPos + _vec3(0.f, -1.f, 0.f));

		m_UpdateMatrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)) /*+ _vec3(0.f, 1.f, 0.f)*/);

		EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_Galaxy");
		Info.pMatrix = &m_UpdateMatrix;
		Info.isFollow = true;
		m_pBall = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_Thorn");
		Info.pMatrix = &m_UpdateMatrix;
		Info.isFollow = true;
		m_pEffect[0] = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_Thorn2");
		Info.pMatrix = &m_UpdateMatrix;
		Info.isFollow = true;
		m_pEffect[1] = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_Out_Galaxy");
		Info.pMatrix = &m_UpdateMatrix;
		Info.isFollow = true;
		m_pEffect[2] = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		_mat Matrix = _mat::CreateScale(1.2f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)) + _vec3(0.f, 1.1f, 0.f));

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_Floor_Galaxy");
		Info.pMatrix = &Matrix;
		//Info.isFollow = false;
		m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);
	}

	break;

	case Client::CProjectile::TYPE_SPEAR:
	{
		m_pTransformCom->Set_Speed(1.f);
		m_pTransformCom->Set_Position(m_ProjectileDesc.vStartPos);

		m_UpdateMatrix = _mat::CreateScale(1.f) * _mat::CreateRotationY(XMConvertToRadians(90.f)) 
			* _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

		EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Spear");
		Info.pMatrix = &m_UpdateMatrix;
		Info.isFollow = true;
		m_pEffect[0] = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		m_ProjectileDesc.vStartPos.y = 0.f;

		_mat FrameMatrix = _mat::CreateScale(3.f) * _mat::CreateRotationX(XMConvertToRadians(90.f))
			* _mat::CreateTranslation(_vec3(m_ProjectileDesc.vStartPos + _vec3(0.f, 0.1f, 0.f)));

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Frame");
		Info.pMatrix = &FrameMatrix;
		m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Dim");
		Info.pMatrix = &FrameMatrix;
		m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);


	}
		break;
	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CProjectile::Tick(_float fTimeDelta)
{
	m_fTime += fTimeDelta;

	switch (m_ProjectileDesc.eType)
	{
	case Client::CProjectile::TYPE_MISSILE:

		if (m_fTime >= 2.f)
		{
			Kill();
			Safe_Release(m_pBall);
			Safe_Release(m_pBallOut);
			Safe_Release(m_pBallParticle);
		}

		m_pTransformCom->Go_Straight(fTimeDelta);

		m_UpdateMatrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

		if (m_pBall && m_pBallParticle)
		{
			m_pBall->Tick(fTimeDelta);
			m_pBallOut->Tick(fTimeDelta);
			m_pBallParticle->Tick(fTimeDelta);
		}

		m_pGameInstance->Attack_Player(m_pColliderCom, rand() % 6, MonAtt_Hit);

		break;

	case Client::CProjectile::TYPE_FLOOR:

	{
		CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
		_vec3 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
		_float fDistance = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Length();

		m_UpdateMatrix = _mat::CreateScale(m_fCircleRange) * _mat::CreateRotationX(XMConvertToRadians(90.f))
			* _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));

		//if (m_fTime >= 1.5f)
		if (m_fCircleRange >= 5.f)
		{
			if (m_pBaseEffect && m_pFrameEffect)
			{
				Safe_Release(m_pFrameEffect);
				Safe_Release(m_pBaseEffect);

				_mat Matrix = _mat::CreateScale(2.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)) + _vec3(0.f, 0.5f, 0.f));
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_Explode_Blue");
				Info.pMatrix = &Matrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				Matrix = _mat::CreateScale(0.8f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)) + _vec3(0.f, 1.f, 0.f));
				Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_Explode_Mesh_Blue");
				Info.pMatrix = &Matrix;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				if (fDistance <= 2.f)
				{
					m_pGameInstance->Attack_Player(nullptr, rand() % 10, MonAtt_Hit);
				}

				//Matrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)) + _vec3(0.f, 1.f, 0.f));
				//Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Ball_Explode_Parti_Blue");
				//Info.pMatrix = &Matrix;
				//CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			}

			m_fTime = 0.f;
		}

		if (m_pBaseEffect && m_pFrameEffect)
		{
			m_fCircleRange += 0.1f;

			m_pBaseEffect->Tick(fTimeDelta);
			m_pFrameEffect->Tick(fTimeDelta);
		}
	}

		break;

	case Client::CProjectile::TYPE_GUIDED_MISSILE:

		if (m_fTime >= 5.f)
		{
			Kill();
			Safe_Release(m_pBall);
			//Safe_Release(m_pBallParticle);
		}

		m_pTransformCom->Go_Straight(fTimeDelta);

		m_UpdateMatrix = _mat::CreateScale(0.75f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)) + _vec3(0.f, 1.f, 0.f));

		if (m_pBall /*&& m_pBallParticle*/)
		{
			m_pBall->Tick(fTimeDelta);
			//m_pBallParticle->Tick(fTimeDelta);
		}

		m_pGameInstance->Attack_Player(m_pColliderCom, rand() % 6, MonAtt_Hit);

		break;
	case Client::CProjectile::TYPE_TANGHURU:

		if (m_fTime >= 4.f)
		{
			Kill();
			Safe_Release(m_pBall);
			Safe_Release(m_pEffect[0]);
			Safe_Release(m_pEffect[1]);
			Safe_Release(m_pEffect[2]);
			Safe_Release(m_pFrameEffect);
		}

		if (m_pFrameEffect)
		{
			m_pFrameEffect->Tick(fTimeDelta);
		}

		if (m_fTime >= 1.f)
		{
			if (m_pBall)
			{
				m_pBall->Tick(fTimeDelta);
				m_pEffect[0]->Tick(fTimeDelta);
				m_pEffect[1]->Tick(fTimeDelta);
				m_pEffect[2]->Tick(fTimeDelta);
			}

			if (m_pTransformCom->Get_State(State::Pos).y <= 1.f)
			{
				m_pTransformCom->Go_Up(fTimeDelta);
			}
		}

		m_UpdateMatrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)) /*+ _vec3(0.f, 1.f, 0.f)*/);

		m_pGameInstance->Attack_Player(m_pColliderCom, rand() % 6, MonAtt_Hit);

		break;

	case Client::CProjectile::TYPE_SPEAR:
	{
		if (m_pTransformCom->Get_State(State::Pos).y <= 3.5f)
		{
			m_pTransformCom->Set_Speed(30.f);
		}		
		
		m_pTransformCom->Go_Down(fTimeDelta);

		m_pGameInstance->Attack_Player(m_pColliderCom, rand() % 10, MonAtt_Hit);

		if (m_pTransformCom->Get_State(State::Pos).y <= -5.f)
		{
			Kill();
			Safe_Release(m_pFrameEffect);
			Safe_Release(m_pBaseEffect);
			Safe_Release(m_pEffect[0]);
			
			break;
		}

		if (m_pFrameEffect && m_pBaseEffect)
		{
			m_pBaseEffect->Tick(fTimeDelta);
			m_pFrameEffect->Tick(fTimeDelta);
	
			m_pEffect[0]->Tick(fTimeDelta);
		}

		m_UpdateMatrix = _mat::CreateScale(1.f) * _mat::CreateRotationY(XMConvertToRadians(90.f))
			* _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

	}

	break;

	}

	if (m_pColliderCom)
	{
		m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
	}

}

void CProjectile::Late_Tick(_float fTimeDelta)
{
	switch (m_ProjectileDesc.eType)
	{
	case Client::CProjectile::TYPE_MISSILE:

		if (m_pBall && m_pBallParticle)
		{
			m_pBall->Late_Tick(fTimeDelta);
			m_pBallOut->Late_Tick(fTimeDelta);
			m_pBallParticle->Late_Tick(fTimeDelta);
		}

		break;

	case Client::CProjectile::TYPE_FLOOR:

		if (m_pFrameEffect && m_pBaseEffect)
		{
			m_pFrameEffect->Late_Tick(fTimeDelta);
			m_pBaseEffect->Late_Tick(fTimeDelta);
		}

		break;

	case Client::CProjectile::TYPE_GUIDED_MISSILE:

		if (m_pBall/* && m_pBallParticle*/)
		{
			m_pBall->Late_Tick(fTimeDelta);
			//m_pBallParticle->Late_Tick(fTimeDelta);
		}

		break;
	case Client::CProjectile::TYPE_TANGHURU:
		if (m_pBall)
		{
			m_pBall->Late_Tick(fTimeDelta);
			m_pEffect[0]->Late_Tick(fTimeDelta);
			m_pEffect[1]->Late_Tick(fTimeDelta);
			m_pEffect[2]->Late_Tick(fTimeDelta);
			m_pFrameEffect->Late_Tick(fTimeDelta);
		}

		break;

	case Client::CProjectile::TYPE_SPEAR:
	{
		if (m_pFrameEffect && m_pBaseEffect)
		{
			m_pFrameEffect->Late_Tick(fTimeDelta);
			m_pBaseEffect->Late_Tick(fTimeDelta);
			m_pEffect[0]->Late_Tick(fTimeDelta);
		}

	}
	break;

	}

#ifdef _DEBUG
	if (m_pColliderCom)
	{
		m_pRendererCom->Add_DebugComponent(m_pColliderCom);
	}
#endif

}

HRESULT CProjectile::Render()
{
	return S_OK;
}

HRESULT CProjectile::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	switch (m_ProjectileDesc.eType)
	{
	case Client::CProjectile::TYPE_MISSILE:
	{
		Collider_Desc CollDesc = {};
		CollDesc.eType = ColliderType::Sphere;
		CollDesc.vCenter = _vec3(0.f);
		CollDesc.fRadius = 0.3f;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider_Sphere"), (CComponent**)&m_pColliderCom, &CollDesc)))
		{
			return E_FAIL;
		}
	}
	break;

	case Client::CProjectile::TYPE_FLOOR:

	{

	}

	break;

	case Client::CProjectile::TYPE_GUIDED_MISSILE:

	{
		Collider_Desc CollDesc = {};
		CollDesc.eType = ColliderType::Sphere;
		CollDesc.vCenter = _vec3(0.f);
		CollDesc.fRadius = 0.3f;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider_Sphere"), (CComponent**)&m_pColliderCom, &CollDesc)))
		{
			return E_FAIL;
		}

	}

	break;

	case Client::CProjectile::TYPE_TANGHURU:

	{
		Collider_Desc CollDesc = {};
		CollDesc.eType = ColliderType::Sphere;
		CollDesc.vCenter = _vec3(0.f);
		CollDesc.fRadius = 0.3f;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider_Sphere"), (CComponent**)&m_pColliderCom, &CollDesc)))
		{
			return E_FAIL;
		}
	}

	break;

	case Client::CProjectile::TYPE_SPEAR:

	{
		Collider_Desc CollDesc = {};
		CollDesc.eType = ColliderType::Sphere;
		CollDesc.vCenter = _vec3(0.f, 0.2f, 0.f);
		CollDesc.fRadius = 0.2f;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider_Sphere"), (CComponent**)&m_pColliderCom, &CollDesc)))
		{
			return E_FAIL;
		}
	}

	break;

	}


	return S_OK;
}

HRESULT CProjectile::Bind_ShaderResources()
{
	return S_OK;
}

CProjectile* CProjectile::Create(_dev pDevice, _context pContext)
{
	CProjectile* pInstance = new CProjectile(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CProjectile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CProjectile::Clone(void* pArg)
{
	CProjectile* pInstance = new CProjectile(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CProjectile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CProjectile::Free()
{
	__super::Free();

	Safe_Release(m_pBall);
	Safe_Release(m_pBallOut);
	Safe_Release(m_pBallParticle);

	Safe_Release(m_pFrameEffect);
	Safe_Release(m_pBaseEffect);

	Safe_Release(m_pEffect[0]);
	Safe_Release(m_pEffect[1]);
	Safe_Release(m_pEffect[2]);

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
}
