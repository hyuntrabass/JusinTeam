#include "NPC_Dummy.h"
#include "UI_Manager.h"
#include "Dialog.h"

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

	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.vCenter = _vec3(0.f, 0.f, 0.f);
	CollDesc.fRadius = 12.f;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_NpcCol"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;


	CDialog::DIALOG_DESC DialogDesc = {};
	DialogDesc.eLevelID = LEVEL_STATIC;
	DialogDesc.pParentTransform = m_pTransformCom;
	DialogDesc.vPosition = _vec3(0.f, 2.2f, 0.f);
	DialogDesc.strText = TEXT("�׽�Ʈ�Դϴ�~");

#pragma region Animal

	if (m_strModelTag == TEXT("Prototype_Model_Donkey"))
	{
		m_Animation.iAnimIndex = 1;
		DialogDesc.strText = TEXT("����������");
	}

#pragma endregion Animal

#pragma region IDLE NPC

	if (m_strModelTag == TEXT("Prototype_Model_Dwarf_Male_002"))
	{
		m_Animation.iAnimIndex = 0;
		DialogDesc.strText = TEXT("�츮 ������ �Ϳ���?");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_003"))
	{
		m_Animation.iAnimIndex = 2;
		DialogDesc.strText = TEXT("������ �� ����~");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_004"))
	{
		m_Animation.iAnimIndex = 2;
		DialogDesc.strText = TEXT("���������� �� ��� ���� ���̾�");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_006"))
	{
		m_Animation.iAnimIndex = 4;
		DialogDesc.strText = TEXT("�ƴ� ��� 5�ÿ� ���ڸ鼭 �� �ȿ�");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_009"))
	{
		m_Animation.iAnimIndex = 1;
		DialogDesc.strText = TEXT("�ɽ��ϴ�");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_013"))
	{
		DialogDesc.strText = TEXT("���� ���� ���� ����?");
		m_Animation.iAnimIndex = 1;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_015"))
	{
		m_Animation.iAnimIndex = 2;
		DialogDesc.strText = TEXT("�����ٴϴ� ����� ������");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_018"))
	{
		DialogDesc.strText = TEXT("�츮�� ����̰� �����?");
		m_Animation.iAnimIndex = 0;
	}

	if (m_strModelTag == TEXT("Prototype_Model_SkillMerchant"))
	{
		DialogDesc.strText = TEXT("�ȳ��ϼ���");
		m_Animation.iAnimIndex = 0;
	}

#pragma endregion IDLE NPC

#pragma region DANCE NPC

	if (m_strModelTag == TEXT("Prototype_Model_Female_013"))
	{
		m_Animation.iAnimIndex = 1;
		DialogDesc.strText = TEXT("�ų���~");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_027"))
	{
		DialogDesc.strText = TEXT("�� ���� �� ������ ��");
		m_Animation.iAnimIndex = 2;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_Chi"))
	{
		DialogDesc.strText = TEXT("���� ���� ������");
		m_Animation.iAnimIndex = 3;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_016"))
	{
		DialogDesc.strText = TEXT("�� ���� �� ������");
		m_Animation.iAnimIndex = 0;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Male_020"))
	{
		DialogDesc.strText = TEXT("�Ҿƹ��� �ҸӴ� ����� ����~");
		m_Animation.iAnimIndex = 0;
	}

#pragma endregion DANCE NPC

#pragma region TALK NPC

	if (m_strModelTag == TEXT("Prototype_Model_Female_002"))
	{
		m_Animation.iAnimIndex = 7;
		DialogDesc.strText = TEXT("���� ����̾�"); 
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_005"))
	{
		m_Animation.iAnimIndex = 1;
		DialogDesc.strText = TEXT("��� ���� �����̾�?");
	}

	if (m_strModelTag == TEXT("Prototype_Model_Female_010"))
	{
		m_Animation.iAnimIndex = 10;
		DialogDesc.strText = TEXT("�°� �׷��ٴϱ�~ ");
	}

#pragma endregion TALK NPC

#pragma region ��Ÿ ġ�¾�

	if (m_strModelTag == TEXT("Prototype_Model_Male_027"))
	{
		DialogDesc.strText = TEXT("�� ���� ��Ÿ�� ��");
		m_Animation.iAnimIndex = 1;
	}

#pragma endregion ��Ÿ ġ�¾�

	m_Animation.isLoop = true;
	m_Animation.fAnimSpeedRatio = 2.f;

	m_pDialog = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Dialog"), &DialogDesc);
	if (m_pDialog == nullptr)
	{
		return E_FAIL;
	}

	if (m_strModelTag == TEXT("Prototype_Model_Donkey"))
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
	__super::Tick(fTimeDelta);

#ifdef _DEBUG

	if (m_strModelTag == TEXT("Prototype_Model_Male_009"))
	{
		dynamic_cast<CDialog*>(m_pDialog)->Set_Text(L"���� �������� " + to_wstring(m_pGameInstance->Get_FPS()) + L"�̱���.");
	}

#endif // _DEBUG

	if (m_strModelTag == TEXT("Prototype_Model_Male_027"))
	{
		m_fTimer += fTimeDelta;

		if (m_fTimer > 3.f)
		{
			_int iRandomNum = rand() % 3;

			switch (iRandomNum)
			{
			case 0:
				dynamic_cast<CDialog*>(m_pDialog)->Set_Text(L"�� ���� ��Ÿ�� ��");
				break;
			case 1:
				dynamic_cast<CDialog*>(m_pDialog)->Set_Text(L"����������");
				break;
			case 2:
				dynamic_cast<CDialog*>(m_pDialog)->Set_Text(L"������ ��������");
				break;
			}

			m_fTimer = {};
		}
	}


	m_pModelCom->Set_Animation(m_Animation);
	m_pDialog->Tick(fTimeDelta);

	__super::Update_Collider();
	m_pTransformCom->Gravity(fTimeDelta);

}

void CNPC_Dummy::Late_Tick(_float fTimeDelta)
{
	if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_State(State::Pos), 2.f))
	{
		__super::Late_Tick(fTimeDelta);
		CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Player_Hit_OBB"));
		_bool isColl = m_pColliderCom->Intersect(pCollider);
		if (isColl)
		{
			if (m_strModelTag == TEXT("Prototype_Model_Male_009"))
			{
				_int a = 10;
			}

			//wstring strTest = m_strModelTag;
			m_pDialog->Late_Tick(fTimeDelta);
		}
	}
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
	//CUI_Manager::Get_Instance()->Delete_RadarPos(CUI_Manager::NPC, m_pTransformCom);
	__super::Free();
	Safe_Release(m_pDialog);
}
