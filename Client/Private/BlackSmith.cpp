#include "BlackSmith.h"
#include "UI_Manager.h"

CBlackSmith::CBlackSmith(_dev pDevice, _context pContext)
	: CNPC(pDevice, pContext)
{
}

CBlackSmith::CBlackSmith(const CBlackSmith& rhs)
	: CNPC(rhs)
{
}

HRESULT CBlackSmith::Init_Prototype()
{
    return S_OK;
}

HRESULT CBlackSmith::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_BlackSmith");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	//m_pTransformCom->Set_State(State::Pos, _vec4(static_cast<_float>(rand() % 20), 0.f, static_cast<_float>(rand() % 20), 1.f));
	m_pTransformCom->Set_State(State::Pos, _vec4(70.f, 0.f, 100.f, 1.f));

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 2.f;

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.4f; // ����(�� �Ʒ��� �ݱ� ũ�� ����
	ControllerDesc.radius = 0.6f; // ���Ʒ� �ݱ��� ������
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // �� ����
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // ĳ���Ͱ� ���� �� �ִ� �ִ� ����
	ControllerDesc.contactOffset = 0.1f; // ĳ���Ϳ� �ٸ� ��ü���� �浹�� �󸶳� ���� ��������. ���� Ŭ���� �� ���� ���������� ���ɿ� ���� ���� �� ����.
	ControllerDesc.stepOffset = 0.2f; // ĳ���Ͱ� ���� �� �ִ� ����� �ִ� ����

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	if (pArg)
	{
		if (FAILED(__super::Init(pArg)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

void CBlackSmith::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	if (false) // ���߿� ���� �߰�
	{
		m_bTalking = true;
	}

	if (m_bTalking == true)
	{
		m_Animation.iAnimIndex = TALK01;
	}
	else
	{
		m_Animation.iAnimIndex = IDLE;
	}

	m_pModelCom->Set_Animation(m_Animation);

	m_pTransformCom->Gravity(fTimeDelta);

}

void CBlackSmith::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CBlackSmith::Render()
{
	__super::Render();

    return S_OK;
}

CBlackSmith* CBlackSmith::Create(_dev pDevice, _context pContext)
{
	CBlackSmith* pInstance = new CBlackSmith(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBlackSmith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlackSmith::Clone(void* pArg)
{
	CBlackSmith* pInstance = new CBlackSmith(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBlackSmith");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlackSmith::Free()
{
	__super::Free();
}
