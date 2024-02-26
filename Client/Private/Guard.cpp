#include "Guard.h"
#include "UI_Manager.h"


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
	if (FAILED(Add_Components()))
		return E_FAIL;

	CUI_Manager::Get_Instance()->Set_RadarPos(CUI_Manager::MONSTER, m_pTransformCom);

	m_Animation.iAnimIndex;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 1.5f;

	m_iHP = 1;

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 1.2f; // ����(�� �Ʒ��� �ݱ� ũ�� ����
	ControllerDesc.radius = 0.4f; // ���Ʒ� �ݱ��� ������
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // �� ����
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // ĳ���Ͱ� ���� �� �ִ� �ִ� ����
	ControllerDesc.contactOffset = 0.1f; // ĳ���Ϳ� �ٸ� ��ü���� �浹�� �󸶳� ���� ��������. ���� Ŭ���� �� ���� ���������� ���ɿ� ���� ���� �� ����.
	ControllerDesc.stepOffset = 0.2f; // ĳ���Ͱ� ���� �� �ִ� ����� �ִ� ����

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);


	return S_OK;
}

void CGuard::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_OldMatrix();

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


}

void CGuard::Late_Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
#endif // _DEBUG

}

HRESULT CGuard::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i) {
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuaseTexture", i, TextureType::Diffuse)))
			continue;

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
	if (m_ePreState != m_eCurState) {
		switch (m_eCurState)
		{
		case Client::CGuard::STATE_IDLE:
			m_Animation.iAnimIndex;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio;

			m_pTransformCom->Set_Speed(1.f);
			break;
		case Client::CGuard::STATE_PATROL:
			m_Animation.iAnimIndex;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 4.f;
			break;
		case Client::CGuard::STATE_CHASE:
			break;
		case Client::CGuard::STATE_ATTACK:
			break;
		case Client::CGuard::STATE_HIT:
			break;
		case Client::CGuard::STATE_DIE:
			break;
		}

		m_ePreState = m_eCurState;
	}

}

void CGuard::Tick_State(_float fTimeDelta)
{
}


HRESULT CGuard::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_T_EFF_Noise_04_BC"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CGuard::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OldWorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
		return E_FAIL;

	if(FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof(_float2))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pModelCom->Bind_Animation(m_pShaderCom)))
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Prototype_Component_Collider",
		L"Com_Collider_OBB", (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
		return E_FAIL;

	// Frustum
	Collider_Desc ColDesc{};
	ColDesc.eType = ColliderType::Frustum;
	_matrix matView = XMMatrixLookAtLH(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	_matrix matProj = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.f), 0.5f, 0.01f, 1.5f);

	ColDesc.matFrustum = matView * matProj;

	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider_Attack"), reinterpret_cast<CComponent**>(&m_pAttackColliderCom), &ColDesc)))
	//{
	//	return E_FAIL;
	//}

	return S_OK;
}

void CGuard::Update_Collider()
{
}

CGuard* CGuard::Create(_dev pDevice, _context pContext)
{
	CGuard* pInstance = new CGuard(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CVoid01");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGuard::Clone(void* pArg)
{
	CGuard* pInstance = new CGuard(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVoid01");
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

	Safe_Release(m_pBodyColliderCom);
	Safe_Release(m_pDissolveTextureCom);
}
