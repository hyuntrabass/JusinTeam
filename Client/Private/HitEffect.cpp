#include "HitEffect.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CHitEffect::CHitEffect(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CHitEffect::CHitEffect(const CHitEffect& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CHitEffect::Init_Prototype()
{
	return S_OK;
}

HRESULT CHitEffect::Init(void* pArg)
{

	m_fDepth = (_float)D_NAMETAG / (_float)D_END;

	m_iDamage = ((HITEFFECT_DESC*)pArg)->iDamage;
	m_vTextPosition = ((HITEFFECT_DESC*)pArg)->vTextPosition;
	//m_vColor = ((NAMETAG_DESC*)pArg)->vColor;HITEFFECT_DESC
	m_pParentTransform = ((HITEFFECT_DESC*)pArg)->pParentTransform;
	Safe_AddRef(m_pParentTransform);

	m_fSizeX = 150.f;
	m_fSizeY = 150.f;
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}	

	return S_OK;
}

void CHitEffect::Tick(_float fTimeDelta)
{
	static _float z =-0.f;
	z += fTimeDelta;


	if (m_fSizeX <= 30.f)
	{
		m_fTime += fTimeDelta;
		if (m_fTime >= 2.f)
		{
			m_isDead;
		}
		if (m_fTime >= 0.5f)
		{
			m_fAlpha -= 2.f * fTimeDelta;
			m_vTextPosition.y += 2.f * fTimeDelta;
		}
		//m_fSizeX = 150.f;
		//m_fSizeY = 150.f;
	}
	else
	{
		m_fSizeX -= 120.f * fTimeDelta;
		m_fSizeY -= 120.f * fTimeDelta;
		m_fAlpha = 1.f;// Lerp(0.f, 1.f, (120.f - m_fSizeX) / 120.f);
	}



	/*
	
	m_pTransformCom->Set_State(State::Pos, m_pParentTransform->Get_State(State::Pos) + m_vTextPosition);
	_vec2 v2DPos = __super::Convert_To_2D(m_pTransformCom);
	m_fX = v2DPos.x;
	m_fY = v2DPos.y;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	*/
}

void CHitEffect::Late_Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CHitEffect::Render()
{

	wstring strNum = to_wstring(m_iDamage);
	string str(strNum.begin(), strNum.end());
	for (size_t j = 0; j < str.length(); ++j)
	{
		string strTest = str.substr(j, 1);
		m_iNumIdx = stoi(strTest);


		m_pTransformCom->Set_State(State::Pos, m_pParentTransform->Get_State(State::Pos) + m_vTextPosition);
		_vec2 v2DPos = __super::Convert_To_2D(m_pTransformCom);
		m_fX = v2DPos.x + (25.f * j);
		m_fY = v2DPos.y;
		__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

		if (FAILED(Bind_ShaderResources()))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(VTPass_LerpColorNAlpha)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pVIBufferCom->Render()))
		{
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CHitEffect::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_NumDamage"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CHitEffect::Bind_ShaderResources()
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iNumIdx)))
	{
		return E_FAIL;
	}

	_vec4 vColor = _vec4(1.f, 0.2f, 0.2f, 1.f);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof _vec4)))
	{
		return E_FAIL;
	}	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CHitEffect* CHitEffect::Create(_dev pDevice, _context pContext)
{
	CHitEffect* pInstance = new CHitEffect(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CHitEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHitEffect::Clone(void* pArg)
{
	CHitEffect* pInstance = new CHitEffect(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CHitEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHitEffect::Free()
{
	__super::Free();

	Safe_Release(m_pParentTransform);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
