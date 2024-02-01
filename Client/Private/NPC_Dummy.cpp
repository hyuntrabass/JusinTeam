#include "NPC_Dummy.h"
#include "UI_Manager.h"
#include "Dialog.h"

_float CNPC_Dummy::m_fOffsetX = 75.f;

CNPC_Dummy::CNPC_Dummy(_dev pDevice, _context pContext)
	: CNPC(pDevice, pContext)
{
}

CNPC_Dummy::CNPC_Dummy(const CNPC_Dummy& rhs)
	: CNPC(rhs)
{
}

HRESULT CNPC_Dummy::Init_Prototype()
{
    return S_OK;
}

HRESULT CNPC_Dummy::Init(void* pArg)
{
	if (pArg)
	{
		m_pInfo = *(NPC_INFO*)pArg;
		m_strModelTag = m_pInfo.strNPCPrototype;
	}

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}


	CDialog::DIALOG_DESC DialogDesc = {};
	DialogDesc.eLevelID = LEVEL_STATIC;
	DialogDesc.pParentTransform = m_pTransformCom;
	DialogDesc.vPosition = _vec3(0.f, 1.8f, 0.f);
	DialogDesc.strText = TEXT("�׽�Ʈ�Դϴ�~");

#pragma region Animal

	if (m_strModelTag == TEXT("Prototype_Model_Horse"))
	{
		m_Animation.iAnimIndex = 1;
	}

#pragma endregion Animal

