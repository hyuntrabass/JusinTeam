#include "InfiltrationGame.h"

#include "UI_Manager.h"
#include "Camera_Manager.h"
#include "Guard.h"
#include "GuardTower.h"
#include "CheckPoint.h"

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
	m_pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	Safe_AddRef(m_pPlayerTransform);

	m_CheckPointMatrix = m_pPlayerTransform->Get_World_Matrix();

	_tchar* pPath = TEXT("../Bin/Data/MiniDungeon_Guard_1_Data.dat");
	Create_Guard(pPath);
	Create_CheckPoint();
	return S_OK;
}

void CInfiltrationGame::Tick(_float fTimeDelta)
{
	if (!CUI_Manager::Get_Instance()->InfinityTower_UI(true, BOSS1))
	{
		return;
	}

	Reset_Play(fTimeDelta);
	for (auto& pGuardList : m_GuardList)
	{
		for (auto& pGuard : pGuardList)
		{
			pGuard->Tick(fTimeDelta);
	
		}
	}
	for (auto& pGuardTowerList : m_GuardTowerList)
	{
		for (auto& pGuardTower : pGuardTowerList)
		{
			pGuardTower->Tick(fTimeDelta);
		}
	}

	for (auto& pCheckPoint : m_CheckPoint)
	{
		pCheckPoint->Tick(fTimeDelta);

	}

	Release_DeadObjects();
}

void CInfiltrationGame::Late_Tick(_float fTimeDelta)
{
	for (auto& pGuardList : m_GuardList)
	{
		for (auto& pGuard : pGuardList)
		{
			pGuard->Late_Tick(fTimeDelta);
		}
	}

	for (auto& pGuardTowerList : m_GuardTowerList)
	{
		for (auto& pGuardTower : pGuardTowerList)
		{
			pGuardTower->Late_Tick(fTimeDelta);
		}
	}

	for (auto& pCheckPoint : m_CheckPoint)
	{
		if (pCheckPoint->Get_Collision())
		{
			_tchar* pPath = nullptr;
			m_CheckPointMatrix = pCheckPoint->Get_Matrix() + _mat::CreateTranslation(0.f, 1.f, 0.f);
			_int CheckIndex = pCheckPoint->Get_TriggerNum();
			switch (CheckIndex)
			{
			case 0:
				pPath = TEXT("../Bin/Data/MiniDungeon_Guard_2_Data.dat");
				break;
			case 1:
				pPath = TEXT("../Bin/Data/MiniDungeon_Guard_3_Data.dat");

				break;
			case 2:
				pPath = TEXT("../Bin/Data/MiniDungeon_Guard_4_Data.dat");
				break;
			case 3:
				pPath = TEXT("../Bin/Data/MiniDungeon_Guard_5_Data.dat");
				break;
			case 4:
				pPath = TEXT("../Bin/Data/MiniDungeon_Guard_6_Data.dat");
				break;

			
			}
			_int Index = CheckIndex - 1;
			if (0 <= Index) {
				for (auto& pGameObject : m_GuardList[Index])
					pGameObject->Kill();

				for (auto& pGameObject : m_GuardTowerList[Index])
					pGameObject->Kill();
			}

			if(pPath != nullptr)
				Create_Guard(pPath);
			pCheckPoint->Kill();
		}
	}
	
}



HRESULT CInfiltrationGame::Create_Guard(const TCHAR* pPath)
{
	const TCHAR* pGetPath = pPath;

	std::ifstream inFile(pGetPath, std::ios::binary);
	list<class CGuard*> m_Guard;
	list<class CGuardTower*> m_GuardTower;
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
	m_GuardList.push_back(m_Guard);
	m_GuardTowerList.push_back(m_GuardTower);

	inFile.close();

	return S_OK;

}

