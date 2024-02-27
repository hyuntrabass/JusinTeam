#include "Aur.h"

CAur::CAur(_dev pDevice, _context pContext)
	: CNPC(pDevice, pContext)
{
}

CAur::CAur(const CAur& rhs)
	: CNPC(rhs)
{
}

HRESULT CAur::Init_Prototype()
{
    return S_OK;
}

HRESULT CAur::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Aur");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	m_Animation.iAnimIndex = HANGING01;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 2.f;

	PxCapsuleControllerDesc ControllerDesc{};
	ControllerDesc.height = 0.8f; // ����(�� �Ʒ��� �ݱ� ũ�� ����
	ControllerDesc.radius = 0.6f; // ���Ʒ� �ݱ��� ������
	ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // �� ����
	ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // ĳ���Ͱ� ���� �� �ִ� �ִ� ����
	ControllerDesc.contactOffset = 0.1f; // ĳ���Ϳ� �ٸ� ��ü���� �浹�� �󸶳� ���� ��������. ���� Ŭ���� �� ���� ���������� ���ɿ� ���� ���� �� ����.
	ControllerDesc.stepOffset = 0.2f; // ĳ���Ͱ� ���� �� �ִ� ����� �ִ� ����

	m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);

	m_pTransformCom->Set_Position(_vec3(71.f, 8.f, 97.f));

	return S_OK;
}

void CAur::Tick(_float fTimeDelta)
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

	m_pModelCom->Set_Animation(m_Animation);
}

void CAur::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CAur::Render()
{
	__super::Render();

    return S_OK;
}

CAur* CAur::Create(_dev pDevice, _context pContext)
{
	CAur* pInstance = new CAur(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CAur");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CAur::Clone(void* pArg)
{
	CAur* pInstance = new CAur(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CAur");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAur::Free()
{
	__super::Free();
}
