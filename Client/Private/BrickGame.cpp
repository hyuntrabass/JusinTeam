#include "BrickGame.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"
#include "FadeBox.h"
#include "Event_Manager.h"
#include "Vehicle.h"
#include "Camera_Manager.h"
#include "Balloon.h"
#include "InfinityTower.h"
#include "NumEffect.h"
#include "BrickWall.h"
#include "BrickBar.h"
#include "BrickBall.h"
#include "Effect_Manager.h"
#include "TextButtonColor.h"
#include "BlackCat.h"

CBrickGame::CBrickGame(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CBrickGame::CBrickGame(const CBrickGame& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CBrickGame::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CBrickGame::Init(void* pArg)
{

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	Init_Game();


	return S_OK;
}

void CBrickGame::Tick(_float fTimeDelta)
{
	//중심 DirectX::XMFLOAT4 = {x=-2000.70496 y=11.4677677 z=-1999.06152 ...}
	//왼쪽벽DirectX::XMFLOAT4 = {x=-1991.24585 y=11.4677677 z=-2000.09204 ...}
	//위쪽 벽 DirectX::XMFLOAT4 = {x=-2000.19641 y=11.4677696 z=-2007.11536 ...}
	//DirectX::XMFLOAT4 = {x=-2000.28052 y=11.3177662 z=-1991.18335 ...}


	if (!CUI_Manager::Get_Instance()->InfinityTower_UI(true, BRICK))
	{
		return;
	}


	CUI_Manager::Get_Instance()->Set_FullScreenUI(true);

	if (CTrigger_Manager::Get_Instance()->Get_CurrentSpot() != TS_BrickMap)
	{
		Kill();
	}

	if (m_pGameInstance->Key_Down(DIK_ESCAPE))
	{
		DestroyWindow(g_hWnd);
	}

	m_fTime += fTimeDelta;
	if (!m_ITemList.empty() && m_ITemList.front() == CBrickItem::STOP && m_fStopTime <= 3.f)
	{
		m_fStopTime += fTimeDelta;
		m_fTime = 0.f;
		m_pTimeBarLock->Tick(fTimeDelta);
	}
	if (m_fStopTime >= 3.f)
	{
		m_fStopTime = 0.f;
		m_ITemList.pop_front();
	}

	if (m_fTime >= 1.f)
	{
		if (m_iSec <= 0)
		{
			m_iMinute -= 1;
			m_iSec = 60;
			if (m_iMinute < 0)
			{
				m_iMinute = 0;
				m_isGameOver = true;
				return;
			}
		}
		else
		{
			m_iSec -= 1;
		}
		m_fTime = 0.f;
	}

	if (m_pCatBoss)
	{
		m_pCatBoss->Tick(fTimeDelta);
	}
	if (m_pBackGround)
	{
		m_pBackGround->Tick(fTimeDelta);
	}

	if (m_pCatBoss && !m_pCatBoss->Is_GameStart())
	{
		return;
	}
	if (m_pCatBoss && m_pCatBoss->Create_Bricks())
	{
		Create_Bricks();

	}
	

	Tick_Ball(fTimeDelta);

	if (!m_ITemList.empty())
	{
		m_isReadyItem = true;
	}
	else
	{
		m_isReadyItem = false;
	}

	

	if (m_isReadyItem)
	{
		if (m_pItemSlotEffect->Get_Alpha() > 1.f)
		{
			m_fDir = -1.f;
		}
		if (m_pItemSlotEffect->Get_Alpha() < 0.5f)
		{
			m_fDir = 1.f;
		}
		m_pItemSlotEffect->Set_Alpha(m_pItemSlotEffect->Get_Alpha() + fTimeDelta * m_fDir * 2.f);
		m_pItemSlotEffect->Tick(fTimeDelta);
	}


	m_pBackGround->Tick(fTimeDelta);


	if (m_pBall)
	{
		m_pBall->Tick(fTimeDelta);
	}
	if (m_pBar)
	{
		m_pBar->Tick(fTimeDelta);
	}
	if (m_pCombo)
	{
		m_pCombo->Set_TargetNum(m_iCombo);
		m_pCombo->Tick(fTimeDelta);
	}

	if (m_isDead)
	{
		if (m_Light_Desc.eType != LIGHT_DESC::TYPE::End)
		{
			LIGHT_DESC* LightDesc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, TEXT("Light_Main"));
			*LightDesc = m_Light_Desc;
		}

		CCamera_Manager::Get_Instance()->Set_CameraState(CS_DEFAULT);
	}

	m_EffectMatrix = _mat::CreateTranslation(_vec3(-1989.f, 0.f, -2005.11536f));
}

void CBrickGame::Late_Tick(_float fTimeDelta)
{

	if (!m_isActive)
	{
		return;
	}

	m_pItemSlot->Late_Tick(fTimeDelta);
	if (m_isReadyItem)
	{
		if (!m_ITemList.empty())
		{
			m_pItem[m_ITemList.front()]->Late_Tick(fTimeDelta);
		}
		m_pItemSlotEffect->Late_Tick(fTimeDelta);
	}

	if (m_fTimeLimit > 0.f)
	{
		m_pTimeBarLock->Late_Tick(fTimeDelta);
	}
	if (m_pBackGround)
	{
		m_pBackGround->Late_Tick(fTimeDelta);
	}
	if (m_pCatBoss)
	{
		m_pCatBoss->Late_Tick(fTimeDelta);
	}

	if (m_pCatBoss && !m_pCatBoss->Is_GameStart())
	{
		return;
	}

	m_pTimeBar->Late_Tick(fTimeDelta);

	if (m_pBall)
	{
		m_pBall->Late_Tick(fTimeDelta);
	}
	if (m_pBar)
	{
		m_pBar->Late_Tick(fTimeDelta);
	}
	if (m_pCombo)
	{
		m_pCombo->Late_Tick(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CBrickGame::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	wstring strMin = to_wstring(m_iMinute);
	if (m_iMinute == 60)
	{
		strMin = to_wstring(0);
	}
	wstring strSec = to_wstring(m_iSec);
	wstring strText = strMin + TEXT(" : ") + strSec;
	_vec4 vColor{};
	if (m_iMinute < 1 && 60 - m_iSec <= 30)
	{
		vColor = _vec4(1.f, 0.f, 0.f, 1.f);
	}
	else
	{
		vColor = _vec4(1.f, 1.f, 1.f, 1.f);
	}

	m_pGameInstance->Render_Text(L"Font_Malang", strText, _vec2((_float)g_ptCenter.x, 38.f), 0.8f, vColor);

	return S_OK;
}



HRESULT CBrickGame::Add_Parts()
{
	if (m_pGameInstance->Get_LayerSize(LEVEL_TOWER, TEXT("Layer_BrickWall")) == 0)
	{
		CBrickWall::WALL_DESC WallDesc{};

		WallDesc.rcRect = { (_long)1.f, (_long)0.f, (_long)0.f, (_long)0.f };
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_BrickWall"), TEXT("Prototype_GameObject_BrickWall"), &WallDesc)))
		{
			return E_FAIL;
		}
		WallDesc.rcRect = { (_long)0.f, (_long)1.f, (_long)0.f, (_long)0.f };
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_BrickWall"), TEXT("Prototype_GameObject_BrickWall"), &WallDesc)))
		{
			return E_FAIL;
		}
		WallDesc.rcRect = { (_long)0.f, (_long)0.f, (_long)1.f, (_long)0.f };
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_BrickWall"), TEXT("Prototype_GameObject_BrickWall"), &WallDesc)))
		{
			return E_FAIL;
		}
	}

	_vec3 vStartPos = _vec3(-1989.f, 1.5f, -2005.11536f);
	for (_uint i = 0; i < BRICKROW; i++)
	{
		for (_uint j = 0; j < BRICKCOL; j++)
		{
			_vec2 vCenterPos = _vec2(-2000.f, -2005.f);
			_vec2 vSize = _vec2(8.f, 8.f);
			RECT rcRect = {
				  (LONG)(vCenterPos.x - vSize.x * 0.5f),
				  (LONG)(vCenterPos.y - vSize.y * 0.5f),
				  (LONG)(vCenterPos.x + vSize.x * 0.5f),
				  (LONG)(vCenterPos.y + vSize.y * 0.5f)
			};

			CBalloon::BALLOON_DESC Desc{};
			Desc.vColor = { 0.f, 0.6f, 1.f, 1.f };
			Desc.vPosition = _vec3(vStartPos.x - 2.25f * j, vStartPos.y, vStartPos.z + 2.25f * i);
			POINT ptPos = { (_long)Desc.vPosition.x, (_long)Desc.vPosition.z };
			if (PtInRect(&rcRect, ptPos))
			{
				continue;
			}
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_Balloons"), TEXT("Prototype_GameObject_Balloon"), &Desc)))
			{
				return E_FAIL;
			}
		}

	}

	UiInfo info{};
	info.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_NightSky");
	info.vPos = _vec2((_float)g_iWinSizeX / 2.f, (_float)g_iWinSizeY / 2.f);
	info.vSize = _vec2((_float)g_iWinSizeX, (_float)g_iWinSizeY);
	info.iLevel = (_uint)LEVEL_STATIC;

	m_pBackGround = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BackGround_Mask"), &info);
	if (not m_pBackGround)
	{
		return E_FAIL;
	}

	CNumEffect::NUMEFFECT_DESC NumDesc{};
	NumDesc.bOrth = true;
	NumDesc.iDamage = 0;
	NumDesc.vTextPosition = _vec2((_float)g_iWinSizeX - 200.f, (_float)g_ptCenter.y);
	m_pCombo = (CNumEffect*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NumEffect"), &NumDesc);
	if (not m_pCombo)
	{
		return E_FAIL;
	}

	CTextButtonColor::TEXTBUTTON_DESC ColButtonDesc = {};
	ColButtonDesc.eLevelID = LEVEL_STATIC;
	ColButtonDesc.fDepth = (_float)D_SCREEN / (_float)D_END;
	ColButtonDesc.fFontSize = 0.f;
	ColButtonDesc.strText = TEXT("");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_TimeLimit");
	ColButtonDesc.vSize = _vec2(200.f, 200.f);
	ColButtonDesc.vPosition = _vec2((_float)g_ptCenter.x, 40.);
	ColButtonDesc.fAlpha = 0.7f;


	m_pTimeBar = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pTimeBar)
	{
		return E_FAIL;
	}	
	m_pTimeBar->Set_Pass(VTPass_UI_Alpha);
	
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_TimeLimitLock");

	ColButtonDesc.fDepth = (_float)D_SCREEN / (_float)D_END - 0.01f;
	m_pTimeBarLock = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pTimeBarLock)
	{
		return E_FAIL;
	}	
	m_pTimeBarLock->Set_Pass(VTPass_UI_Alpha);
	

	ColButtonDesc.fDepth = (_float)D_SCREEN / (_float)D_END + 0.01f;
	ColButtonDesc.fFontSize = 0.f;
	ColButtonDesc.strText = TEXT("");
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_BG_SkillSlot_Item");
	ColButtonDesc.vSize = _vec2(200.f, 200.f);
	ColButtonDesc.vPosition = _vec2(130.f, 600.f);
	ColButtonDesc.fAlpha = 0.5f;
	m_pItemSlot = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pItemSlot)
	{
		return E_FAIL;
	}
	m_pItemSlot->Set_Pass(VTPass_UI);

	ColButtonDesc.fDepth = (_float)D_SCREEN / (_float)D_END - 0.01f;
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_BG_SkillSlot_Sellect");
	m_pItemSlotEffect = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pItemSlotEffect)
	{
		return E_FAIL;
	}
	m_pItemSlotEffect->Set_Pass(VTPass_UI_Alpha);

	ColButtonDesc.fDepth = (_float)D_SCREEN / (_float)D_END;
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_Double");
	ColButtonDesc.vSize = _vec2(120.f, 120.f);
	ColButtonDesc.vPosition = _vec2(130.f, 572.f);
	ColButtonDesc.fAlpha = 0.5f;
	m_pItem[CBrickItem::DOUBLE] = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pItem[CBrickItem::DOUBLE])
	{
		return E_FAIL;
	}
	m_pItem[CBrickItem::DOUBLE]->Set_Pass(VTPass_UI);

	
	
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_Power");
	m_pItem[CBrickItem::POWER] = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pItem[CBrickItem::POWER])
	{
		return E_FAIL;
	}
	m_pItem[CBrickItem::POWER]->Set_Pass(VTPass_UI);
	
	ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Tower_Stop");
	m_pItem[CBrickItem::STOP] = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pItem[CBrickItem::STOP])
	{
		return E_FAIL;
	}
	m_pItem[CBrickItem::STOP]->Set_Pass(VTPass_UI);

	
	return S_OK;
}