HRESULT CInfiltrationGame::Create_CheckPoint()
{

	TriggerInfo Info{};
	const TCHAR* pGetPath = TEXT("../Bin/Data/MiniDungeon_CheckPointData.dat");

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MSG_BOX("MiniDungeon_CheckPointData 불러오기 실패.");
		return E_FAIL;
	}
	_uint TriggerListSize;
	inFile.read(reinterpret_cast<char*>(&TriggerListSize), sizeof(_uint));


	for (_uint i = 0; i < TriggerListSize; ++i)
	{
		TriggerInfo TriggerInfo{};

		_uint iIndex{};
		inFile.read(reinterpret_cast<char*>(&iIndex), sizeof(_uint));
		TriggerInfo.iIndex = iIndex;

		_bool bCheck{};
		inFile.read(reinterpret_cast<char*>(&bCheck), sizeof(_bool));
		TriggerInfo.bLimited = bCheck;

		_ulong TriggerPrototypeSize;
		inFile.read(reinterpret_cast<char*>(&TriggerPrototypeSize), sizeof(_ulong));

		wstring TriggerPrototype;
		TriggerPrototype.resize(TriggerPrototypeSize);
		inFile.read(reinterpret_cast<char*>(&TriggerPrototype[0]), TriggerPrototypeSize * sizeof(wchar_t));

		_float TriggerSize{};
		inFile.read(reinterpret_cast<char*>(&TriggerSize), sizeof(_float));
		TriggerInfo.fSize = TriggerSize;

		_mat TriggerWorldMat;
		inFile.read(reinterpret_cast<char*>(&TriggerWorldMat), sizeof(_mat));

		TriggerInfo.WorldMat = TriggerWorldMat;

		CCheckPoint* pCheckPoint = dynamic_cast<CCheckPoint*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_CheckPoint"), &TriggerInfo));
		if (not pCheckPoint)
		{
			MSG_BOX("트리거 생성 실패.");
			return E_FAIL;
		}

		m_CheckPoint.push_back(pCheckPoint);
	}

	inFile.close();

	return S_OK;
}

void CInfiltrationGame::Reset_Play(_float fTimeDelta)
{
	if (m_isReset == false)
	{
		if (CUI_Manager::Get_Instance()->Get_Hp().x <= 0)
		{
			m_fResurrectionTime += fTimeDelta;
			if (m_fResurrectionTime > 2.f)
			{
				m_isReset = true;
				m_fResurrectionTime = 0.f;
			}
		}
	}
	else
	{

		//m_pPlayerTransform->Set_Matrix(m_CheckPointMatrix);
		m_pPlayerTransform->Set_Position(_vec3(m_CheckPointMatrix._41, m_CheckPointMatrix._42, m_CheckPointMatrix._43));
		m_isReset = false;
		m_fResurrectionTime = 0.f;

	}
}

void CInfiltrationGame::Release_DeadObjects()
{
	for (auto& pGuardList : m_GuardList) {
		for (auto& iter = pGuardList.begin(); iter != pGuardList.end();) {
			if ((*iter)->isDead()) {
				Safe_Release(*iter);
				iter = pGuardList.erase(iter);
			}
			else
				iter++;
		}
	}

	for (auto& pTowerList : m_GuardTowerList) {
		for (auto& iter = pTowerList.begin(); iter != pTowerList.end();) {
			if ((*iter)->isDead()) {
				Safe_Release(*iter);
				iter = pTowerList.erase(iter);
			}
			else
				iter++;
		}
	}

	for (auto& it = m_CheckPoint.begin(); it != m_CheckPoint.end();)
	{
		if ((*it)->isDead())
		{
			Safe_Release(*it);
			it = m_CheckPoint.erase(it);
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

	for (auto& List : m_GuardList)
	{
		for (auto& pGuard : List)
		{
			Safe_Release(pGuard);
		}
		List.clear();
	}
	m_GuardList.clear();

	for (auto& List : m_GuardTowerList)
	{
		for (auto& pGuardTower : List)
		{
			Safe_Release(pGuardTower);
		}
		List.clear();
	}
	m_GuardTowerList.clear();

	for (auto& pCheckPoint : m_CheckPoint)
	{
		Safe_Release(pCheckPoint);
	}
	m_CheckPoint.clear();

	Safe_Release(m_pPlayerTransform);
}
