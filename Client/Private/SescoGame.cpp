#include "SescoGame.h"
#include "VTFMonster.h"

CSescoGame::CSescoGame(_dev pDevice, _context pContext)
	:CGameObject(pDevice, pContext)
{
}

CSescoGame::CSescoGame(const CSescoGame& rhs)
	:CGameObject(rhs)
{
}

HRESULT CSescoGame::Init_Prototype()
{
	return S_OK;
}

HRESULT CSescoGame::Init(void* pArg)
{
	return S_OK;
}

void CSescoGame::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_9))
	{
		Kill();
	}

	m_fTimeLimit -= fTimeDelta;

	for (auto& pMonster : m_Monsters)
	{
		pMonster->Tick(fTimeDelta);
	}

	if (m_Monsters.size() > m_iMonsterLimit || m_fTimeLimit <= 0.f)
	{
		Kill();
	}

	Release_DeadObjects();
}

void CSescoGame::Late_Tick(_float fTimeDelta)
{
	for (auto& pMonster : m_Monsters)
	{
		pMonster->Late_Tick(fTimeDelta);
	}
}

void CSescoGame::Release_DeadObjects()
{
	for (auto& it = m_Monsters.begin(); it != m_Monsters.end();)
	{
		if ((*it)->isDead())
		{
			Safe_Release(*it);
			it = m_Monsters.erase(it);
		}
		else
		{
			++it;
		}
	}
}

CSescoGame* CSescoGame::Create(_dev pDevice, _context pContext)
{
	CSescoGame* pInstance = new CSescoGame(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSescoGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSescoGame::Clone(void* pArg)
{
	CSescoGame* pInstance = new CSescoGame(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSescoGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSescoGame::Free()
{
	__super::Free();

	for (auto& pMonster : m_Monsters)
	{
		pMonster->Free();
	}
}
