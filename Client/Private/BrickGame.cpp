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

//const _float CBrickGame::m_iRow = 7;
//const _float CBrickGame::m_iCOl = 7;

CBrickGame::CBrickGame(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBrickGame::CBrickGame(const CBrickGame& rhs)
	: CGameObject(rhs)
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

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	
	return S_OK;
}

void CBrickGame::Tick(_float fTimeDelta)
{

	//중심 DirectX::XMFLOAT4 = {x=-2000.70496 y=11.4677677 z=-1999.06152 ...}
	//왼쪽벽DirectX::XMFLOAT4 = {x=-1991.24585 y=11.4677677 z=-2000.09204 ...}
	//위쪽 벽 DirectX::XMFLOAT4 = {x=-2000.19641 y=11.4677696 z=-2007.11536 ...}
	//DirectX::XMFLOAT4 = {x=-2000.28052 y=11.3177662 z=-1991.18335 ...}
	CTransform* pPlayerTransform = dynamic_cast<CTransform*>(m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform")));
	_vec4 vPos = pPlayerTransform->Get_State(State::Pos);
	if (!m_isActive && CUI_Manager::Get_Instance()->Get_CurrentMiniGame() == (TOWER)BRICK)
	{
		Init_Game();
	}

	if (!m_isActive)
	{
		if (CCamera_Manager::Get_Instance()->Get_CameraState() == CS_BRICKGAME)
		{
			CCamera_Manager::Get_Instance()->Set_CameraState(CS_DEFAULT);
		}
		return;
	}

	if (m_isActive && m_pGameInstance->Key_Down(DIK_PGUP))
	{
		CCamera_Manager::Get_Instance()->Set_CameraState(CS_DEFAULT);
		m_isActive = false;
		CUI_Manager::Get_Instance()->Open_InfinityTower(true);
		return;
	}

	if ((m_pBall && m_pBall->Is_Dead()) || (m_pBall && m_pGameInstance->Key_Down(DIK_RETURN)))
	{
		Safe_Release(m_pBall);
		return;
	}

	if (not m_pBall && m_pGameInstance->Key_Down(DIK_B, InputChannel::GamePlay)) //나중에는 space 해제
	{
		m_iCombo = 0;
		CBrickBall::BALL_DESC Desc{};
		CTransform* pTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_VILLAGE, TEXT("Layer_BrickBar"), TEXT("Com_Transform"));
		_vec3 vPos = pTransform->Get_State(State::Pos);
		Desc.vPos = _vec3(vPos.x, vPos.y, vPos.z - 2.f);
		m_pBall = (CBrickBall*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_BrickBall"), &Desc);
		if (not m_pBall)
		{
			return;
		}
	}
	if(m_fComboTime > 2.f || not m_pBall)
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

	/*
	
	for (size_t i = 0; i < BRICKROW; i++)
	{
		for (size_t j = 0; j < BRICKCOL; j++)
		{
			if (m_pBalloon[i][j] != nullptr)
			{
				if (m_pBalloon[i][j]->Is_Coll() && m_fComboTime <1.5f)
				{
					CNumEffect::NUMEFFECT_DESC Desc{};
					Desc.bOrth = true;
					Desc.iDamage = m_iCombo;
					Desc.vTextPosition = _vec2(1000.f, 200.f);
					if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_NumEffect"), TEXT("Prototype_GameObject_NumEffect"), &Desc)))
					{
						return;
					}
					m_fComboTime = 0.f;
					m_iCombo++;					
				}
				m_pBalloon[i][j]->Tick(fTimeDelta);
			}
		}
	}*/
	m_pBackGround->Tick(fTimeDelta);
	if (m_pBall)
	{
		m_pBall->Tick(fTimeDelta);
	}
	if (m_pCombo)
	{
		m_pCombo->Set_TargetNum(m_iCombo);
		m_pCombo->Tick(fTimeDelta);
	}
}

void CBrickGame::Late_Tick(_float fTimeDelta)
{

	if (!m_isActive)
	{
		return;
	}
	//m_pBackGround->Late_Tick(fTimeDelta);
	/*
	
	for (size_t i = 0; i < BRICKROW; i++)
	{
		for (size_t j = 0; j < BRICKCOL; j++)
		{
			if (m_pBalloon[i][j] != nullptr)
			{
				m_pBalloon[i][j]->Late_Tick(fTimeDelta);
			}
		}
	}*/
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);
	if (m_pBall)
	{
		m_pBall->Late_Tick(fTimeDelta);
	}
	if (m_pCombo)
	{
		m_pCombo->Late_Tick(fTimeDelta);
	}
}

