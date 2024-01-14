#include "Penguin.h"

CPenguin::CPenguin(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CPenguin::CPenguin(const CPenguin& rhs)
	: CMonster(rhs)
{
}

HRESULT CPenguin::Init_Prototype()
{
	return S_OK;
}

HRESULT CPenguin::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Penguin");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, _vec4(10.f, 0.f, 0.f, 1.f));

	m_Animation.iAnimIndex = 0;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = true;

	m_iHP = 1;

	return S_OK;
}

void CPenguin::Tick(_float fTimeDelta)
{
	m_pModelCom->Set_Animation(m_Animation);

	Update_Collider();
}

void CPenguin::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

#ifdef _DEBUGTEST
	m_pRendererCom->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CPenguin::Render()
{
	__super::Render();

	return S_OK;
}

void CPenguin::Change_State(_float fTimeDelta)
{
}

void CPenguin::Control_State(_float fTimeDelta)
{
}

HRESULT CPenguin::Add_Collider()
{
	// Com_Collider
	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::Sphere;
	CollDesc.fRadius = 0.2f;
	CollDesc.vCenter = _vec3(0.f, 0.f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Collider_Sphere"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

void CPenguin::Update_Collider()
{
	_mat Matrix = *(m_pModelCom->Get_BoneMatrix("B_Mouth"));
	Matrix *= XMMatrixTranslation(0.f, 0.f, 0.3f);
	Matrix *= m_pTransformCom->Get_World_Matrix();

	m_pColliderCom->Update(Matrix);
}

CPenguin* CPenguin::Create(_dev pDevice, _context pContext)
{
	CPenguin* pInstance = new CPenguin(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CPenguin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPenguin::Clone(void* pArg)
{
	CPenguin* pInstance = new CPenguin(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPenguin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPenguin::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
}
