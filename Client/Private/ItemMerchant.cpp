#include "ItemMerchant.h"
#include "Shop.h"
#include "3DUITex.h"

CItemMerchant::CItemMerchant(_dev pDevice, _context pContext)
	: CNPC(pDevice, pContext)
{
}

CItemMerchant::CItemMerchant(const CItemMerchant& rhs)
	: CNPC(rhs)
{
}

HRESULT CItemMerchant::Init_Prototype()
{
    return S_OK;
}

HRESULT CItemMerchant::Init(void* pArg)
{
	m_strModelTag = TEXT("Prototype_Model_ItemMerchant");

	if (FAILED(__super::Add_Components()))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	//m_pTransformCom->Set_State(State::Pos, _vec4(static_cast<_float>(rand() % 20), 0.f, static_cast<_float>(rand() % 20), 1.f));
	m_pTransformCom->Set_State(State::Pos, _vec4(75.f, 0.f, 100.f, 1.f));

	m_Animation.iAnimIndex = IDLE;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_Animation.fAnimSpeedRatio = 2.f;

	return S_OK;
}

void CItemMerchant::Tick(_float fTimeDelta)
{
	if (m_bTalking && !m_pShop->IsActive())
	{
		m_bTalking = false;
	}

	CCollider* pCollider = (CCollider*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Player_Hit_OBB"));
	_bool isColl = m_pColliderCom->Intersect(pCollider);
	if (!m_bTalking && isColl && m_pGameInstance->Key_Down(DIK_E))
	{
		m_bTalking = true;
		m_pShop->Open_Shop();
	}


	if (m_bTalking == true)
	{
		m_Animation.iAnimIndex = TALK;
		m_pShop->Tick(fTimeDelta);
	}
	else
	{
		m_Animation.iAnimIndex = IDLE;
		m_pSpeechBubble->Tick(fTimeDelta);
	}




	m_pModelCom->Set_Animation(m_Animation);

	__super::Update_Collider();
}

void CItemMerchant::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (m_bTalking == true)
	{
		m_pShop->Late_Tick(fTimeDelta);
	}
	else
	{
		m_pSpeechBubble->Late_Tick(fTimeDelta);
	}

}

HRESULT CItemMerchant::Render()
{
	__super::Render();

    return S_OK;
}

HRESULT CItemMerchant::Add_Parts()
{
	m_pShop = (CShop*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Shop"));

	C3DUITex::UITEX_DESC TexDesc = {};
	TexDesc.eLevelID = LEVEL_STATIC;
	TexDesc.pParentTransform = m_pTransformCom;
	TexDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_SpeechBubble");
	TexDesc.vPosition = _vec3(0.f, 2.2f, 0.f);
	TexDesc.vSize = _vec2(20.f, 20.f);

	m_pSpeechBubble = (C3DUITex*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_3DUITex"), &TexDesc);
	if (not m_pSpeechBubble)
	{
		return E_FAIL;
	}

	Collider_Desc CollDesc = {};
	CollDesc.eType = ColliderType::AABB;
	CollDesc.vRadians = _vec3(0.f, 0.f, 0.f);
	CollDesc.vExtents = _vec3(0.8f, 0.8f, 0.8f);
	CollDesc.vCenter = _vec3(0.f, CollDesc.vExtents.y * 0.9f, 0.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		TEXT("Com_Roskva"), (CComponent**)&m_pColliderCom, &CollDesc)))
		return E_FAIL;

	return S_OK;
}

CItemMerchant* CItemMerchant::Create(_dev pDevice, _context pContext)
{
	CItemMerchant* pInstance = new CItemMerchant(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CItemMerchant");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItemMerchant::Clone(void* pArg)
{
	CItemMerchant* pInstance = new CItemMerchant(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CItemMerchant");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemMerchant::Free()
{
	__super::Free();
	Safe_Release(m_pShop);
	Safe_Release(m_pSpeechBubble);
}
