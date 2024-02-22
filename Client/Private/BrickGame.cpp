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
	/*
	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	*/



	return S_OK;
}

void CBrickGame::Tick(_float fTimeDelta)
{
	if (!m_isActive && CUI_Manager::Get_Instance()->Get_CurrentMiniGame() == (TOWER)BRICK)
	{
		CUI_Manager::Get_Instance()->Set_FullScreenUI(true);
		m_isActive = true;
		CTrigger_Manager::Get_Instance()->Teleport(TS_Minigame);
	}
	

	if (!m_isActive)
	{
		return;
	}
	if (m_isActive && m_pGameInstance->Key_Down(DIK_PGUP))
	{
		m_isActive = false;
		CUI_Manager::Get_Instance()->Open_InfinityTower(true);
		return;
	}
	

}

void CBrickGame::Late_Tick(_float fTimeDelta)
{
	if (!m_isActive)
	{
		return;
	}

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);
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
	CTransform * pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	_vec3 vPlayerPos = pPlayerTransform->Get_State(State::Pos);

	for (_uint i = 0; i < 3; i++)
	{
		for (_uint j = 0; j < 3; j++)
		{
			CBalloon::BALLOON_DESC Desc{};
			Desc.vColor = { 0.f, 0.6f, 1.f, 1.f };
			Desc.vPosition = _vec3(vPlayerPos.x + 10.f * j, vPlayerPos.y + 1.f, vPlayerPos.z + 10.f * i);

			CBalloon * pBalloon = (CBalloon*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Balloon"), &Desc);
			if (pBalloon == nullptr)
			{
				return E_FAIL;
			}
		}

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

void CBrickGame::Free()
{
	__super::Free();


	Safe_Release(m_pRendererCom);
}