#include "CescoGame.h"
#include "VTFMonster.h"
#include "Log.h"

CCescoGame::CCescoGame(_dev pDevice, _context pContext)
	:CGameObject(pDevice, pContext)
{
}

CCescoGame::CCescoGame(const CCescoGame& rhs)
	:CGameObject(rhs)
	, m_pPlayerTransform(rhs.m_pPlayerTransform)
{
	Safe_AddRef(m_pPlayerTransform);
}

HRESULT CCescoGame::Init_Prototype()
{
	m_pPlayerTransform = GET_TRANSFORM("Layer_Player", LEVEL_STATIC);
	Safe_AddRef(m_pPlayerTransform);

	return S_OK;
}

HRESULT CCescoGame::Init(void* pArg)
{
	//UP
	m_SpawnPositions.push_back(_vec3(-3000.f, 1.f, 30.f));
	//DOWN
	m_SpawnPositions.push_back(_vec3(-3000.f, 1.f, -30.f));
	//RIGHT
	m_SpawnPositions.push_back(_vec3(-2970.f, 1.f, 0.f));
	//LEFT
	m_SpawnPositions.push_back(_vec3(-3030.f, 1.f, 0.f));

	random_device rand;
	m_RandomNumber = _randNum(rand());

	CLog::LOG_DESC LogDesc{};
	
	for (_uint i = 0; i < m_SpawnPositions.size(); i++)
	{
		LogDesc.WorldMatrix = _mat::CreateScale(3.f, 3.f, 10.f);
		if (i <= 1)
		{
			LogDesc.WorldMatrix *= _mat::CreateRotationY(XMConvertToRadians(90.f));
		}
		_vec3 vSpawnPos = m_SpawnPositions[i];

		vSpawnPos.y = 18.f;
		LogDesc.WorldMatrix.Position_vec3(vSpawnPos);

		if (FAILED(m_pGameInstance->Add_Layer(m_pGameInstance->Get_CurrentLevelIndex(), TEXT("Layer_Log"), TEXT("Prototype_GameObject_Log_Object"), &LogDesc)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

void CCescoGame::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_8,InputChannel::UI))
	{
		Create_Hook();
	}
	if (m_pGameInstance->Key_Down(DIK_9))
	{
		m_fTimeLimit = 0.f;
	
	}

	m_fTimeLimit -= fTimeDelta;

	if (m_fTimeLimit <= -1.f)
	{
		Kill();
		return;
	}

	m_fMonsterSpawnTime += fTimeDelta;

#pragma region SpawnMonster

	if (m_fMonsterSpawnTime >= 1.f)
	{
		_vec3 vSpawnPos = m_SpawnPositions[0];
		vSpawnPos.z -= 1.f;
		if (FAILED(Create_CommonMonster(TEXT("Prototype_VTFModel_Scorpion"), vSpawnPos, TEXT("Prototype_GameObject_Scorpion_Object"))))
			return;

		vSpawnPos = m_SpawnPositions[1];
		vSpawnPos.z += 1.f;
		if (FAILED(Create_CommonMonster(TEXT("Prototype_VTFModel_Redant"), vSpawnPos, TEXT("Prototype_GameObject_RedAnt_Object"))))
			return;

		m_iMonsterSpawnCount++;
		m_fMonsterSpawnTime = 0.f;
	}

#pragma endregion

#pragma region SpawnLarva

	if (m_iMonsterSpawnCount % 10 == 1 && m_fMonsterSpawnTime == 0.f)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_VTFModel_Larva");
		VTFMonsterDesc.pPlayerTransform = m_pPlayerTransform;

		_int iNumSpawn{};
		_randInt RandomDir(0, 3);
		_randInt RandomPos(5, 20);
		_randInt RandomSymbol(0, 1);

		while (iNumSpawn < 5)
		{
			_int iSymbol = RandomSymbol(m_RandomNumber);
			if (iSymbol == 0)
			{
				iSymbol = -1;
			}

			_vec3 vPos{};
			_bool HasPosition{};

			_int iRandom = RandomDir(m_RandomNumber);
			if (iRandom <= 1)
			{
				vPos = m_SpawnPositions[iRandom];
				vPos.x += RandomPos(m_RandomNumber) * iSymbol;
				for (auto& Pair : m_LarvaPositions)
				{
					if (Pair.second == vPos)
					{
						HasPosition = true;
						break;
					}
				}
			}
			else
			{
				vPos = m_SpawnPositions[iRandom];
				vPos.z += RandomPos(m_RandomNumber) * iSymbol;
				for (auto& Pair : m_LarvaPositions)
				{
					if (Pair.second == vPos)
					{
						HasPosition = true;
						break;
					}
				}
			}

			if (m_LarvaPositions.size() == 128)
			{
				break;
			}

			if (HasPosition)
				continue;

			VTFMonsterDesc.vPosition = vPos;
			CVTFMonster* pMonster = reinterpret_cast<CVTFMonster*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Larva_Object"), &VTFMonsterDesc));
			m_Monsters.push_back(pMonster);
			m_LarvaPositions.emplace(pMonster->Get_ID(), vPos);
			iNumSpawn++;
		}
	}

#pragma endregion
	//실패 조건
	if (m_Monsters.size() > m_iMonsterLimit || m_fTimeLimit <= 0.f)
	{
		for (auto& pMonster : m_Monsters)
		{
			pMonster->Set_Damage(pMonster->Get_HP());
		}
	}

	for (auto& pMonster : m_Monsters)
	{
		pMonster->Tick(fTimeDelta);
	}

	for (auto& pHook : m_vecHooks)
	{
		pHook->Tick(fTimeDelta);
	}
	m_bHadDragging = false;

	_bool bDrag{};
	_bool bCollision{};

	for (auto& pHooks : m_vecHooks)
	{
		if (pHooks->Get_Dragging())
		{
			bDrag = true;
			break;
		}
	}

	if (!bDrag)
	{
		for (auto& pHooks : m_vecHooks)
		{
				
			if (pHooks->Get_HadCollision())
			{
				m_pCurrent_DraggingHook = pHooks;
				pHooks->Set_Dragging(true);
				break;
			}
		}
	}

	if (m_pCurrent_DraggingHook)
	{
		m_pPlayerTransform->Set_Position(_vec3(m_pCurrent_DraggingHook->Get_Position()));
	}

	Release_DeadObjects();
}

