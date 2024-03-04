#include "Launcher.h"

#include "Projectile.h"
#include "Effect_Dummy.h"
#include "Effect_Manager.h"
#include "Camera_Manager.h"

_uint CLauncher::m_iLauncherID = 0;
_uint CLauncher::m_iDestroyCount = 0;

CLauncher::CLauncher(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CLauncher::CLauncher(const CLauncher& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLauncher::Init_Prototype()
{
	return S_OK;
}

HRESULT CLauncher::Init(void* pArg)
{
	m_eType = *(LAUNCHER_TYPE*)pArg;

	//_vec3 vCenterPos = _vec3(-2000.f, 0.f, 2000.f);

	switch (m_eType)
	{
	case Client::CLauncher::TYPE_RANDOM_POS:
	{
		m_strModelTag = TEXT("Prototype_Model_LokiStone");
		m_Animation.iAnimIndex = 0;
		m_Animation.isLoop = true;
		m_Animation.fAnimSpeedRatio = 2.f;

		m_pTransformCom->Set_Scale(_vec3(0.7f));

		random_device dev;
		_randNum RandomNumber(dev());
		_randFloat Random = _randFloat(-1.f, 1.f);

		_vec3 vRandomDir = _vec3(Random(RandomNumber), 0.f, Random(RandomNumber)).Get_Normalized();

		m_pTransformCom->Set_Position(CENTER_POS + static_cast<_float>(rand() % 8 + 1) * vRandomDir); // Radius : 8

		m_iPassIndex = AnimPass_Dissolve;
	}

	break;

	case Client::CLauncher::TYPE_CANNON:

		// Anim
		// Attack01
		// Attack02
		// Die
		// Idle

	{
		m_strModelTag = TEXT("Prototype_Model_Cannon");
		m_Animation.iAnimIndex = 1;
		m_Animation.isLoop = false;
		m_Animation.fStartAnimPos = 115.f;

		m_pTransformCom->Set_Position(CENTER_POS);
		m_pTransformCom->Set_Speed(1.f);

		m_iPassIndex = AnimPass_Dissolve;

		Collider_Desc BodyCollDesc = {};
		BodyCollDesc.eType = ColliderType::OBB;
		BodyCollDesc.vExtents = _vec3(0.5f, 0.5f, 15.f);
		BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
		BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider_OBB"), (CComponent**)&m_pColliderCom, &BodyCollDesc)))
			return E_FAIL;

		_mat Matrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)) /*+ _vec3(0.f, 0.3f, 0.f)*/);
		EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Cannon_Parti");
		Info.pMatrix = &Matrix;
		m_pLauncher = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

		m_iDestroyCount = 0;
	}

	break;

	case Client::CLauncher::TYPE_BLUEGEM:

	{
		m_strModelTag = TEXT("Prototype_Model_BlueGem");
		m_Animation.iAnimIndex = 0;
		m_Animation.isLoop = true;
		m_Animation.fAnimSpeedRatio = 2.f;

		m_iPassIndex = AnimPass_Dissolve;

		m_pTransformCom->Set_Scale(_vec3(0.25f));

		switch (m_iLauncherID)
		{
		case 0:
			m_pTransformCom->Set_Position(CENTER_POS + _vec3(9.f, 6.f, 0.f));
			break;
		case 1:
			m_pTransformCom->Set_Position(CENTER_POS + _vec3(0.f, 6.f, 9.f));
			break;
		case 2:
			m_pTransformCom->Set_Position(CENTER_POS + _vec3(-9.f, 6.f, 0.f));
			break;
		case 3:
			m_pTransformCom->Set_Position(CENTER_POS + _vec3(0.f, 6.f, -9.f));
			break;
		}

		Collider_Desc BodyCollDesc = {};
		BodyCollDesc.eType = ColliderType::AABB;
		BodyCollDesc.vExtents = _vec3(2.f, 3.f, 2.f);
		BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
			TEXT("Com_Collider_AABB"), (CComponent**)&m_pColliderCom, &BodyCollDesc)))
			return E_FAIL;

		m_pGameInstance->Register_CollisionObject(this, m_pColliderCom);

		++m_iLauncherID;

		if (m_iLauncherID >= 4)
		{
			m_iLauncherID = 0;
		}

		_mat Matrix = _mat::CreateScale(1.2f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)) + _vec3(0.f, 0.3f, 0.f));
		EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_BlueGem_Shield");
		Info.pMatrix = &Matrix;
		m_pLauncher = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	}

	break;

	case Client::CLauncher::TYPE_BARRICADE:

		// Anim
		// Destroy
		// Idle
		// Spawn

	{
		m_strModelTag = TEXT("Prototype_Model_Barricade");
		m_Animation.iAnimIndex = 2;
		m_Animation.isLoop = false;
		m_Animation.fAnimSpeedRatio = 2.f;
		//m_Animation.fDurationRatio = 0.001f;

		m_iPassIndex = AnimPass_Default;

		m_pTransformCom->Set_Scale(_vec3(0.15f));

		switch (m_iLauncherID)
		{
		case 0:
			m_pTransformCom->Set_Position(CENTER_POS + _vec3(5.f, 0.f, 0.f));
			break;
		case 1:
			m_pTransformCom->Set_Position(CENTER_POS + _vec3(5.f, 0.f, -7.5f));
			break;
		case 2:
			m_pTransformCom->Set_Position(CENTER_POS + _vec3(5.f, 0.f, 7.5f));
			break;
		case 3:
			m_pTransformCom->Set_Position(CENTER_POS + _vec3(-5.f, 0.f, 0.f));
			m_pTransformCom->LookAt_Dir(_vec4(0.f, 0.f, -1.f, 0.f));
			break;
		case 4:
			m_pTransformCom->Set_Position(CENTER_POS + _vec3(-5.f, 0.f, -7.5f));
			m_pTransformCom->LookAt_Dir(_vec4(0.f, 0.f, -1.f, 0.f));
			break;
		case 5:
			m_pTransformCom->Set_Position(CENTER_POS + _vec3(-5.f, 0.f, 7.5f));
			m_pTransformCom->LookAt_Dir(_vec4(0.f, 0.f, -1.f, 0.f));
			break;
		}

		++m_iLauncherID;

		if (m_iLauncherID >= 6)
		{
			m_iLauncherID = 0;
		}


	}

	break;

	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}



	return S_OK;
}

