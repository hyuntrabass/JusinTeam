#include "Player.h"
#include "BodyPart.h"
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
	if (m_pGameInstance->Key_Down(DIK_K))
	{
		((CBodyPart*)m_vecParts[PT_HAIR])->Set_ModelIndex();
		((CBodyPart*)m_vecParts[PT_HAIR])->Reset_Model();
		((CBodyPart*)m_vecParts[PT_FACE])->Reset_Model();
		((CBodyPart*)m_vecParts[2])->Reset_Model();
	}


	if (m_pGameInstance->Key_Down(DIK_L))
	{
		((CBodyPart*)m_vecParts[PT_FACE])->Set_ModelIndex();
		((CBodyPart*)m_vecParts[PT_HAIR])->Reset_Model();
		((CBodyPart*)m_vecParts[PT_FACE])->Reset_Model();
		((CBodyPart*)m_vecParts[2])->Reset_Model();
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