#pragma region IDLE NPC

	if (m_strModelTag == TEXT("Prototype_Model_Dwarf_Male_002"))
	{
		m_Animation.iAnimIndex = 0;
		DialogDesc.strText = TEXT("���� �������");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_003"))
	{
		m_Animation.iAnimIndex = 2;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_004"))
	{
		m_Animation.iAnimIndex = 2;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_006"))
	{
		m_Animation.iAnimIndex = 4;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_009"))
	{
		m_Animation.iAnimIndex = 1;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_013"))
	{
		m_Animation.iAnimIndex = 1;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_015"))
	{
		m_Animation.iAnimIndex = 2;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_018"))
	{
		m_Animation.iAnimIndex = 0;
	}

	if (m_strModelTag == TEXT("Prototype_Model_SkillMerchant"))
	{
		m_Animation.iAnimIndex = 0;
	}

#pragma endregion IDLE NPC

#pragma region DANCE NPC

	if (m_strModelTag == TEXT("Prototype_Model_Female_013"))
	{
		m_Animation.iAnimIndex = 1;
		DialogDesc.strText = TEXT("���ߴ� ��� ���ߴ� ��� ���ߴ� ��� �� �ؽ�Ʈ");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_027"))
	{
		m_Animation.iAnimIndex = 2;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_Chi"))
	{
		m_Animation.iAnimIndex = 3;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_016"))
	{
		m_Animation.iAnimIndex = 0;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_020"))
	{
		m_Animation.iAnimIndex = 0;
	}

#pragma endregion DANCE NPC

#pragma region TALK NPC

	if (m_strModelTag == TEXT("Prototype_Model_Female_002"))
	{
		m_Animation.iAnimIndex = 7;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_005"))
	{
		m_Animation.iAnimIndex = 1;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_010"))
	{
		m_Animation.iAnimIndex = 10;
	}

#pragma endregion TALK NPC

#pragma region ��Ÿ ġ�¾�

	if (m_strModelTag == TEXT("Prototype_Model_Male_027"))
	{
		m_Animation.iAnimIndex = 1;
	}

#pragma endregion ��Ÿ ġ�¾�

	m_Animation.isLoop = true;
	m_Animation.fAnimSpeedRatio = 2.f;

	m_fOffsetX -= 2.f;

	
	m_pDialog = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Dialog"), &DialogDesc);
	if (m_pDialog == nullptr)
	{
		return E_FAIL;
	}
	
	CUI_Manager::Get_Instance()->Set_RadarPos(CUI_Manager::NPC, m_pTransformCom);

	if (m_strModelTag == TEXT("Prototype_Model_Horse"))
	{
		PxCapsuleControllerDesc ControllerDesc{};
		ControllerDesc.height = 0.7f; // ����(�� �Ʒ��� �ݱ� ũ�� ����
		ControllerDesc.radius = 1.f; // ���Ʒ� �ݱ��� ������
		ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // �� ����
		ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // ĳ���Ͱ� ���� �� �ִ� �ִ� ����
		ControllerDesc.contactOffset = 0.1f; // ĳ���Ϳ� �ٸ� ��ü���� �浹�� �󸶳� ���� ��������. ���� Ŭ���� �� ���� ���������� ���ɿ� ���� ���� �� ����.
		ControllerDesc.stepOffset = 0.2f; // ĳ���Ͱ� ���� �� �ִ� ����� �ִ� ����

		m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);
	}

	else if (m_strModelTag == TEXT("Prototype_Model_Dwarf_Male_002"))
	{
		PxCapsuleControllerDesc ControllerDesc{};
		ControllerDesc.height = 0.4f; // ����(�� �Ʒ��� �ݱ� ũ�� ����
		ControllerDesc.radius = 0.8f; // ���Ʒ� �ݱ��� ������
		ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // �� ����
		ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // ĳ���Ͱ� ���� �� �ִ� �ִ� ����
		ControllerDesc.contactOffset = 0.1f; // ĳ���Ϳ� �ٸ� ��ü���� �浹�� �󸶳� ���� ��������. ���� Ŭ���� �� ���� ���������� ���ɿ� ���� ���� �� ����.
		ControllerDesc.stepOffset = 0.2f; // ĳ���Ͱ� ���� �� �ִ� ����� �ִ� ����

		m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);
	}

	else
	{
		PxCapsuleControllerDesc ControllerDesc{};
		ControllerDesc.height = 0.8f; // ����(�� �Ʒ��� �ݱ� ũ�� ����
		ControllerDesc.radius = 0.6f; // ���Ʒ� �ݱ��� ������
		ControllerDesc.upDirection = PxVec3(0.f, 1.f, 0.f); // �� ����
		ControllerDesc.slopeLimit = cosf(PxDegToRad(60.f)); // ĳ���Ͱ� ���� �� �ִ� �ִ� ����
		ControllerDesc.contactOffset = 0.1f; // ĳ���Ϳ� �ٸ� ��ü���� �浹�� �󸶳� ���� ��������. ���� Ŭ���� �� ���� ���������� ���ɿ� ���� ���� �� ����.
		ControllerDesc.stepOffset = 0.2f; // ĳ���Ͱ� ���� �� �ִ� ����� �ִ� ����

		m_pGameInstance->Init_PhysX_Character(m_pTransformCom, COLGROUP_MONSTER, &ControllerDesc);
	}

	if (pArg)
	{
		if (FAILED(__super::Init(pArg)))
		{
			return E_FAIL;
		}
	}

    return S_OK;
}

void CNPC_Dummy::Tick(_float fTimeDelta)
{	
	m_pModelCom->Set_Animation(m_Animation);
	//m_pDialog->Tick(fTimeDelta);

	m_pTransformCom->Gravity(fTimeDelta);
}

void CNPC_Dummy::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	//m_pDialog->Late_Tick(fTimeDelta);
}

HRESULT CNPC_Dummy::Render()
{
	__super::Render();

    return S_OK;
}

CNPC_Dummy* CNPC_Dummy::Create(_dev pDevice, _context pContext)
{
	CNPC_Dummy* pInstance = new CNPC_Dummy(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CNPC_Dummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNPC_Dummy::Clone(void* pArg)
{
	CNPC_Dummy* pInstance = new CNPC_Dummy(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CNPC_Dummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNPC_Dummy::Free()
{
	__super::Free();
	Safe_Release(m_pDialog);
}
