#include "Eagle.h"

#include "Effect_Dummy.h"
#include "Effect_Manager.h"

const _float CEagle::m_fChaseRange = 7.f;
const _float CEagle::m_fAttackRange = 4.f;

_uint CEagle::m_iEagleID = 0;

CEagle::CEagle(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CEagle::CEagle(const CEagle& rhs)
	: CMonster(rhs)
{
}

HRESULT CEagle::Init_Prototype()
{
    return S_OK;
}

HRESULT CEagle::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Eagle");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}


	m_eCurState = STATE_SPAWN;

	m_iHP = 10;

	m_pGameInstance->Register_CollisionObject(this, m_pBodyColliderCom);

	CTransform* pDragonTransform = GET_TRANSFORM("Layer_Dragon_Boss", LEVEL_TOWER);
	_vec4 vDragonUp = pDragonTransform->Get_State(State::Up).Get_Normalized();
	_vec3 vDragonLook = pDragonTransform->Get_State(State::Look).Get_Normalized();
	_vec4 vDragonPos = pDragonTransform->Get_State(State::Pos);

	_vec4 vDir = {};

	switch (m_iEagleID)
	{
	case 0:
		vDir = vDragonUp;
		break;

	case 1:
		vDir = _vec4::Transform(vDragonUp, _mat::CreateFromAxisAngle(vDragonLook, XMConvertToRadians(30.f)));
		break;

	case 2:
		vDir = _vec4::Transform(vDragonUp, _mat::CreateFromAxisAngle(vDragonLook, XMConvertToRadians(-30.f)));
		break;
	}

	m_pTransformCom->Set_Scale(_vec3(0.5f));
	m_pTransformCom->Set_Position(_vec3(vDragonPos +  10 * vDir));
	m_pTransformCom->LookAt_Dir(vDragonLook);

	++m_iEagleID;

	if (m_iEagleID == 3)
	{
		m_iEagleID = 0;
	}

	m_fDissolveRatio = 1.f;

    return S_OK;
}

void CEagle::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_BACKSPACE))
	{
		Kill();
	}

	Init_State(fTimeDelta);
	Tick_State(fTimeDelta);

	m_pModelCom->Set_Animation(m_Animation);

	__super::Update_BodyCollider();

	__super::Tick(fTimeDelta);
}

void CEagle::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
	m_pRendererCom->Add_DebugComponent(m_pBodyColliderCom);
#endif
}

HRESULT CEagle::Render()
{
	__super::Render();

    return S_OK;
}

void CEagle::Set_Damage(_int iDamage, _uint iDamageType)
{
	if (iDamage > 0)
	{
		m_eCurState = STATE_DIE;
		m_iHP = 0;
		
		m_iPassIndex = AnimPass_Default;
		m_pTransformCom->Set_Speed(6.f);
	}

}

