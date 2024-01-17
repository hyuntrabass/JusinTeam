#include "Player.h"
#include "BodyPart.h"
#include "UI_Manager.h"
#include "Weapon.h"
CPlayer::CPlayer(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Init_Prototype()
{
	return S_OK;
}

HRESULT CPlayer::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = true;
	m_pTransformCom->Set_Scale(_vec3(4.f));
	Add_Parts();
	m_pTransformCom->Set_Speed(1);

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{

	PART_TYPE eType = CUI_Manager::Get_Instance()->Is_CustomPartChanged();
	if (PART_TYPE::PT_END != eType)
	{
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f);
		Change_Parts(eType, CUI_Manager::Get_Instance()->Get_CustomPart(eType));
	}

	if (m_pGameInstance->Get_CurrentLevelIndex() != LEVEL_CUSTOM)
	{
		Set_Key(fTimeDelta);
	}

	_float fMouseSensor = 0.1f;
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_CUSTOM)
	{
		if (!CUI_Manager::Get_Instance()->Is_Picking_UI() && m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
		{
			_long dwMouseMove;

			if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
			{
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * dwMouseMove * -1.f * 0.1f);
			}
		}
	}

	for (int i = 0; i < m_vecParts.size(); i++)
	{
		m_vecParts[i]->Tick(fTimeDelta);
	}

	if(m_pWeapon!=nullptr)
	m_pWeapon->Tick(fTimeDelta);
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	if (!m_bStartGame && m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_GAMEPLAY)
	{
		Change_Parts(PT_FACE, 7);
		Change_Parts(PT_HAIR, 9);
		Change_Parts(PT_BODY, 1);
		m_pTransformCom->Set_Scale(_vec3(0.1f));
		Add_Weapon();
		m_bStartGame = true;
	}

	for (int i = 0; i < m_vecParts.size();i++)
	{
		m_vecParts[i]->Late_Tick(fTimeDelta);
	}

	if (m_pWeapon != nullptr)
	m_pWeapon->Late_Tick(fTimeDelta);

}

HRESULT CPlayer::Render()
{
	return S_OK;
}

HRESULT CPlayer::Add_Parts()
{
	CGameObject* pParts = { nullptr };

	BODYPART_DESC BodyParts_Desc{};
	BodyParts_Desc.pParentTransform = m_pTransformCom;
	BodyParts_Desc.Animation = &m_Animation;
	BodyParts_Desc.eType = PT_HAIR;
	BodyParts_Desc.iNumVariations = 10;
	
	wstring strName{};

	strName = TEXT("Prototype_GameObject_Body_Parts");
	pParts = m_pGameInstance->Clone_Object(strName, &BodyParts_Desc);

	if (pParts == nullptr)
		return E_FAIL;

	m_vecParts.push_back(pParts);

	BodyParts_Desc.eType = PT_FACE;
	BodyParts_Desc.iNumVariations = 8;


	pParts = m_pGameInstance->Clone_Object(strName, &BodyParts_Desc);

	if (pParts == nullptr)
		return E_FAIL;

	m_vecParts.push_back(pParts);

	BodyParts_Desc.eType = PT_BODY;
	BodyParts_Desc.iNumVariations = 4;

	pParts = m_pGameInstance->Clone_Object(strName, &BodyParts_Desc);

	if (pParts == nullptr)
		return E_FAIL;

	m_vecParts.push_back(pParts);

	return S_OK;
}

HRESULT CPlayer::Add_Weapon()
{

	

	WEAPONPART_DESC WeaponParts_Desc{};
	WeaponParts_Desc.pParentTransform = m_pTransformCom;
	WeaponParts_Desc.Animation = &m_Animation;
	WeaponParts_Desc.iNumVariations = 6;

	wstring strName{};

	strName = TEXT("Prototype_GameObject_Weapon");
	m_pWeapon = m_pGameInstance->Clone_Object(strName, &WeaponParts_Desc);

	if (m_pWeapon == nullptr)
		return E_FAIL;

	m_Current_Weapon = WP_BOW;

	return S_OK;
}

void CPlayer::Change_Parts(PART_TYPE PartsType, _int ChangeIndex)
{
	dynamic_cast<CBodyPart*>(m_vecParts[PartsType])->Set_ModelIndex(ChangeIndex);
	Reset_PartsAnim();
}

void CPlayer::Change_Weapon(WEAPON_TYPE PartsType, _int ChangeIndex)
{
	dynamic_cast<CWeapon*>(m_pWeapon)->Set_ModelIndex(PartsType);

}

void CPlayer::Reset_PartsAnim()
{
	dynamic_cast<CBodyPart*>(m_vecParts[PT_HAIR])->Reset_Model();
	dynamic_cast<CBodyPart*>(m_vecParts[PT_FACE])->Reset_Model();
	dynamic_cast<CBodyPart*>(m_vecParts[PT_BODY])->Reset_Model();
	//dynamic_cast<CWeapon*>(m_pWeapon)->Reset_Model();

}

void CPlayer::Set_Key(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_0))
	{
		Change_Parts(PT_BODY, 0);
	}
	if (m_pGameInstance->Key_Down(DIK_1))
	{
		Change_Parts(PT_BODY, 1);
	}
	if (m_pGameInstance->Key_Down(DIK_2))
	{
		Change_Parts(PT_BODY, 2);
	}
	if (m_pGameInstance->Key_Down(DIK_3))
	{
		Change_Parts(PT_BODY, 3);
	}
	if (m_pGameInstance->Key_Pressing(DIK_W))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
		m_Animation.iAnimIndex = Anim_Normal_Walk;
	}

	if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
		m_Animation.iAnimIndex = Anim_Normal_Walk;
	}

	if (m_pGameInstance->Key_Pressing(DIK_A))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
		m_Animation.iAnimIndex = Anim_Normal_Walk;
	}

	if (m_pGameInstance->Key_Pressing(DIK_D))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
		m_Animation.iAnimIndex = Anim_Normal_Walk;
	}

	if (m_pGameInstance->Key_Pressing(DIK_G))
	{
		Attack();
	}
}

void CPlayer::Attack()
{
	switch (m_Current_Weapon)
	{
	case WP_BOW:

		break;
	case WP_SWORD:
		
		break;
	default:
		break;
	}
}

HRESULT CPlayer::Add_Components()
{
	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{
	return S_OK;
}

CPlayer* CPlayer::Create(_dev pDevice, _context pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();


	for (auto& iter : m_vecParts)
	{
		Safe_Release(iter);
	}

	m_vecParts.clear();

	Safe_Release(m_pWeapon);

}