void CLauncher::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_OldMatrix();

	if (m_pGameInstance->Key_Pressing(DIK_DOWN))
	{
		Kill();
	}

	m_fTime += fTimeDelta;
	m_fProjectileCreateTime += fTimeDelta;

	switch (m_eType)
	{
	case Client::CLauncher::TYPE_RANDOM_POS:

		if (m_fDissolveRatio <= 0.f)
		{
			m_iPassIndex = AnimPass_Rim;
		}

		if (m_fTime >= 2.5f)
		{
			if (m_fProjectileCreateTime >= 1.5f)
			{
				if (m_iProjectileCount <= 3)
				{
					for (size_t i = 0; i < 8; i++)
					{
						CProjectile::PROJECTILE_DESC Desc = {};
						Desc.eType = CProjectile::TYPE_MISSILE;
						Desc.pLauncherTransform = m_pTransformCom;
						Desc.vStartPos = m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.2f, 0.f);

						m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Projectile"), TEXT("Prototype_GameObject_Projectile"), &Desc);
					}

					++m_iProjectileCount;
				}

				else
				{
					Kill();
				}

				m_fProjectileCreateTime = 0.f;
			}
		}

		m_pModelCom->Set_Animation(m_Animation);

		break;

	case Client::CLauncher::TYPE_CANNON:

	{
		if (m_pLauncher)
		{
			m_pLauncher->Tick(fTimeDelta);
		}

		if (m_fDissolveRatio <= 0.f)
		{
			m_iPassIndex = AnimPass_Rim;
		}

		if (m_bDestroy == true)
		{
			m_pTransformCom->Go_Down(fTimeDelta);

			if (m_pTransformCom->Get_State(State::Pos).y <= -3.f)
			{
				Kill();
			}
		}

		if (m_iDestroyCount >= 4)
		{
			Safe_Release(m_pLauncher);

			m_Animation.iAnimIndex = 2;
			m_Animation.fAnimSpeedRatio = 2.f;

			if (m_pModelCom->IsAnimationFinished(2))
			{
				m_bDestroy = true;
			}

		}

		CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
		_vec3 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
		_vec3 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
		vDir.y = 0.f;

		if (m_pModelCom->IsAnimationFinished(1))
		{
			m_Animation.iAnimIndex = 3;
		}


		if (m_iPassIndex == AnimPass_Rim)
		{
			if (m_Animation.iAnimIndex == 3)
			{
				m_Animation.fAnimSpeedRatio = 6.f;

				m_pTransformCom->LookAt_Dir(vDir);
			}
			else if (m_Animation.iAnimIndex == 0)
			{
				m_Animation.fAnimSpeedRatio = 1.7f;
				m_Animation.fStartAnimPos = 12.f;

				if (m_pModelCom->Get_CurrentAnimPos() >= 20.f && m_pModelCom->Get_CurrentAnimPos() <= 85.f)
				{
					if (m_fTime >= 0.3f)
					{
						_uint iDamage = 30 + rand() % 10;
						m_pGameInstance->Attack_Player(m_pColliderCom, iDamage, MonAtt_Hit);

						m_fTime = 0.f;
					}
				}
			}

			if (m_pModelCom->IsAnimationFinished(3))
			{
				m_Animation.iAnimIndex = 0;
			}
			else if (m_pModelCom->IsAnimationFinished(0))
			{
				m_Animation.iAnimIndex = 3;
				++m_iProjectileCount;
			}
		}


		_mat Offset = _mat::CreateTranslation(0.f, 3.f, 18.f);
		m_pColliderCom->Update(Offset * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_World_Matrix());

		m_pModelCom->Set_Animation(m_Animation);

	}

	break;

	case Client::CLauncher::TYPE_BLUEGEM:

		if (m_pLauncher)
		{
			m_pLauncher->Tick(fTimeDelta);
		}

		if (m_fDissolveRatio <= 0.f)
		{
			if (!m_bDestroy)
			{
				m_iPassIndex = AnimPass_OutLine;
			}
			else
			{
				m_iPassIndex = AnimPass_Default;
			}
		}

		if (!m_bDestroy)
		{
			m_pTransformCom->Turn(_vec4(0.f, 1.f, 0.f, 0.f), fTimeDelta);
			m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());
		}
		else
		{
			if (m_pLauncher)
			{
				Safe_Release(m_pLauncher);
			}

			m_pTransformCom->Go_Down(fTimeDelta);
		}

		if (m_pTransformCom->Get_State(State::Pos).y <= -5.f)
		{
			Kill();
			m_pGameInstance->Delete_CollisionObject(this);
		}

		if (!m_bDestroy)
		{
			m_pModelCom->Set_Animation(m_Animation);
		}
		else
		{
			m_pDestroyModelCom->Set_Animation(m_Animation);
		}

		break;

	case Client::CLauncher::TYPE_BARRICADE:

		if (m_fTime >= 1.f)
		{
		}

		_float fAnimPos = m_pModelCom->Get_CurrentAnimPos();
		m_pModelCom->Set_Animation(m_Animation);

		break;

	}
}

