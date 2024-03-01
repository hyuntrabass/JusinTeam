#include "TowerLazer.h"
#include "GuardTower.h"
#include "UI_Manager.h"
#include "Effect_Dummy.h"


CTowerLazer::CTowerLazer(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CTowerLazer::CTowerLazer(const CTowerLazer& rhs)
	: CGameObject(rhs)
	, EffectMatrix(rhs.EffectMatrix)
{
}

HRESULT CTowerLazer::Init_Prototype()
{
	return S_OK;
}

HRESULT CTowerLazer::Init(void* pArg)
{
	_mat mLazerMatrix = *(_mat*)pArg
	
	if (FAILED(Add_Components()))
		return E_FAIL;

	if (FAILED(Add_Collider()))
		return E_FAIL;

	m_pTransformCom->Set_Matrix(mLazerMatrix);

	Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Range_45_Frame");
	Info.pMatrix = &EffectMatrix;
	Info.isFollow = true;

	m_pEffect = CEffect_Manager::Get_Instance()->Clone_Effect(Info);

	EffectInfo Effect = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"TorchFire_Dun");

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 1.2f; // 높이(위 아래의 반구 크기 제외
	ControllerDesc.radius = 0.4f; // 위아래 반구의 반지름
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // 업 방향
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // 캐릭터가 오를 수 있는 최대 각도
	ControllerDesc.contactOffset = 0.1f; // 캐릭터와 다른 물체와의 충돌을 얼마나 먼저 감지할지. 값이 클수록 더 일찍 감지하지만 성능에 영향 있을 수 있음.
	ControllerDesc.stepOffset = 0.2f; // 캐릭터가 오를 수 있는 계단의 최대 높이

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	return S_OK;
}

void CTowerLazer::Tick(_float fTimeDelta)
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

	if (m_pFrameEffect)
		m_pFrameEffect->Tick(fTimeDelta);
	if (m_pBaseEffect)
		m_pBaseEffect->Tick(fTimeDelta);

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

void CTowerLazer::Late_Tick(_float fTimeDelta)
{
	if (m_pFrameEffect)
		m_pFrameEffect->Late_Tick(fTimeDelta);

	if (m_pBaseEffect)
		m_pBaseEffect->Late_Tick(fTimeDelta);

	if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_State(State::Pos), 20.f))
	{
		m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
	}

	m_pModelCom->Play_Animation(fTimeDelta);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);

#endif // _DEBUG

}

HRESULT CTowerLazer::Render()
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

_vec4 CTowerLazer::Compute_PlayerPos()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	return pPlayerTransform->Get_State(State::Pos);
}

_vec4 CTowerLazer::Compute_PlayerLook()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	return pPlayerTransform->Get_State(State::Look).Get_Normalized();
}

_float CTowerLazer::Compute_PlayerDistance()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);

	_float fDistance = (vPlayerPos - vPos).Length();

	return fDistance;
}

_vec4 CTowerLazer::Compute_Player_To_Dir()
{
	CTransform* pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec4 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	_vec4 vPos = m_pTransformCom->Get_State(State::Pos);
	//_vec4 vPos = m_pModelCom->Get_CenterPos();

	_vec4 vDir = (vPlayerPos - vPos).Get_Normalized();

	return vDir;
}

HRESULT CTowerLazer::Add_Components()
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


	return S_OK;
}

HRESULT CTowerLazer::Bind_ShaderResources()
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

HRESULT CTowerLazer::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(1.f, 2.5f, 1.f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider", L"Com_Collider_OBB", (CComponent**)&m_pColliderCom, &BodyCollDesc)))
		return E_FAIL;

	return S_OK;
}

void CTowerLazer::Update_Collider()
{
	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());

}

CTowerLazer* CTowerLazer::Create(_dev pDevice, _context pContext)
{
	CTowerLazer* pInstance = new CTowerLazer(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTowerLazer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTowerLazer::Clone(void* pArg)
{
	CTowerLazer* pInstance = new CTowerLazer(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTowerLazer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTowerLazer::Free()
{
	__super::Free();
	__super::Free();
	//if (!m_isPrototype)
	//{
	//	CUI_Manager::Get_Instance()->Delete_RadarPos(CUI_Manager::MONSTER, m_pTransformCom);
	//}

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pEffect);
	Safe_Release(m_pBodyColliderCom);
	Safe_Release(m_pDissolveTextureCom);
}
