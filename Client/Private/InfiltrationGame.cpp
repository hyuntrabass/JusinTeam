#include "InfiltrationGame.h"

#include "UI_Manager.h"
#include "Camera_Manager.h"
#include "Guard.h"
#include "GuardTower.h"

CInfiltrationGame::CInfiltrationGame(_dev pDevice, _context pContext)
	:CGameObject(pDevice, pContext)
{
}

CInfiltrationGame::CInfiltrationGame(const CInfiltrationGame& rhs)
	:CGameObject(rhs)
{
}

HRESULT CInfiltrationGame::Init_Prototype()
{
	return S_OK;
}

HRESULT CInfiltrationGame::Init(void* pArg)
{
	
	const TCHAR* pGetPath = TEXT("../Bin/Data/MiniDungeon_GuardData.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MessageBox(g_hWnd, L"파일을 찾지 못했습니다.", L"파일 로드 실패", MB_OK);
		return E_FAIL;
	}

	_uint GuardListSize;
	inFile.read(reinterpret_cast<char*>(&GuardListSize), sizeof(_uint));


	for (_uint i = 0; i < GuardListSize; ++i)
	{
		_ulong GuardPrototypeSize;
		inFile.read(reinterpret_cast<char*>(&GuardPrototypeSize), sizeof(_ulong));

		wstring GuardPrototype;
		GuardPrototype.resize(GuardPrototypeSize);
		inFile.read(reinterpret_cast<char*>(&GuardPrototype[0]), GuardPrototypeSize * sizeof(wchar_t));

		_mat GuardWorldMat;
		inFile.read(reinterpret_cast<char*>(&GuardWorldMat), sizeof(_mat));

		_uint iPattern = 0;
		inFile.read(reinterpret_cast<char*>(&iPattern), sizeof(_int));
	
		if (GuardPrototype == L"Prototype_Model_Guard")
		{
			GuardInfo GuardInfo{};
			GuardInfo.mMatrix = GuardWorldMat;
			GuardInfo.iIndex = iPattern;

			CGuard* pGuard = dynamic_cast<CGuard*>(m_pGameInstance->Clone_Object(L"Prototype_GameObject_Guard", &GuardInfo));
			m_Guard.push_back(pGuard);
			
		}
		if (GuardPrototype == L"Prototype_Model_StoneTower")
		{
			GuardTowerInfo GuardTowerInfo{};
			GuardTowerInfo.mMatrix = GuardWorldMat;
			GuardTowerInfo.iIndex = iPattern;

			CGuardTower* pGuardTower = dynamic_cast<CGuardTower*>(m_pGameInstance->Clone_Object(L"Prototype_GameObject_GuardTower", &GuardTowerInfo));
			m_GuardTower.push_back(pGuardTower);
		}

	}

	inFile.close();

	return S_OK;

}

void CInfiltrationGame::Tick(_float fTimeDelta)
{

	for (auto& pGuard : m_Guard)
	{
		pGuard->Tick(fTimeDelta);
	}

	for (auto& pGuardTower : m_GuardTower)
	{
		pGuardTower->Tick(fTimeDelta);
	}

	Release_DeadObjects();
}

void CInfiltrationGame::Late_Tick(_float fTimeDelta)
{
	for (auto& pGuard : m_Guard)
	{
		pGuard->Late_Tick(fTimeDelta);
	}

	for (auto& pGuardTower : m_GuardTower)
	{
		pGuardTower->Late_Tick(fTimeDelta);
	}
}



void CInfiltrationGame::Release_DeadObjects()
{
	for (auto& it = m_Guard.begin(); it != m_Guard.end();)
	{
		if ((*it)->isDead())
		{
			Safe_Release(*it);
			it = m_Guard.erase(it);
		}
		else
		{
			++it;
		}
	}
	for (auto& it = m_GuardTower.begin(); it != m_GuardTower.end();)
	{
		if ((*it)->isDead())
		{
			Safe_Release(*it);
			it = m_GuardTower.erase(it);
		}
		else
		{
			++it;
		}
	}
}

CInfiltrationGame* CInfiltrationGame::Create(_dev pDevice, _context pContext)
{
	CInfiltrationGame* pInstance = new CInfiltrationGame(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CInfiltrationGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInfiltrationGame::Clone(void* pArg)
{
	CInfiltrationGame* pInstance = new CInfiltrationGame(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CInfiltrationGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInfiltrationGame::Free()
{
	__super::Free();
	for (auto& pGuard : m_Guard)
	{
		Safe_Release(pGuard);
	}
	m_Guard.clear();

	for (auto& pGuardTower : m_GuardTower)
	{
		Safe_Release(pGuardTower);
	}
	m_GuardTower.clear();
}