void CLauncher::Late_Tick(_float fTimeDelta)
{
	switch (m_eType)
	{
	case Client::CLauncher::TYPE_RANDOM_POS:
		m_pModelCom->Play_Animation(fTimeDelta);

		break;

	case Client::CLauncher::TYPE_CANNON:

		if (m_pLauncher)
		{
			m_pLauncher->Late_Tick(fTimeDelta);
		}

		m_pModelCom->Play_Animation(fTimeDelta);

		break;
	case Client::CLauncher::TYPE_BLUEGEM:

		if (m_pLauncher)
		{
			m_pLauncher->Late_Tick(fTimeDelta);
		}

		if (m_bDestroy == true)
		{
			m_pDestroyModelCom->Play_Animation(fTimeDelta);
		}
		else
		{
			m_pModelCom->Play_Animation(fTimeDelta);
		}

		break;

	case Client::CLauncher::TYPE_BARRICADE:

		m_pModelCom->Play_Animation(fTimeDelta);

		break;

	}

	if (m_eType == CLauncher::TYPE_BARRICADE)
	{
		if (m_pModelCom->Get_CurrentAnimPos() >= 1.f)
		{
			m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

		}

	}
	else
	{
		m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

	}

#ifdef _DEBUG
	if (m_pColliderCom)
	{
		m_pRendererCom->Add_DebugComponent(m_pColliderCom);
	}
#endif

}