void CCescoGame::Late_Tick(_float fTimeDelta)
{
	for (auto& pMonster : m_Monsters)
	{
		pMonster->Late_Tick(fTimeDelta);
	}

	for (auto& pHook : m_vecHooks)
	{
		pHook->Late_Tick(fTimeDelta);
	}
}

HRESULT CCescoGame::Create_CommonMonster(const wstring& strModelTag, _vec3 SpawnPosition, const wstring& strPrototypeTag)
{
	CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
	VTFMonsterDesc.strModelTag = strModelTag;
	VTFMonsterDesc.vPosition = SpawnPosition;
	VTFMonsterDesc.pPlayerTransform = m_pPlayerTransform;
	CVTFMonster* pVTFMonster = reinterpret_cast<CVTFMonster*>(m_pGameInstance->Clone_Object(strPrototypeTag, &VTFMonsterDesc));
	m_Monsters.push_back(pVTFMonster);

	return S_OK;
}

HRESULT CCescoGame::Create_Hook()
{
	CHook::HOOK_DESC HookDesc{};
	_randInt RandomDir(0, 3);
	_randInt RandomCount(1, 3);
	_randInt RandomCountNum(1, 7);

	HookDesc.WorldMatrix = _mat::CreateScale(2.f, 2.f, 1.5f);

	for (int i = 0; i < 2; i++)
	{
		_int iDirNum = RandomDir(m_RandomNumber);
		vector<int> vecHookPos;
		_int iCountNum = RandomCount(m_RandomNumber);
		_vec3 vSpawnPos = m_SpawnPositions[iDirNum];
		
		switch (iDirNum)
		{
		case 0:	//위
		{
			while (iCountNum < 4)
			{
				_vec3 vHookPos = vSpawnPos;
				_int iPosNum = RandomCountNum(m_RandomNumber);
				vector<int>::iterator it = std::find(vecHookPos.begin(), vecHookPos.end(), iPosNum);
				if (it == vecHookPos.end())
				{
					vecHookPos.push_back(iPosNum);
					vHookPos.x = vSpawnPos.x - 20.f + 6.f * iPosNum;
					vHookPos.y += 1.f;
					HookDesc.WorldMatrix.Position_vec3(vHookPos);
					HookDesc.vLookat = _vec4(0.f, 0.f, -1.f, 1.f);
					CHook* pHook = dynamic_cast<CHook*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Hook_Object"), &HookDesc));
					m_vecHooks.push_back(pHook);
					iCountNum++;
				}
			}
		break;
		}
		case 1:	//아래
		{
			while (iCountNum < 5)
			{
				_int iPosNum = RandomCountNum(m_RandomNumber);
				_vec3 vHookPos = vSpawnPos;
				vector<int>::iterator it = std::find(vecHookPos.begin(), vecHookPos.end(), iPosNum);
				if (it == vecHookPos.end())
				{
					vecHookPos.push_back(iPosNum);
					vHookPos.x = vSpawnPos.x - 20.f + 6.f * iPosNum;
					vHookPos.y += 1.f;
					HookDesc.WorldMatrix.Position_vec3(vHookPos);
					HookDesc.vLookat = _vec4(0.f, 0.f, 1.f, 1.f);
					CHook* pHook = dynamic_cast<CHook*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Hook_Object"), &HookDesc));
					m_vecHooks.push_back(pHook);
					iCountNum++;
				}
			}
		}
		break;
		case 2:	//오른쪽
		{
			while (iCountNum < 5)
			{
				_int iPosNum = RandomCountNum(m_RandomNumber);
				_vec3 vHookPos = vSpawnPos;
				vector<int>::iterator it = std::find(vecHookPos.begin(), vecHookPos.end(), iPosNum);
				if (it == vecHookPos.end())
				{
					vecHookPos.push_back(iPosNum);
					vHookPos.z = vSpawnPos.z - 20.f + 6.f * iPosNum;
					vHookPos.y += 1.f;
					HookDesc.WorldMatrix.Position_vec3(vHookPos);
					HookDesc.vLookat = _vec4(-1.f, 0.f, 0.f, 1.f);
					CHook* pHook = dynamic_cast<CHook*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Hook_Object"), &HookDesc));
					m_vecHooks.push_back(pHook);
					iCountNum++;
				}
			}
			break;
		}
		case 3:	//왼쪽		
		{
			while (iCountNum < 5)
			{
				_int iPosNum = RandomCountNum(m_RandomNumber);
				_vec3 vHookPos = vSpawnPos;
				vector<int>::iterator it = std::find(vecHookPos.begin(), vecHookPos.end(), iPosNum);
				if (it == vecHookPos.end())
				{
					vecHookPos.push_back(iPosNum);
					vHookPos.z = vSpawnPos.z - 20.f + 6.f * iPosNum;
					vHookPos.y += 1.f;
					HookDesc.WorldMatrix.Position_vec3(vHookPos);
					HookDesc.vLookat = _vec4(1.f,0.f,0.f,1.f);
					CHook* pHook = dynamic_cast<CHook*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Hook_Object"), &HookDesc));
					m_vecHooks.push_back(pHook);
					iCountNum++;
				}
			}
			break;
		}
	}
}

	return S_OK;
}

