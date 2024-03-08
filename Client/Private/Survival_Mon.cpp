#include "Survival_Mon.h"

#include "Effect_Dummy.h"
#include "Effect_Manager.h"

CSurvival_Mon::CSurvival_Mon(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CSurvival_Mon::CSurvival_Mon(const CSurvival_Mon& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSurvival_Mon::Init_Prototype()
{
	return S_OK;
}

HRESULT CSurvival_Mon::Init(void* pArg)
{
	m_eType = *(SURVIVAL_MON_TYPE*)pArg;

	_vec3 vCenterPos = _vec3(-2000.f, 0.f, 2000.f);

	random_device dev;
	_randNum RandomNumber(dev());
	_randFloat Random = _randFloat(-1.f, 1.f);

	switch (m_eType)
	{
	case Client::CSurvival_Mon::TYPE_IMP:

		m_strModelTag = TEXT("Prototype_Model_Imp");

		m_Animation.iAnimIndex = ROAR;
		m_Animation.isLoop = false;
		m_Animation.fAnimSpeedRatio = 2.f;

		m_pTransformCom->Set_Scale(_vec3(1.f));

		_vec3 vRandomDir = _vec3(Random(RandomNumber), 0.f, Random(RandomNumber)).Get_Normalized();

		m_pTransformCom->Set_Position(vCenterPos + static_cast<_float>(rand() % 8 + 1) * vRandomDir); // Radius : 8
		m_pTransformCom->Set_Speed(5.f);

		m_iPassIndex = AnimPass_Dissolve;



		break;
	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	m_pTransformCom->LookAt(vPlayerPos);

	m_pGameInstance->Play_Sound(TEXT("Bat_Roar_SFX_03"));

	return S_OK;
}

void CSurvival_Mon::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_LEFT))
	{
		Kill();
	}

	m_pTransformCom->Set_OldMatrix();

	if (m_fDissolveRatio <= 0.f)
	{
		m_iPassIndex = AnimPass_OutLine;
	}

	switch (m_eType)
	{
	case Client::CSurvival_Mon::TYPE_IMP:

	{
		CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
		_vec3 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
		_vec3 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
		vDir.y = 0.f;
		_float fDistance = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Length();

		m_UpdateMatrix = _mat::CreateScale(m_fCircleRange) * _mat::CreateRotationX(XMConvertToRadians(90.f))
			* _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));


		if (m_pModelCom->IsAnimationFinished(ROAR))
		{
			m_bSpawned = true;
		}

		if (m_bSpawned == true)
		{
			if (fDistance >= 2.f && m_bExplode == false)
			{
				m_Animation.iAnimIndex = RUN;
				m_Animation.isLoop = true;

				m_pTransformCom->LookAt_Dir(vDir);
				m_pTransformCom->Go_Straight(fTimeDelta);
			}

			else
			{
				m_Animation.iAnimIndex = ATTACK01_H;
				m_Animation.isLoop = false;
				m_Animation.fDurationRatio = 0.371f;
				m_Animation.fAnimSpeedRatio = 2.2f;

				m_bExplode = true;

				if (!m_bCreateEffect)
				{
					_mat FrameMatrix = _mat::CreateScale(3.7f) * _mat::CreateRotationX(XMConvertToRadians(90.f))
						* _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));

					EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Frame");
					Info.pMatrix = &FrameMatrix;
					m_pFrameEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

					m_UpdateMatrix = _mat::CreateScale(m_fCircleRange) * _mat::CreateRotationX(XMConvertToRadians(90.f))
						* _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));

					Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_Circle_Base");
					Info.pMatrix = &m_UpdateMatrix;
					Info.isFollow = true;
					m_pBaseEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

					m_bCreateEffect = true;
				}

				if (m_pFrameEffect && m_pBaseEffect)
				{
					if (m_fCircleRange <= 3.75f)
					{
						m_fCircleRange += 0.1f;
					}

					m_pFrameEffect->Tick(fTimeDelta);
					m_pBaseEffect->Tick(fTimeDelta);
				}
			}
		}

		if (m_pModelCom->IsAnimationFinished(ATTACK01_H))
		{
			Kill();

			if (m_pFrameEffect && m_pBaseEffect)
			{
				m_pGameInstance->Play_Sound(TEXT("Bat_Hit_Voice_02"));

				Safe_Release(m_pFrameEffect);
				Safe_Release(m_pBaseEffect);
			}

			// ÀÌÆåÆ® Ãß°¡
			_mat FrameMatrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos) + _vec3(0.f, 0.1f, 0.f)));

			EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Explode_Mesh_Purple");
			Info.pMatrix = &FrameMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Survival_Explode_Sphere_Purple");
			Info.pMatrix = &FrameMatrix;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			if (fDistance <= 1.8f)
			{
				m_pGameInstance->Attack_Player(nullptr, rand() % 10, MonAtt_Hit);
			}
		}

	}

	break;
	}

	m_pModelCom->Set_Animation(m_Animation);
}

void CSurvival_Mon::Late_Tick(_float fTimeDelta)
{
	if (m_pFrameEffect && m_pBaseEffect)
	{
		m_pFrameEffect->Late_Tick(fTimeDelta);
		m_pBaseEffect->Late_Tick(fTimeDelta);
	}

	m_pModelCom->Play_Animation(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

}

HRESULT CSurvival_Mon::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			_bool bFailed = true;
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

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(m_iPassIndex)))
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

HRESULT CSurvival_Mon::Add_Components()
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

	return S_OK;
}

HRESULT CSurvival_Mon::Bind_ShaderResources()
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

	if (m_iPassIndex == AnimPass_OutLine)
	{
		_uint iColor = {};

		switch (m_eType)
		{
		case Client::CSurvival_Mon::TYPE_IMP:
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

CSurvival_Mon* CSurvival_Mon::Create(_dev pDevice, _context pContext)
{
	CSurvival_Mon* pInstance = new CSurvival_Mon(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSurvival_Mon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSurvival_Mon::Clone(void* pArg)
{
	CSurvival_Mon* pInstance = new CSurvival_Mon(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSurvival_Mon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSurvival_Mon::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pDissolveTextureCom);

	Safe_Release(m_pFrameEffect);
	Safe_Release(m_pBaseEffect);
}
