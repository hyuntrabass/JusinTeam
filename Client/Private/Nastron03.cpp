#include "Nastron03.h"

CNastron03::CNastron03(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CNastron03::CNastron03(const CNastron03& rhs)
	: CMonster(rhs)
{
}

HRESULT CNastron03::Init_Prototype()
{
	return S_OK;
}

HRESULT CNastron03::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Nastron03");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, _vec4(5.f, 0.f, 0.f, 1.f));
	m_pTransformCom->Set_Speed(3.f);

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = true;
	m_Animation.fAnimSpeedRatio = 1.5f;

	m_eCurState = STATE_IDLE;

	m_iHP = 10;

    return S_OK;
}

void CNastron03::Tick(_float fTimeDelta)
{
	Change_State(fTimeDelta);
	Control_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	Update_Collider();
}

void CNastron03::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUGTEST
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CNastron03::Render()
{
	__super::Render();

    return S_OK;
}

void CNastron03::Change_State(_float fTimeDelta)
{
	if (m_pModelCom->IsAnimationFinished(m_Animation.iAnimIndex))
	{
		m_eCurState = STATE_IDLE;
	}

	if (m_pGameInstance->Key_Down(DIK_J))
	{
		m_eCurState = STATE_ATTACK;
	}

	if (m_pGameInstance->Key_Down(DIK_H))
	{
		m_eCurState = STATE_HIT;
	}

	//if (m_pModelCom->IsAnimationFinished(WALK))
	//{
	//	m_eCurState = STATE_IDLE;
	//}

	//if (m_pModelCom->IsAnimationFinished(m_Animation.iAnimIndex) == false && m_Animation.isLoop == false)
	//{
	//	return;
	//}

	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CNastron03::STATE_IDLE:
			m_Animation.iAnimIndex = IDLE;
			m_Animation.isLoop = true;

			break;
		case Client::CNastron03::STATE_WALK:
			m_Animation.iAnimIndex = WALK;
			m_Animation.isLoop = false;

			{
				random_device rd;
				_randNum RandNum(rd());
				_randFloat Random = _randFloat(-1.f, 1.f);
				m_pTransformCom->LookAt_Dir(_vec4(Random(RandNum), 0.f, Random(RandNum), 0.f));
			}

			break;
		case Client::CNastron03::STATE_ATTACK:
			m_iAttackPattern = rand() % 3;

			break;
		case Client::CNastron03::STATE_HIT:
			m_iHitPattern = rand() % 2;

			break;
		case Client::CNastron03::STATE_DIE:
			m_Animation.iAnimIndex = DIE;
			m_Animation.isLoop = false;

			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CNastron03::Control_State(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case Client::CNastron03::STATE_IDLE:

		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 2.f)
		{
			m_eCurState = STATE_WALK;
			m_fIdleTime = 0.f;
		}

		break;
	case Client::CNastron03::STATE_WALK:
		m_pTransformCom->Go_Straight(fTimeDelta);
		break;
	case Client::CNastron03::STATE_ATTACK:

		switch (m_iAttackPattern)
		{
		case 0:
			m_Animation.iAnimIndex = ATTACK01;
			m_Animation.isLoop = false;
			break;
		case 1:
			m_Animation.iAnimIndex = ATTACK02;
			m_Animation.isLoop = false;
			break;
		case 2:
			m_Animation.iAnimIndex = ATTACK03;
			m_Animation.isLoop = false;
			break;
		}

		break;
	case Client::CNastron03::STATE_HIT:

		switch (m_iHitPattern)
		{
		case 0:
			m_Animation.iAnimIndex = HIT_L;
			m_Animation.isLoop = false;
			break;
		case 1:
			m_Animation.iAnimIndex = HIT_R;
			m_Animation.isLoop = false;
			break;
		}

		break;
	case Client::CNastron03::STATE_DIE:
		break;
	}
}

HRESULT CNastron03::Add_Collider()
{
	// Com_Collider
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.fRadius = 0.3f;
	CollDesc.vCenter = _vec3(0.f, 0.7f, -1.5f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Sphere"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

    return S_OK;
}

void CNastron03::Update_Collider()
{
	_mat Matrix = *(m_pModelCom->Get_BoneMatrix("Bip001-Prop1_end"));
	Matrix *= m_pTransformCom->Get_World_Matrix();

	m_pColliderCom->Update(Matrix);
}

CNastron03* CNastron03::Create(_dev pDevice, _context pContext)
{
	CNastron03* pInstance = new CNastron03(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CNastron03");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNastron03::Clone(void* pArg)
{
	CNastron03* pInstance = new CNastron03(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CNastron03");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNastron03::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
