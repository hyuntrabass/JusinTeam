#include "Void19.h"

CVoid19::CVoid19(_dev pDevice, _context pContext)
	:CVTFMonster(pDevice, pContext)
{
}

CVoid19::CVoid19(const CVoid19& rhs)
	:CVTFMonster(rhs)
{
}

HRESULT CVoid19::Init_Prototype()
{
	return S_OK;
}

HRESULT CVoid19::Init(void* pArg)
{
	m_pTransformCom->Set_State(State::Pos, _vec4(0.f, -1000.f, 0.f, 1.f));
	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 1.2f; // ����(�� �Ʒ��� �ݱ� ũ�� ����
	ControllerDesc.radius = 0.5f; // ���Ʒ� �ݱ��� ������
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // �� ����
	ControllerDesc.slopeLimit = cosf(PxDegToRad(1.f)); // ĳ���Ͱ� ���� �� �ִ� �ִ� ����
	ControllerDesc.contactOffset = 0.1f; // ĳ���Ϳ� �ٸ� ��ü���� �浹�� �󸶳� ���� ��������. ���� Ŭ���� �� ���� ���������� ���ɿ� ���� ���� �� ����.
	ControllerDesc.stepOffset = 0.01f; // ĳ���Ͱ� ���� �� �ִ� ����� �ִ� ����
	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pGameInstance->Register_CollisionObject(this, m_pColliderCom);

	m_iHP = 500;

	m_eState = State_Idle;
	m_Animation.iAnimIndex = Anim_roar;

	return S_OK;
}

void CVoid19::Tick(_float fTimeDelta)
{
	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_World_Matrix());

	__super::Tick(fTimeDelta);
}

void CVoid19::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);

#endif // DEBUG
}

HRESULT CVoid19::Render()
{
	__super::Render();

	return S_OK;
}

HRESULT CVoid19::Render_Instance()
{
	__super::Render_Instance();

	return S_OK;
}

void CVoid19::Init_State(_float fTimeDelta)
{
	if (m_IsHitted == true)
	{
		m_fHitTime += fTimeDelta;

		m_pShaderCom->Set_PassIndex(1);

		if (m_fHitTime >= 0.3f)
		{
			m_fHitTime = 0.f;
			m_IsHitted = false;
			m_pShaderCom->Set_PassIndex(0);
		}
	}

	if (m_eState != m_ePreState)
	{
		m_Animation = {};
		m_Animation.fAnimSpeedRatio = 2.f;

		switch (m_eState)
		{
		case Client::CVoid19::State_Idle:
			m_Animation.iAnimIndex = Anim_roar;
			break;
		case Client::CVoid19::State_Attack:
			m_Animation.iAnimIndex = Anim_attack01;
			break;
		case Client::CVoid19::State_Die:
			m_Animation.iAnimIndex = Anim_stun;
			m_Animation.fDurationRatio = 0.1f;

			m_pTransformCom->Delete_Controller();
			break;
		}

		m_ePreState = m_eState;
	}
}

void CVoid19::Tick_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case Client::CVoid19::State_Idle:
		if (m_pModelCom->IsAnimationFinished(Anim_roar))
		{
			m_eState = State_Attack;
		}
		break;
	case Client::CVoid19::State_Attack:
		if (m_pModelCom->IsAnimationFinished(Anim_attack01))
		{
			m_eState = State_Idle;
		}
		break;
	case Client::CVoid19::State_Die:
		if (m_pModelCom->IsAnimationFinished(Anim_stun))
		{
			Kill();
		}
		break;
	}
}

HRESULT CVoid19::Add_Components()
{
	Collider_Desc ColliderDesc{};
	ColliderDesc.eType = ColliderType::AABB;
	ColliderDesc.vExtents = _vec3(0.5f, 1.2f, 0.5f);
	ColliderDesc.vCenter = _vec3(0.f, ColliderDesc.vExtents.y, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CVoid19* CVoid19::Create(_dev pDevice, _context pContext)
{
	CVoid19* pInstance = new CVoid19(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CVoid19");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVoid19::Clone(void* pArg)
{
	CVoid19* pInstance = new CVoid19(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVoid19");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVoid19::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
