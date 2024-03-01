#include "Balloon.h"
#include "UI_Manager.h"
#include "Event_Manager.h"
#include "Effect_Manager.h"
#include "Camera_Manager.h"
#include "GlowCube.h"

CBalloon::CBalloon(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBalloon::CBalloon(const CBalloon& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBalloon::Init_Prototype()
{
	return S_OK;
}

HRESULT CBalloon::Init(void* pArg)
{
	m_vColor = ((BALLOON_DESC*)pArg)->vColor;
	_vec3 vPos = ((BALLOON_DESC*)pArg)->vPosition;
	m_pTransformCom->Set_Position(vPos);

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}
	Set_RandomColor();


	m_Animation.iAnimIndex = Idle;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 2.f;

	m_eCurState = STATE_IDLE;

	m_pTransformCom->Set_Scale(_vec3(1.4f, 1.4f, 1.4f));

	CGlowCube::GLOWCUBE_DESC Desc{};
	Desc.pParentTransform = m_pTransformCom;
	Desc.vColor = _vec4(m_vColor);
	Desc.vPos = _vec4(0.f, 0.f, 0.f, 0.f);
	m_pCube = (CGlowCube*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_GlowCube"), &Desc);
	if (m_pCube == nullptr)
	{
		return E_FAIL;
	}
	Desc.isDefault = true;
	m_pDefCube = (CGlowCube*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_GlowCube"), &Desc);
	if (m_pDefCube == nullptr)
	{
		return E_FAIL;
	}
	
	
	return S_OK;
}

void CBalloon::Tick(_float fTimeDelta)
{

	m_pTransformCom->Set_Scale(_vec3(3.f, 3.f, 3.f));

	m_pBodyColliderCom->Change_Extents(_vec3(0.35f, 0.35f, 0.35f));
	m_pBodyColliderCom->Set_Normal();
	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);
	if (m_pCube != nullptr)
	{
		m_pDefCube->Tick(fTimeDelta);
		m_pCube->Tick(fTimeDelta);
	}
	m_pModelCom->Set_Animation(m_Animation);

	Update_BodyCollider();

	m_pTransformCom->Gravity(fTimeDelta);


}

void CBalloon::Late_Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
	//m_pDefCube->Late_Tick(fTimeDelta);
	m_pCube->Late_Tick(fTimeDelta);
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
#endif
}

HRESULT CBalloon::Render()
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

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof _vec4)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(AnimPass_Color)))
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

void CBalloon::Init_State(_float fTimeDelta)
{

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CBalloon::STATE_IDLE:
			m_Animation.iAnimIndex = Idle;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.2f;
			m_Animation.fInterpolationTime = 0.5f;

			m_pTransformCom->Set_Speed(3.f);
			m_bDamaged = false;
			break;

		case Client::CBalloon::STATE_HIT:
			m_Animation.iAnimIndex = NodetreeAction;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 3.f;

			break;

		case Client::CBalloon::STATE_DIE:
			m_Animation.iAnimIndex = die;
			m_Animation.isLoop = false;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CBalloon::Tick_State(_float fTimeDelta)
{
	m_isColl = false;
	switch (m_eCurState)
	{
	case Client::CBalloon::STATE_IDLE:
	{

		CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_TOWER, TEXT("Layer_BrickGame"), TEXT("BrickBall"));
		if (pCollider == nullptr)
		{
			return;
		}

		m_isColl = m_pBodyColliderCom->Intersect(pCollider);
		if (m_isColl)
		{
			_mat Mat = _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));

			EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"LightningTube_White");
			Info.pMatrix = &Mat;
			CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

			CCamera_Manager::Get_Instance()->Set_ShakeCam(true, 2.0f);
			m_eCurState = STATE_HIT;
			if (CUI_Manager::Get_Instance()->Get_BrickBallColor() == m_eCurColor)
			{
				
			}
		}
	}
	break;

	case Client::CBalloon::STATE_HIT:

		if (m_pModelCom->IsAnimationFinished(m_Animation.iAnimIndex))
		{
			m_eCurState = STATE_IDLE;
			_uint iColor = (_uint)m_eCurColor + 1;
			m_eCurColor = (BrickColor)iColor;
		}

		break;

	case Client::CBalloon::STATE_DIE:

		if (m_pModelCom->IsAnimationFinished(die))
		{
			m_isDead = true;
		}

		break;
	}

	Set_Color();
}

void CBalloon::Set_Color()
{
	switch (m_eCurColor)
	{
	case PINK:
		//m_vColor = _vec4(0.38f, 0.235f, 0.f, 1.f);
		m_vColor = _vec4(0.27f, 0.14f, 0.36f, 1.f);
		break;
	case YELLOW:
		//m_vColor = _vec4(0.5f, 0.42f, 0.f, 1.f);
		m_vColor = _vec4(0.143f, 0.267f, 0.321f, 1.f);
		break;
	case PURPLE:
		m_vColor = _vec4(0.27f, 0.14f, 0.36f, 1.f);
		break;
	case BLUE:
		m_vColor = _vec4(0.143f, 0.267f, 0.321f, 1.f);
		break;
	case COLOR_END:
		if (m_eCurState != STATE_DIE)
		{
			m_eCurState = STATE_DIE;
			m_isDead = true;
		}
		break;
	default:
		break;
	}
	if (m_pCube != nullptr)
	{
		m_pDefCube->Set_Color(m_vColor);
		m_pCube->Set_Color(m_vColor);
	}
}

void CBalloon::Set_RandomColor()
{
	_uint iRandom = rand() % 100;

	if (iRandom < 50)
	{
		m_eCurColor = BLUE;
	}
	else if (iRandom < 70)
	{
		m_eCurColor = PURPLE;
	}
	else if (iRandom < 80)
	{
		m_eCurColor = PINK;
	}
	else
	{
		m_eCurColor = YELLOW;
	}
	Set_Color();
}

HRESULT CBalloon::Add_Collider()
{
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::AABB;
	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(0.8f, 0.8f, 0.8f);
	CollDesc.vCenter = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Sphere"), (CComponent**)&m_pBodyColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}


void CBalloon::Update_BodyCollider()
{
	m_pBodyColliderCom->Update(m_pTransformCom->Get_World_Matrix());
}

HRESULT CBalloon::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Balloon"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_T_EFF_Noise_04_BC"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CBalloon::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CBalloon* CBalloon::Create(_dev pDevice, _context pContext)
{
	CBalloon* pInstance = new CBalloon(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBalloon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBalloon::Clone(void* pArg)
{
	CBalloon* pInstance = new CBalloon(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBalloon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBalloon::Free()
{
	__super::Free();

	Safe_Release(m_pCube);
	Safe_Release(m_pDefCube);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pBodyColliderCom);

	Safe_Release(m_pDissolveTextureCom);
}