HRESULT CLauncher::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	CModel* pModel = { nullptr };
	pModel = m_pModelCom;

	if (m_eType == CLauncher::TYPE_BLUEGEM)
	{
		if (m_bDestroy == true)
		{
			pModel = m_pDestroyModelCom;
		}
		else
		{
			pModel = m_pModelCom;
		}
	}

	for (_uint i = 0; i < pModel->Get_NumMeshes(); i++)
	{
		if (FAILED(pModel->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			_bool bFailed = true;
		}

		_bool HasNorTex{};
		if (FAILED(pModel->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
		{
			HasNorTex = false;
		}
		else
		{
			HasNorTex = true;
		}

		_bool HasMaskTex{};
		if (FAILED(pModel->Bind_Material(m_pShaderCom, "g_MaskTexture", i, TextureType::Shininess)))
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

		if (FAILED(pModel->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(m_iPassIndex)))
		{
			return E_FAIL;
		}

		if (FAILED(pModel->Render(i)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

void CLauncher::Set_Damage(_int iDamage, _uint iDamageType)
{
	if (iDamage > 0)
	{
		if (m_eType == CLauncher::TYPE_BLUEGEM)
		{
			m_bDestroy = true;

			m_Animation.iAnimIndex = 0;
			m_Animation.isLoop = false;

			m_pTransformCom->Set_Speed(7.f);

			CCamera_Manager::Get_Instance()->Set_ShakeCam(true, 0.5f);

			++m_iDestroyCount;

			//m_pGameInstance->Delete_CollisionObject(this);
			//Safe_Release(m_pColliderCom);
		}
	}
}

HRESULT CLauncher::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strModelTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_T_EFF_Noise_04_BC"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
	{
		return E_FAIL;
	}

	switch (m_eType)
	{
	case Client::CLauncher::TYPE_RANDOM_POS:
		break;

	case Client::CLauncher::TYPE_CANNON:
		break;

	case Client::CLauncher::TYPE_BLUEGEM:

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_BlueGem_Destroy"), TEXT("Com_DestroyModel"), reinterpret_cast<CComponent**>(&m_pDestroyModelCom), m_pTransformCom)))
		{
			return E_FAIL;
		}

		break;

	case Client::CLauncher::TYPE_BARRICADE:
		break;
	}

	return S_OK;
}

HRESULT CLauncher::Bind_ShaderResources()
{
	if (m_iPassIndex == AnimPass_Dissolve)
	{
		if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
		{
			return E_FAIL;
		}

		m_fDissolveRatio -= 0.02f;
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

	else if (m_iPassIndex == AnimPass_Rim)
	{
		_vec4 vColor = {};

		switch (m_eType)
		{
		case Client::CLauncher::TYPE_RANDOM_POS:
			vColor = Colors::Gold;
			break;
		case Client::CLauncher::TYPE_CANNON:
			vColor = Colors::Cyan;
			break;
		case Client::CLauncher::TYPE_BLUEGEM:
			break;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &vColor, sizeof vColor)))
		{
			return E_FAIL;
		}
	}

	else if (m_iPassIndex == AnimPass_OutLine)
	{
		_uint iColor = {};

		switch (m_eType)
		{
		case Client::CLauncher::TYPE_BLUEGEM:
			iColor = OutlineColor_Red;
			break;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_OutlineColor", &iColor, sizeof iColor)))
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
	{
		return E_FAIL;
	}

	return S_OK;
}

CLauncher* CLauncher::Create(_dev pDevice, _context pContext)
{
	CLauncher* pInstance = new CLauncher(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CLauncher");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLauncher::Clone(void* pArg)
{
	CLauncher* pInstance = new CLauncher(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CLauncher");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLauncher::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pDestroyModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);

	Safe_Release(m_pDissolveTextureCom);

	Safe_Release(m_pLauncher);
}