HRESULT CBrickGame::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}


	return S_OK;
}



HRESULT CBrickGame::Add_Parts()
{
	CBrickWall::WALL_DESC WallDesc{};
	WallDesc.rcRect = { (_long)1.f, (_long)0.f, (_long)0.f, (_long)0.f };
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_BrickWall"), &WallDesc)))
	{
		return E_FAIL;
	}
	WallDesc.rcRect = { (_long)0.f, (_long)1.f, (_long)0.f, (_long)0.f };
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_BrickWall"), &WallDesc)))
	{
		return E_FAIL;
	}
	WallDesc.rcRect = { (_long)0.f, (_long)0.f, (_long)1.f, (_long)0.f };
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_BrickWall"), &WallDesc)))
	{
		return E_FAIL;
	}
	/*
	WallDesc.rcRect = { (_long)0.f, (_long)0.f, (_long)0.f, (_long)1.f };
	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Wall"), TEXT("Prototype_GameObject_BrickWall"), &WallDesc)))
	{
		return E_FAIL;
	}
	*/

	_vec3 vStartPos = _vec3(-1990.f, 1.5f, -2005.11536f);
	for (_uint i = 0; i < BRICKROW; i++)
	{
		for (_uint j = 0; j < BRICKCOL; j++)
		{
			CBalloon::BALLOON_DESC Desc{};
			Desc.vColor = { 0.f, 0.6f, 1.f, 1.f };
			Desc.vPosition = _vec3(vStartPos.x - 2.0f * j, vStartPos.y, vStartPos.z + 2.0f * i);

			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Balloons"), TEXT("Prototype_GameObject_Balloon"), &Desc)))
			{
				return E_FAIL;
			}
		 /*
			CBalloon * pBalloon = (CBalloon*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Balloon"), &Desc);
			if (pBalloon == nullptr)
			{
				return E_FAIL;
			}
		 */
			//m_pBalloon[i][j] = pBalloon;
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
	NumDesc.vTextPosition = _vec2((_float)g_ptCenter.x - 5.f, 40.f);
	m_pCombo = (CNumEffect*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NumEffect"), &NumDesc);
	if (not m_pCombo)
	{
		return E_FAIL;
	}

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
	CCamera_Manager::Get_Instance()->Set_CameraState(CS_BRICKGAME);
	CUI_Manager::Get_Instance()->Set_FullScreenUI(true);
	m_isActive = true;
	CTrigger_Manager::Get_Instance()->Teleport(TS_Minigame);

	if (m_pGameInstance->Get_LayerSize(LEVEL_VILLAGE, TEXT("Layer_BrickBar")) == 0)
	{
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_BrickBar"), TEXT("Prototype_GameObject_BrickBar"))))
		{
			return;
		}
	}
	if (m_pBall == nullptr)
	{
		CBalloon::BALLOON_DESC Desc{};
		CTransform* pTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_VILLAGE, TEXT("Layer_BrickBar"), TEXT("Com_Transform"));
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
	if (m_pGameInstance->Get_LayerSize(LEVEL_VILLAGE, TEXT("Layer_BrickPet")) == 0)
	{
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_BrickPet"), TEXT("Prototype_GameObject_BrickCat"))))
		{
			MSG_BOX("BrickPet");
			return;
		}
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_BlackCat"), TEXT("Prototype_GameObject_BlackCat"))))
		{
			MSG_BOX("BrickPet");
			return;
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

void CBrickGame::Free()
{
	__super::Free();

	if (!m_isPrototype)
	{
		for (_uint i = 0; i < BRICKROW; i++)
		{
			for (_uint j = 0; j < BRICKCOL; j++)
			{
				Safe_Release(m_pBalloon[i][j]);
			}
		}
	}

	Safe_Release(m_pBall);
	Safe_Release(m_pCombo);
	Safe_Release(m_pBackGround);
	Safe_Release(m_pRendererCom);
}