HRESULT CBrickGame::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CBrickGame::Bind_ShaderResources()
{
	return S_OK;
}

void CBrickGame::Init_Game()
{
	m_iMinute = 3;
	m_iSec = 60;

	if (FAILED(Add_Parts()))
	{
		return;
	}

	CCamera_Manager::Get_Instance()->Set_CameraState(CS_BRICKGAME);
	CUI_Manager::Get_Instance()->Set_FullScreenUI(true);
	m_isActive = true;
	CTrigger_Manager::Get_Instance()->Teleport(TS_BrickMap);


	LIGHT_DESC* LightDesc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, TEXT("Light_Main"));

	m_Light_Desc = *LightDesc;
	LightDesc->eType = LIGHT_DESC::Directional;
	LightDesc->vDirection = _float4(-1.f, 0.f, -1.f, 0.f);
	LightDesc->vDiffuse = _vec4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc->vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc->vSpecular = _vec4(1.f);


	if (not m_pBar)
	{
		m_pBar = (CBrickBar*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BrickBar"));
	}
	if (m_pBall == nullptr)
	{

		CBalloon::BALLOON_DESC Desc{};
		CTransform* pTransform = m_pBar->Get_Transform();
		_vec3 vPos = pTransform->Get_State(State::Pos);
		Desc.vColor = { 0.f, 0.6f, 1.f, 1.f };
		Desc.vPosition = _vec3(vPos.x, vPos.y, vPos.z - 2.f);
		m_pBall = (CBrickBall*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BrickBall"), &Desc);
		if (not m_pBall)
		{
			MSG_BOX("BrickBall");
			return;
		}
	}
	if (m_pGameInstance->Get_LayerSize(LEVEL_TOWER, TEXT("Layer_BrickPet")) == 0)
	{
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_BrickPet"), TEXT("Prototype_GameObject_BrickCat"))))
		{
			MSG_BOX("BrickPet");
			return;
		}
	}

	m_pCatBoss = (CBlackCat*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BlackCat"));
	if (not m_pCatBoss)
	{
		return;
	}
	
}

void CBrickGame::Create_Bricks()
{

	_vec3 vStartPos = _vec3(-1989.f, 5.f, -2005.11536f);
	for (_uint i = 0; i < BRICKROW; i++)
	{
		for (_uint j = 0; j < BRICKCOL; j++)
		{
			_vec2 vCenterPos = _vec2(-2000.f, -2005.f);
			_vec2 vSize = _vec2(8.f, 8.f);
			RECT rcRect = {
				  (LONG)(vCenterPos.x - vSize.x * 0.5f),
				  (LONG)(vCenterPos.y - vSize.y * 0.5f),
				  (LONG)(vCenterPos.x + vSize.x * 0.5f),
				  (LONG)(vCenterPos.y + vSize.y * 0.5f)
			};

			CBalloon::BALLOON_DESC Desc{};
			Desc.isEmpty = true;
			Desc.vColor = { 0.f, 0.6f, 1.f, 1.f };
			Desc.vPosition = _vec3(vStartPos.x - 2.25f * j, vStartPos.y, vStartPos.z + 2.25f * i);
			POINT ptPos = { (_long)Desc.vPosition.x, (_long)Desc.vPosition.z };
			if (PtInRect(&rcRect, ptPos))
			{
				continue;
			}
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_TOWER, TEXT("Layer_Balloons"), TEXT("Prototype_GameObject_Balloon"), &Desc)))
			{
				return;
			}
		}

	}
}

