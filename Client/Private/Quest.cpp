#include "Quest.h"
#include "GameInstance.h"
#include "TextButton.h"

CQuest::CQuest(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CQuest::CQuest(const CQuest& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CQuest::Init_Prototype()
{
	return S_OK;
}

HRESULT CQuest::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	

	m_fSizeX = 40.f;
	m_fSizeY = 70.f;

	m_fX = 360.f;
	m_fY = 630.f;

	m_fDepth = 0.5f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CQuest::Tick(_float fTimeDelta)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	RECT rectUI = {
			  (LONG)(m_fX - m_fSizeX * 0.5f),
			  (LONG)(m_fY - m_fSizeY * 0.5f),
			  (LONG)(m_fX + m_fSizeX * 0.5f),
			  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

	if (TRUE == PtInRect(&rectUI, ptMouse))
	{
		if (m_isActive && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
			m_isActive = false;
		}
		if (!m_isActive && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
			m_bNewQuestIn = false;
			m_isActive = true;
		}
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (m_isActive)
	{
		for (auto& iter : m_QuestMap)
		{
			iter.second->Tick(fTimeDelta);
		}
	}

}

void CQuest::Late_Tick(_float fTimeDelta)
{
	if (m_isActive)
	{
		m_pButton->Late_Tick(fTimeDelta);
	}

	if (m_bNewQuestIn)
	{
		m_pNotify->Late_Tick(fTimeDelta);
	}
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);

}

HRESULT CQuest::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_UI)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CQuest::Set_Quest(CQuestBox::QUESTBOX_DESC& QuestBoxDesc)
{
	_float fY = m_fY + (m_QuestMap.size() * 45.f) + (m_QuestMap.size() * 2.f);
	QuestBoxDesc.vPosition = _float2(m_fX - 70.f, fY);

	CQuestBox* pQuestBox = (CQuestBox*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_QuestBox"), &QuestBoxDesc);
	if (not pQuestBox)
	{
		return E_FAIL;
	}
	m_QuestMap.emplace(QuestBoxDesc.strQuestTitle, pQuestBox);
	m_bNewQuestIn = true;

	Sort_Quest();
	
	return S_OK;
}

void CQuest::Sort_Quest()
{
	if (m_QuestMap.empty())
	{
		return;
	}
	_bool isMain = false;
	for (auto& iter : m_QuestMap)
	{
		if (iter.second->IsMain())
		{
			isMain = true;
			break;
		}
	}

	_float fY = m_fY + (m_QuestMap.size() * 45.f) + (m_QuestMap.size() * 2.f);
	if (isMain)
	{
		_uint iSize = 1;
		for (auto& iter : m_QuestMap)
		{
			if (iter.second->IsMain())
			{
				iter.second->Set_Position(_float2(iter.second->Get_Position().x, m_fY));
			}
			else
			{
				_float fY = m_fY + (iSize * 45.f) + (iSize * 2.f);
				iter.second->Set_Position(_float2(iter.second->Get_Position().x, fY));
			}
			iSize++;
		}
	}
	else
	{
		_uint iSize = 0;
		for (auto& iter : m_QuestMap)
		{
			_float fY = m_fY + (iSize * 45.f) + (iSize * 2.f);
			iter.second->Set_Position(_float2(iter.second->Get_Position().x, fY));
		}
	}
}

_bool CQuest::Update_Quest(const wstring& strQuest)
{
	auto iter = m_QuestMap.find(strQuest);
	if (iter == m_QuestMap.end())
		return false;

	if (iter->second->Update_Quest())
	{
		Safe_Release(m_QuestMap[strQuest]);
		m_QuestMap.erase(iter);
		Sort_Quest();
		return true;
	}
	return false;
}

HRESULT CQuest::Add_Parts()
{
	CTextButton::TEXTBUTTON_DESC Button = {};
	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.01f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_ShadeButton");
	Button.vPosition = _vec2(m_fX, m_fY);
	Button.vSize = _vec2(m_fSizeX, m_fSizeY);

	m_pButton = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pButton)
	{
		return E_FAIL;
	}

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.02f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Notify");
	Button.vPosition = _vec2(m_fX + 5.f, m_fY - 7.f);
	Button.vSize = _vec2(20.f, 20.f);

	m_pNotify = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pNotify)
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CQuest::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_questbox"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CQuest::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	return S_OK;
}

CQuest* CQuest::Create(_dev pDevice, _context pContext)
{
	CQuest* pInstance = new CQuest(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CQuest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CQuest::Clone(void* pArg)
{
	CQuest* pInstance = new CQuest(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CQuest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuest::Free()
{
	__super::Free();

	for (auto& iter : m_QuestMap)
	{
		Safe_Release(iter.second);
	}
	m_QuestMap.clear();

	Safe_Release(m_pButton);
	Safe_Release(m_pNotify);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}