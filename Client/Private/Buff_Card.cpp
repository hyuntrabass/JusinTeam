#include "Buff_Card.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "NineSlice.h"
#include "UI_Manager.h"
#include "SummonWindow.h"
#include "SummonWindowPet.h"
#include "Item.h"

CBuff_Card::CBuff_Card(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CBuff_Card::CBuff_Card(const CBuff_Card& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CBuff_Card::Init_Prototype()
{
	return S_OK;
}

HRESULT CBuff_Card::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	m_fDepth = (_float)D_WINDOW / (_float)D_END;
	m_fSizeX = 300.f;
	m_fSizeY = 300.f;

	m_fX = ((BUFFCARD_DESC*)pArg)->vPos.x;
	m_fY = ((BUFFCARD_DESC*)pArg)->vPos.y;

	m_eBuff = ((BUFFCARD_DESC*)pArg)->eBuff;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	

	m_rcRect = {
		  (LONG)(m_fX - m_fSizeX * 0.5f),
		  (LONG)(m_fY - m_fSizeY * 0.5f),
		  (LONG)(m_fX + m_fSizeX * 0.5f),
		  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CBuff_Card::Tick(_float fTimeDelta)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	
	if (m_isReadyFade)
	{
		if (m_IsSelect)
		{
			return;
		}

		if (m_pBackGround->Get_Alpha() <= 0.5f)
		{
			if (m_pText)
			{
				if (m_pText->Get_Alpha() > 0.f)
				{
					m_pText->Set_Alpha(m_pText->Get_Alpha() - fTimeDelta * 5.f);
				}
				m_pText->Tick(fTimeDelta);
			}
		}
		if (m_pBackGround->Get_Alpha() <= 0.2f)
		{
			m_isFadeEnd = true;
			return;
		}
		_float fSpeedAlpha = 1.f;
		_float fSpeed = 90.f;
		m_pIcon->Set_Alpha(m_pIcon->Get_Alpha() - fTimeDelta * fSpeedAlpha);
		m_pBorder->Set_Alpha(m_pBorder->Get_Alpha() - fTimeDelta * fSpeedAlpha);
		m_pBackGround->Set_Alpha(m_pBackGround->Get_Alpha() - fTimeDelta * fSpeedAlpha);
		
		m_pIcon->Set_Position(_vec2(m_pIcon->Get_TransPosition().x , m_pIcon->Get_TransPosition().y + fTimeDelta * fSpeed ));
		m_pBorder->Set_Position(_vec2(m_pBorder->Get_TransPosition().x, m_pBorder->Get_TransPosition().y + fTimeDelta * fSpeed));
		m_pBackGround->Set_Position(_vec2(m_pBackGround->Get_Position().x, m_pBackGround->Get_Position().y + fTimeDelta * fSpeed));
		
		m_pIcon->Tick(fTimeDelta);
		m_pBorder->Tick(fTimeDelta);
		m_pBackGround->Tick(fTimeDelta);
		return;
	}

	if (PtInRect(&m_pBackGround->Get_Rect(), ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
	{
		m_isReadyClick = true;
		m_isPicking = true;
	}
	else if (!PtInRect(&m_pBackGround->Get_Rect(), ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
	{
		m_isPicking = false;
	}

	m_fTime += fTimeDelta;


	if (m_fTime >= 0.5f && m_isReadyClick)
	{
		m_fTime = 0.f;
		m_isReadyClick = false;
	}

	if (m_isReadyClick && PtInRect(&m_pBackGround->Get_Rect(), ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
	{
		m_IsSelect = true;
		return;
	}

	if(!m_isPicking)
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);


	if (m_isPicking)
	{
		m_pSelect->Tick(fTimeDelta);
	}

	if (m_pFade)
	{
		m_pFade->Tick(fTimeDelta);
	}
	if (m_pText)
	{
		if (m_pText->Get_Alpha() <= 1.f)
		{
			m_pText->Set_Alpha(m_pText->Get_Alpha() + fTimeDelta * 5.f);
		}
		m_pText->Tick(fTimeDelta);
	}

	m_pIcon->Tick(fTimeDelta);
	m_pBorder->Tick(fTimeDelta);
	m_pBackGround->Tick(fTimeDelta);
}

void CBuff_Card::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);

	if (m_isPicking)
	{
		m_pSelect->Late_Tick(fTimeDelta);
	}	
	
	if (m_pFade)
	{
		m_pFade->Late_Tick(fTimeDelta);
	}
	if (m_pText)
	{
		m_pText->Late_Tick(fTimeDelta);
	}
	m_pIcon->Late_Tick(fTimeDelta);
	m_pBorder->Late_Tick(fTimeDelta);
	m_pBackGround->Late_Tick(fTimeDelta);
}

HRESULT CBuff_Card::Render()
{
	
	
	return S_OK;
}


HRESULT CBuff_Card::Add_Parts()
{
	_uint iTexIndex{};
	_bool isFade{};
	switch (m_eBuff)
	{
	case Client::Buff_MaxHp:
		m_strText = TEXT("최대 체력 400 증가");
		m_fStatus = 400.f;
		iTexIndex = 87;
		isFade = true;
		break;
	case Client::Buff_HpRegen:
		m_strText = TEXT("체력 재생 10 증가");
		m_fStatus = 10.f;
		iTexIndex = 1;
		isFade = true;
		break;
	case Client::Buff_MpRegen:
		m_strText = TEXT("마나 재생 10 증가");
		m_fStatus = 10.f;
		iTexIndex = 2;
		isFade = true;
		break;
	case Client::Buff_Attack:
		m_strText = TEXT("공격력 20 증가");
		m_fStatus = 20.f;
		iTexIndex = 7;
		break;
	case Client::Buff_Speed:
		m_strText = TEXT("이동속도 2 증가");
		m_fStatus = 2.f;
		iTexIndex = 6;
		break;
	case Client::Buff_CoolDown:
		m_strText = TEXT("쿨타임 50% 감소");
		m_fStatus = 0.5f;
		iTexIndex = 112;
		break;
	case Client::Buff_BloodDrain:
		m_strText = TEXT("생명력 흡수 20%");
		m_fStatus = 0.2f;
		iTexIndex = 131;
		break;
	case Client::Buff_PoisonImmune:
		m_strText = TEXT("독,슬로우 상태 면역");
		m_fStatus = 1.f;
		iTexIndex = 237;
		break;
	case Client::Buff_MonRegenDown:
		m_strText = TEXT("몬스터 생성 속도 감소");
		m_fStatus = 0.f;
		iTexIndex = 66;
		break;
	case Client::Buff_End:
		break;
	default:
		break;
	}


	

	CNineSlice::SLICE_DESC SliceDesc{};
	SliceDesc.eLevelID = LEVEL_STATIC;
	SliceDesc.fDepth = m_fDepth + 0.02f;
	SliceDesc.fFontSize = 0.f;
	SliceDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_BuffBg2");
	SliceDesc.strText = TEXT("");
	SliceDesc.vPosition = _vec2(m_fX, m_fY);
	SliceDesc.vSize = _vec2(180.f, 300.f);
	m_pBackGround = (CNineSlice*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NineSlice"), &SliceDesc);
	if (not m_pBackGround)
	{
		return E_FAIL;
	}

	m_pBackGround->Set_Pass(VTPass_SpriteAlpha);
	m_pBackGround->Set_Alpha(1.f);

	SliceDesc.fDepth = m_fDepth + 0.015f;
	SliceDesc.fFontSize = 0.f;
	SliceDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BloomRectBlue");
	SliceDesc.strText = TEXT("");
	SliceDesc.vPosition = _vec2(m_fX, m_fY);
	SliceDesc.vSize = _vec2(180.f, 300.f);
	m_pSelect = (CNineSlice*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NineSlice"), &SliceDesc);
	if (not m_pSelect)
	{
		return E_FAIL;
	}

	CTextButtonColor::TEXTBUTTON_DESC ColButtonDesc = {};
	ColButtonDesc.eLevelID = LEVEL_STATIC;
	ColButtonDesc.fDepth = m_fDepth;
	ColButtonDesc.fAlpha = 1.f;
	ColButtonDesc.strText = TEXT("");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_Buff");
	ColButtonDesc.vSize = _vec2(50.f, 50.f);
	ColButtonDesc.vPosition = _vec2(m_fX, m_fY - 55.f);
	ColButtonDesc.vColor = _vec4(0.31f, 0.96f, 1.f, 1.f);

	m_pIcon = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pIcon)
	{
		return E_FAIL;
	}
	m_pIcon->Set_Pass(VTPass_Sprite);
	m_pIcon->Set_Sprite(_int2(16, 16));
	m_pIcon->Set_Index((_float)iTexIndex);

	ColButtonDesc.eLevelID = LEVEL_STATIC;
	ColButtonDesc.fDepth = m_fDepth + 0.01f;
	ColButtonDesc.fFontSize = 0.4f;
	ColButtonDesc.strText = m_strText;
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_Bg_Result_Rps");
	ColButtonDesc.vPosition = _vec2(m_fX,  m_fY - 50.f);
	ColButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ColButtonDesc.vTextPosition = _vec2(0.f, 100.f);
	ColButtonDesc.vSize = _vec2(150.f, 150.f);

	m_pBorder = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pBorder)
	{
		return E_FAIL;
	}
	m_pBorder->Set_Pass(VTPass_UI_Alpha);
	

	if (isFade)
	{
		ColButtonDesc.fDepth = m_fDepth;
		ColButtonDesc.fFontSize = 0.4f;
		ColButtonDesc.strText = TEXT("");
		ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_BuffText");
		ColButtonDesc.vPosition = _vec2(640.f, 120.f);
		ColButtonDesc.vSize = _vec2(100.f, 100.f);
		ColButtonDesc.fAlpha = 0.f;

		m_pText = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
		if (not m_pText)
		{
			return E_FAIL;
		}
		m_pText->Set_Pass(VTPass_UI_Alpha);

		CFadeBox::FADE_DESC Desc = {};
		Desc.fMaxAlpha = 0.7f;
		Desc.isInfiniteLoop = true;
		Desc.fDepth = (m_fDepth + 0.05f);
		Desc.fIn_Duration = 0.5f;
		m_pFade = CUI_Manager::Get_Instance()->Clone_FadeBox(Desc);
		if (not m_pFade)
		{
			return E_FAIL;
		}

	}

	return S_OK;
}

HRESULT CBuff_Card::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CBuff_Card::Bind_ShaderResources()
{
	return S_OK;
}

CBuff_Card* CBuff_Card::Create(_dev pDevice, _context pContext)
{
	CBuff_Card* pInstance = new CBuff_Card(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBuff_Card::Clone(void* pArg)
{
	CBuff_Card* pInstance = new CBuff_Card(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBuff_Card::Free()
{
	__super::Free();

	Safe_Release(m_pText);
	Safe_Release(m_pIcon);
	Safe_Release(m_pFade);
	Safe_Release(m_pBorder);
	Safe_Release(m_pSelect);
	Safe_Release(m_pBackGround);
	Safe_Release(m_pRendererCom);
}