CBrickGame* CBrickGame::Create(_dev pDevice, _context pContext)
{
	CBrickGame* pInstance = new CBrickGame(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBrickGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBrickGame::Clone(void* pArg)
{
	CBrickGame* pInstance = new CBrickGame(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBrickGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CBrickGame::Find_Component(const wstring& strComTag)
{
	if (strComTag == TEXT("BrickBall"))
	{
		if (not m_pBall)
		{
			return nullptr;
		}
		return m_pBall->Get_BrickBallCollider();
	}
	else if (strComTag == TEXT("BrickBarCol"))
	{
		if (not m_pBar)
		{
			return nullptr;
		}
		return m_pBar->Get_BrickBarCollider();
	}
	else if (strComTag == TEXT("BrickBarTransform"))
	{
		if (not m_pBar)
		{
			return nullptr;
		}
		return m_pBar->Get_Transform();
	}
	else if (strComTag == TEXT("BlackCat"))
	{
		if (not m_pCatBoss)
		{
			return nullptr;
		}
		return m_pCatBoss->Get_Collider();
	}
	else
	{
		auto& it = m_Components.find(strComTag);
		if (it == m_Components.end())
		{
			return nullptr;
		}

		return it->second;
	}
}

void CBrickGame::Tick_Ball(_float fTimeDelta)
{

	if ((m_pBall && m_pBall->Is_Dead()) || (m_pBall && m_pGameInstance->Key_Down(DIK_RETURN)))
	{
		Safe_Release(m_pBall);
		if (!m_ITemList.empty())
		{
			while (!m_ITemList.empty())
			{
				m_ITemList.pop_back();
			}
			m_isReadyItem = false;
		}


		return;
	}

	if (not m_pBall && m_pGameInstance->Key_Down(DIK_B, InputChannel::GamePlay)) //나중에는 space 해제
	{
		m_iCombo = 0;
		CBrickBall::BALL_DESC Desc{};
		CTransform* pTransform = m_pBar->Get_Transform();
		_vec3 vPos = pTransform->Get_State(State::Pos);
		Desc.vPos = _vec3(vPos.x, vPos.y, vPos.z - 2.f);
		Desc.eBrickColor = m_pBar->Get_CurrentColor();
		m_pBall = (CBrickBall*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BrickBall"), &Desc);
		if (not m_pBall)
		{
			return;
		}
	}


	if (m_fComboTime > 2.f || not m_pBall)
	{
		m_iCombo = 0;
		m_fComboTime = 0.f;
	}

	if (m_pBall)
	{
		if (m_pBall->Is_Combo() && m_fComboTime <= 2.f)
		{
			m_fComboTime = 0.f;
			m_iCombo++;
		}
	}

	if (m_pBall != nullptr)
	{

		if (m_pBall->Is_BarColl())
		{
			m_pBall->Set_CurrentBallColor(m_pBar->Get_CurrentColor());
		}

	}


	if (m_pBall)
	{
		CBrickItem::TYPE eType = m_pBall->Get_CurItem();
		if (eType != CBrickItem::TYPE_END)
		{
			m_ITemList.push_back(eType);
		}
	}
}

void CBrickGame::Free()
{
	__super::Free();

	if (!m_isPrototype)
	{
		for (size_t i = 0; i < CBrickItem::TYPE_END; i++)
		{
			Safe_Release(m_pItem[i]);
		}
	}

	Safe_Release(m_pTimeBarLock);
	Safe_Release(m_pItemSlot);
	Safe_Release(m_pItemSlotEffect);

	Safe_Release(m_pCatBoss);
	Safe_Release(m_pBall);
	Safe_Release(m_pBar);

	Safe_Release(m_pTimeBar);
	Safe_Release(m_pCombo);
	Safe_Release(m_pBackGround);

	Safe_Release(m_pRendererCom);
}