void CCescoGame::Release_DeadObjects()
{
	for (auto& it = m_Monsters.begin(); it != m_Monsters.end();)
	{
		if ((*it)->isDead())
		{
			if ((*it)->Get_ModelTag() == TEXT("Prototype_VTFModel_Larva"))
			{
				m_LarvaPositions.erase((*it)->Get_ID());
			}
			Safe_Release(*it);
			it = m_Monsters.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (auto& it = m_vecHooks.begin(); it != m_vecHooks.end();)
	{
		if ((*it)->isDead())
		{
			if (m_pCurrent_DraggingHook == *it)
			{
				m_pCurrent_DraggingHook = nullptr;
				m_pGameInstance->Attack_Player(nullptr, 0, MonAtt_Hook_End);
			}
			Safe_Release(*it);
			it = m_vecHooks.erase(it);
		}
		else
		{
			++it;
		}
	}
}

CCescoGame* CCescoGame::Create(_dev pDevice, _context pContext)
{
	CCescoGame* pInstance = new CCescoGame(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCescoGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCescoGame::Clone(void* pArg)
{
	CCescoGame* pInstance = new CCescoGame(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCescoGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCescoGame::Free()
{
	__super::Free();

	for (auto& pMonster : m_Monsters)
	{
		Safe_Release(pMonster);
	}
	m_Monsters.clear();

	for (auto& pHook: m_vecHooks)
	{
		Safe_Release(pHook);
	}
	m_vecHooks.clear();

	m_LarvaPositions.clear();

	Safe_Release(m_pPlayerTransform);
}
