#include "Tentacle.h"

_float CTentacle::m_fOffset = 0.f;

CTentacle::CTentacle(_dev pDevice, _context pContext)
	: CMonster(pDevice, pContext)
{
}

CTentacle::CTentacle(const CTentacle& rhs)
	: CMonster(rhs)
{
}

HRESULT CTentacle::Init_Prototype()
{
    return S_OK;
}

HRESULT CTentacle::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_Groar_Tentacle");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Collider()))
	{
		return E_FAIL;
	}

	m_Animation.iAnimIndex = 0;
	m_Animation.isLoop = true;
	m_Animation.fAnimSpeedRatio = 3.f;

	m_pTransformCom->Set_Position(_vec3(80.f, 0.f, 120.f + m_fOffset));

	m_fOffset += 2.f;

    return S_OK;
}

void CTentacle::Tick(_float fTimeDelta)
{
	m_pModelCom->Set_Animation(m_Animation);
}

void CTentacle::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CTentacle::Render()
{
	__super::Render();

    return S_OK;
}

HRESULT CTentacle::Add_Collider()
{
    return S_OK;
}

void CTentacle::Update_Collider()
{
}

CTentacle* CTentacle::Create(_dev pDevice, _context pContext)
{
	CTentacle* pInstance = new CTentacle(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTentacle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTentacle::Clone(void* pArg)
{
	CTentacle* pInstance = new CTentacle(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTentacle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTentacle::Free()
{
	__super::Free();
}