void CEagle::Init_State(_float fTimeDelta)
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case Client::CEagle::STATE_SPAWN:
			m_Animation.iAnimIndex = FLY;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CEagle::STATE_IDLE:
			m_Animation.iAnimIndex = FLY;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;

			m_bCreateEffect = false;
			break;

		case Client::CEagle::STATE_CHASE:
			m_Animation.iAnimIndex = FLYIDLE;
			m_Animation.isLoop = true;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;

		case Client::CEagle::STATE_ATTACK:
			m_Animation.iAnimIndex = ATTACK02;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 3.f;

			break;

		case Client::CEagle::STATE_DIE:
			m_Animation.iAnimIndex = DIE;
			m_Animation.isLoop = false;
			m_Animation.fAnimSpeedRatio = 2.f;

			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CEagle::Tick_State(_float fTimeDelta)
{
	_vec4 vPlayerPos = __super::Compute_PlayerPos();
	_float fDistance = __super::Compute_PlayerDistance();

	if (m_pTransformCom->Get_State(State::Pos).y <= -5.f)
	{
		Kill();
	}

	switch (m_eCurState)
	{
	case Client::CEagle::STATE_SPAWN:

		if (m_fDissolveRatio <= 0.f)
		{
			m_fDissolveRatio = 0.f;
			//m_iPassIndex = AnimPass_OutLine;
		}
		else
		{
			m_fDissolveRatio -= 0.01f;
		}

		if (m_pModelCom->IsAnimationFinished(FLY))
		{
			m_eCurState = STATE_CHASE;
		}

		break;

	case Client::CEagle::STATE_IDLE:

		m_fIdleTime += fTimeDelta;

		if (m_fIdleTime >= 1.f)
		{
			if (fDistance >= m_fAttackRange)
			{
				m_eCurState = STATE_CHASE;
			}
			else
			{
				m_eCurState = STATE_ATTACK;
			}

			m_fIdleTime = 0.f;
		}

		break;

	case Client::CEagle::STATE_CHASE:

	{
		//_vec4 vDir = (vPlayerPos - m_pTransformCom->Get_State(State::Pos)).Get_Normalized();
		//vDir.y = 0.f;

		//m_pTransformCom->LookAt_Dir(vDir);
		m_pTransformCom->LookAt(vPlayerPos + _vec3(0.f, 3.f, 0.f));
		m_pTransformCom->Go_Straight(fTimeDelta);

		if (fDistance <= m_fAttackRange)
		{
			m_eCurState = STATE_ATTACK;
		}
	}

		break;

	case Client::CEagle::STATE_ATTACK:

		if (m_pModelCom->Get_CurrentAnimPos() >= 110.f)
		{
			m_fTime += fTimeDelta;
			m_fDamageTime += fTimeDelta;

			if (!m_bCreateEffect)
			{
				_mat Matrix = _mat::CreateScale(1.f) * _mat::CreateTranslation(_vec3(m_pTransformCom->Get_State(State::Pos)));
				EffectInfo Info = CEffect_Manager::Get_Instance()->Get_EffectInformation(L"Eagle_Distortion");
				Info.pMatrix = &Matrix;
				Info.fLifeTime = 5.f;
				CEffect_Manager::Get_Instance()->Add_Layer_Effect(Info);

				m_bCreateEffect = true;
			}

			if (m_fTime <= 5.f)
			{
				if (fDistance <= 8.f)
				{
					if (m_fDamageTime >= 0.3f)
					{
						m_pGameInstance->Attack_Player(nullptr, 20 + rand() % 10, MonAtt_Hit);

						m_fDamageTime = 0.f;
					}
				}
			}
		}

		if (m_pModelCom->IsAnimationFinished(ATTACK02))
		{
			m_eCurState = STATE_IDLE;
		}

		break;

	case Client::CEagle::STATE_DIE:

		m_pTransformCom->Go_Down(fTimeDelta);

		//if (m_pModelCom->IsAnimationFinished(DIE))
		//{
		//	m_pTransformCom->Go_Down(fTimeDelta);
		//}

		break;
	}
}

HRESULT CEagle::Add_Collider()
{
	Collider_Desc BodyCollDesc = {};
	BodyCollDesc.eType = ColliderType::OBB;
	BodyCollDesc.vExtents = _vec3(0.8f, 2.f, 0.5f);
	BodyCollDesc.vCenter = _vec3(0.f, BodyCollDesc.vExtents.y + 0.2f, 0.f);
	BodyCollDesc.vRadians = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_OBB"), (CComponent**)&m_pBodyColliderCom, &BodyCollDesc)))
	{
		return E_FAIL;
	}

    return S_OK;
}

void CEagle::Update_Collider()
{
}

CEagle* CEagle::Create(_dev pDevice, _context pContext)
{
	CEagle* pInstance = new CEagle(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CEagle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEagle::Clone(void* pArg)
{
	CEagle* pInstance = new CEagle(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CEagle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEagle::Free()
{
	__super::Free();
}
