#include "Dialog.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"

CDialog::CDialog(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CDialog::CDialog(const CDialog& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CDialog::Init_Prototype()
{
	return S_OK;
}

HRESULT CDialog::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_strText = ((DIALOG_DESC*)pArg)->strText;
	m_eLevel = ((DIALOG_DESC*)pArg)->eLevelID;
	m_vPosition = ((DIALOG_DESC*)pArg)->vPosition;
	m_pParentTransform = ((DIALOG_DESC*)pArg)->pParentTransform;
	Safe_AddRef(m_pParentTransform);

	_vec2 vTextSize = m_pGameInstance->Get_TextSize(L"Font_Malang", m_strText);
	//엔진에서 받아오는게... ?

	m_fSizeX = vTextSize.x;
	m_fSizeY = vTextSize.y + 20.f;
	m_fDepth = (_float)D_NAMETAG / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);


	return S_OK;
}

void CDialog::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_State(State::Pos, m_pParentTransform->Get_State(State::Pos) + m_vPosition);	
	m_vTextPos = __super::Convert_To_2D(m_pTransformCom);
	m_fX = m_vTextPos.x;
	m_fY = m_vTextPos.y;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CDialog::Late_Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CDialog::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_NineSlice)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	_vec2 TextPos = m_vTextPos;
	/*
	m_pGameInstance->Render_Text(L"Font_Malang", m_strText ,TextPos, 0.3f);
	m_pGameInstance->Render_Text(L"Font_Malang", m_strText ,TextPos, 0.3f);
	m_pGameInstance->Render_Text(L"Font_Malang", m_strText ,TextPos, 0.3f);
	m_pGameInstance->Render_Text(L"Font_Malang", m_strText ,TextPos, 0.3f);
	*/
	m_pGameInstance->Render_Text(L"Font_Malang", m_strText, TextPos, 0.35f);

	return S_OK;
}

HRESULT CDialog::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_Dialog"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDialog::Bind_ShaderResources()
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

	_vec4 vSliceRect = _vec4(0.2f, 0.2f, 0.8f, 0.8f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vSliceRect", &vSliceRect, sizeof(_vec4))))
	{
		return E_FAIL;
	}
	
	_float2 vRatio =_float2(m_fSizeX, m_fSizeY);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vRatio", &vRatio, sizeof(_float2))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CDialog* CDialog::Create(_dev pDevice, _context pContext)
{
	CDialog* pInstance = new CDialog(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CDialog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDialog::Clone(void* pArg)
{
	CDialog* pInstance = new CDialog(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CDialog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDialog::Free()
{
	__super::Free();

	Safe_Release(m_pParentTransform);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
