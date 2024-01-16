#include "Player.h"
#include "BodyPart.h"
#include "UI_Manager.h"

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

	Add_Parts();
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
	Set_Key(fTimeDelta);



	if (!CUI_Manager::Get_Instance()->Is_Picking_UI() && m_pGameInstance->Mouse_Pressing(DIM_LBUTTON))
	{
		_long dwMouseMove;

		if (dwMouseMove = m_pGameInstance->Get_MouseMove(MouseState::x))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * dwMouseMove * -1.f * 0.1f);
		}
	}


	for (int i = 0; i < m_vecParts.size(); i++)
	{
		m_vecParts[i]->Tick(fTimeDelta);
	}
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	for (int i = 0; i < m_vecParts.size();i++)
	{
		m_vecParts[i]->Late_Tick(fTimeDelta);
	}
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
	BodyParts_Desc.iNumVariations = 9;
	wstring strName{};
	strName = TEXT("Prototype_GameObject_Body_Parts");
	pParts = m_pGameInstance->Clone_Object(strName, &BodyParts_Desc);

	if (pParts == nullptr)
		return E_FAIL;

	m_vecParts.push_back(pParts);

	BodyParts_Desc.eType = PT_FACE;
	BodyParts_Desc.iNumVariations = 7;


	pParts = m_pGameInstance->Clone_Object(strName, &BodyParts_Desc);

	if (pParts == nullptr)
		return E_FAIL;

	m_vecParts.push_back(pParts);

	BodyParts_Desc.eType = PT_BODY;
	BodyParts_Desc.iNumVariations = 1;

	pParts = m_pGameInstance->Clone_Object(strName, &BodyParts_Desc);

	if (pParts == nullptr)
		return E_FAIL;

	m_vecParts.push_back(pParts);

	return S_OK;
}

void CPlayer::Change_Parts(PART_TYPE PartsType, _int ChangeIndex)
{
	((CBodyPart*)m_vecParts[PartsType])->Set_ModelIndex(ChangeIndex);
	Reset_PartsAnim();
}

void CPlayer::Reset_PartsAnim()
{
	dynamic_cast<CBodyPart*>(m_vecParts[PT_HAIR])->Reset_Model();
	dynamic_cast<CBodyPart*>(m_vecParts[PT_FACE])->Reset_Model();
	dynamic_cast<CBodyPart*>(m_vecParts[PT_BODY])->Reset_Model();
}

void CPlayer::Set_Key(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_W))
	{
		m_pTransformCom->Go_Straight(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_S))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_A))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}

	if (m_pGameInstance->Key_Pressing(DIK_D))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
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
	case Client::WP_BOW:

		break;
	case Client::WP_SWORD:
		
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
}
